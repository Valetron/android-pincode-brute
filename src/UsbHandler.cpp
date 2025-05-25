#include <array>
#include <chrono>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>
#include <libusb-1.0/libusb.h>

#include "UsbHandler.h"
#include "Types.h"

using namespace std::chrono_literals;

namespace
{
enum class TVendorId : uint16_t
{
    Xiaomi = 0x2717,
    Samsung = 0x04E8
};
}

UsbHandler::UsbHandler() : m_hidId(std::rand())
{
    int rc {0};
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x0100010A)
    rc = libusb_init_context(nullptr, nullptr, nullptr);
#else
    rc = libusb_init(nullptr);
#endif
    if (rc != 0)
        throw std::runtime_error("Error init libusb context");

    findDevice();
}

UsbHandler::~UsbHandler()
{
    // if (m_isHidRegistered)
    // TODO: unregister hid
    const auto rc = libusb_control_transfer(m_handle,
                                            static_cast<uint8_t>(TransferType::Out),
                                            static_cast<uint8_t>(ControlRequests::ACCESSORY_UNREGISTER_HID),
                                            m_hidId, 0, nullptr, 0, 0);
    if (rc < 0)
        SPDLOG_ERROR("Error unregister HID: {}", rc);

    libusb_close(m_handle);

    libusb_exit(nullptr);
}

TDeviceIds UsbHandler::findDevice()
{
    libusb_device** devs {nullptr};
    bool isDeviceFound {false};

    uint16_t vendorId {0};
    uint16_t productId {0};

    while (!isDeviceFound)
    {
        auto rc = libusb_get_device_list(nullptr, &devs);
        if (rc < 0)
        {
            SPDLOG_ERROR("No USB devices found");
        }
        else
        {
            for (size_t i = 0; i < rc; ++i)
            {
                libusb_device_descriptor desc {};
                auto res = libusb_get_device_descriptor(devs[i], &desc);

                if (isVendorValid(desc.idVendor))
                {
                    printDeviceInfo(devs[i], desc);
                    isDeviceFound = true;

                    vendorId = desc.idVendor;
                    productId = desc.idProduct;

                    break;
                }
            }

            libusb_free_device_list(devs, 1);
        }

        if (!isDeviceFound)
            std::this_thread::sleep_for(3s);
    }

    m_handle = libusb_open_device_with_vid_pid(nullptr, vendorId, productId);
    if (!m_handle)
        throw std::runtime_error("Error open device");

    bool res {false};
    std::string resStr {};
    std::tie(res, resStr) = checkProtocol();

    if (!res)
        throw std::runtime_error(resStr);

    SPDLOG_DEBUG("{}", resStr);
}

// TODO: make templates
bool UsbHandler::isVendorValid(uint16_t vendorId) const
{
    switch (static_cast<TVendorId>(vendorId))
    {
    case TVendorId::Xiaomi:
    case TVendorId::Samsung:
        return true;
    default:
        return false;
    }
}

void UsbHandler::printDeviceInfo(libusb_device* dev, const libusb_device_descriptor& desc) const
{
    libusb_device_handle* handle {nullptr};
    std::string product {"Unknown"};
    std::string manufacturer {"Unknown"};
    std::string serialNumber {"Unknown"};
    std::array<uint8_t, 256> strInfo {};

    auto rc = libusb_open(dev, &handle);
    if (0 == rc)
    {
        rc = libusb_get_string_descriptor_ascii(handle, desc.iProduct, strInfo.data(), strInfo.size());
        if (rc > 0)
            product.assign(reinterpret_cast<char*>(strInfo.data()), rc);

        rc = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, strInfo.data(), strInfo.size());
        if (rc > 0)
            manufacturer.assign(reinterpret_cast<char*>(strInfo.data()), rc);

        rc = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, strInfo.data(), strInfo.size());
        if (rc > 0)
            serialNumber.assign(reinterpret_cast<char*>(strInfo.data()), rc);

        if (handle)
            libusb_close(handle);
    }

    SPDLOG_INFO("Found device `{} {}`, serial - {}, id - {:x}:{:x}", manufacturer, product, serialNumber, desc.idVendor, desc.idProduct);
}

std::pair<bool, std::string> UsbHandler::checkProtocol()
{
    std::array<uint8_t, 8> data {0};
    const auto res = libusb_control_transfer(m_handle,
                                             static_cast<uint8_t>(TransferType::In),
                                             static_cast<uint8_t>(ControlRequests::ACCESSORY_GET_PROTOCOL),
                                             0, 0, data.data(), data.size(), 0); // TODO: limit timeout?

    if (LIBUSB_ERROR_TIMEOUT == res
        || LIBUSB_ERROR_PIPE == res
        || LIBUSB_ERROR_NO_DEVICE == res
        || LIBUSB_ERROR_BUSY == res
        || LIBUSB_ERROR_INVALID_PARAM == res
        || res > data.size())
    {
        return std::make_pair(false, "Error get protocol");
    }

    uint16_t protocol {0};
    for (int i = 0; i < res; ++i)
        protocol += data.at(i);

    if (protocol != static_cast<uint16_t>(Protocol::AOA2))
        return std::make_pair(false, "Device doesn't support AOAv2 protocol");

    return std::make_pair(true, "AOAv2 protocol supported");
}

int UsbHandler::sendEvent(TransferType direction, ControlRequests request, uint16_t index, const uint8_t* data, uint16_t dataLen, uint32_t timeout)
{
    return libusb_control_transfer(m_handle,
                                   static_cast<uint8_t>(direction),
                                   static_cast<uint8_t>(request),
                                   m_hidId,
                                   index,
                                   const_cast<unsigned char*>(data),
                                   dataLen,
                                   timeout);
}
