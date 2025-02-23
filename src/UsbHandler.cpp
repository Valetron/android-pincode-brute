#include <array>
#include <chrono>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>
#include <libusb-1.0/libusb.h>

#include "UsbHandler.h"

using namespace std::chrono_literals;

namespace
{
enum class TVendorId : uint16_t
{
    Xiaomi = 0x2717,
    Samsung = 0x04E8
};
}

UsbHandler::UsbHandler()
{
    int rc {0};
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x0100010A)
    rc = libusb_init_context(nullptr, nullptr, nullptr);
#else
    rc = libusb_init(nullptr);
#endif
    if (rc != 0)
    {
        SPDLOG_ERROR("Error init libusb context: ", rc);
    }
}

UsbHandler::~UsbHandler()
{
    libusb_exit(nullptr);
}

TDeviceIds UsbHandler::findDevice() const
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

    return std::make_pair(vendorId, productId);

    // libusb_device_handle* handle {nullptr};
    // SPDLOG_INFO("Opening device: {:x}:{:x}", device.Vendor, device.Product);
    // handle = libusb_open_device_with_vid_pid(nullptr, device.Vendor, device.Product);
    // if (!handle)
    // {
    //     SPDLOG_ERROR("Failed open device");
    //     return;
    // }
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
