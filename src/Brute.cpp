#include <array>

#include <spdlog/spdlog.h>

#include "Brute.h"
#include "HIDPacket.h"

Brute::Brute(uint16_t vendorId, uint16_t productId)
{
    SPDLOG_DEBUG("Brute class created with params: protocol {}, vendor id - {}, product id - {}",
                 m_protocol, vendorId, productId);
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

Brute::~Brute()
{
    // if (m_isHidRegistered)
        // TODO: unregister hid
    const auto rc = libusb_control_transfer(m_handle,
                        static_cast<uint8_t>(TransferType::Out),
                        static_cast<uint8_t>(ControlRequests::ACCESSORY_UNREGISTER_HID),
                        142, 0, nullptr, 0, 0);
    if (rc < 0)
        SPDLOG_ERROR("Error unregister HID: {}", rc);

    libusb_close(m_handle);
}

bool Brute::connect()
{
    connectStylus(HIDPackets::TouchSreenPacket);
    return true;
}

std::pair<bool, std::string> Brute::checkProtocol()
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
    {
        return std::make_pair(false, "Device doesn't support AOAv2 protocol");
    }

    return std::make_pair(true, "AOAv2 protocol supported");
}

bool Brute::connectStylus(const std::vector<uint8_t>& data)
{
    bool res {true};

    auto rc = libusb_control_transfer(m_handle,
                        static_cast<uint8_t>(TransferType::Out),
                        static_cast<uint8_t>(ControlRequests::ACCESSORY_REGISTER_HID),
                        142, data.size(), nullptr, 0, 0);

    if (rc < 0)
    {
        SPDLOG_ERROR("...");
        return false;
    }

    // TODO: 142 поменять на случайное число в поле класса
    rc = libusb_control_transfer(m_handle,
                                 static_cast<uint8_t>(TransferType::Out),
                                 static_cast<uint8_t>(ControlRequests::ACCESSORY_SET_HID_REPORT_DESC),
                                 142, 0, const_cast<uint8_t*>(data.data()), data.size(), 0);

    if (rc < 0)
    {
        SPDLOG_ERROR("...");
        return false;
    }

    return res;
}

void Brute::start()
{

}
