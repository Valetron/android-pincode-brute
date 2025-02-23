#include <array>

#include <spdlog/spdlog.h>

#include "Brute.h"

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
