#include <array>
#include <thread>
#include <chrono>

#include <spdlog/spdlog.h>

#include "Brute.h"
#include "HIDPacket.h"

using namespace std::chrono_literals;

Brute::Brute(uint16_t vendorId, uint16_t productId) : m_hidId(std::rand())
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
                        m_hidId, 0, nullptr, 0, 0);
    if (rc < 0)
        SPDLOG_ERROR("Error unregister HID: {}", rc);

    libusb_close(m_handle);
}

bool Brute::connect()
{
    connectStylus(HIDPackets::TouchSreenPacket);
    setPosition(5000, 5000);
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
                        m_hidId, data.size(), nullptr, 0, 0);

    if (rc < 0)
    {
        SPDLOG_ERROR("...");
        return false;
    }

    rc = libusb_control_transfer(m_handle,
                                 static_cast<uint8_t>(TransferType::Out),
                                 static_cast<uint8_t>(ControlRequests::ACCESSORY_SET_HID_REPORT_DESC),
                                 m_hidId, 0, const_cast<uint8_t*>(data.data()), data.size(), 0);

    if (rc < 0)
    {
        SPDLOG_ERROR("...");
        return false;
    }

    return res;
}

void Brute::setPosition(int x, int y)
{
    const auto xMsb = (int((x >> 8) & 0xff));
    const auto yMsb = (int((y >> 8) & 0xff));

    const auto xLsb = (int(x & 0xff));
    const auto yLsb = (int(y & 0xff));

    SPDLOG_INFO("Msb ({0:x}, {0:x}), Lsb = ({0:x}, {0:x})", xMsb, yMsb, xLsb, yLsb);
    std::vector<uint8_t> data {0x01,
                                        static_cast<uint8_t>(xLsb),
                                        static_cast<uint8_t>(xMsb),
                                        static_cast<uint8_t>(yLsb),
                                        static_cast<uint8_t>(yMsb)};

    std::this_thread::sleep_for(2s);
    // , , m_hidId, 0, data
    auto rc = libusb_control_transfer(m_handle,
                                            static_cast<uint8_t>(TransferType::Out),
                                            static_cast<uint8_t>(ControlRequests::ACCESSORY_SEND_HID_EVENT),
                                            m_hidId, 0, data.data(), data.size(), 0);

    SPDLOG_INFO("send 0 rc = {}", rc);

    return;
    std::vector<uint8_t> aaa {0x01, 0, 0, 0, 0};
    std::vector<uint8_t> bbb {0x00, 0, 0, 0, 0};

    for (auto i = 0; i < 2; ++i)
    {
        rc = libusb_control_transfer(m_handle,
                                     static_cast<uint8_t>(TransferType::Out),
                                     static_cast<uint8_t>(ControlRequests::ACCESSORY_SEND_HID_EVENT),
                                     m_hidId, 0, aaa.data(), aaa.size(), 0);
        SPDLOG_INFO("send 1 rc = {}", rc);
        rc = libusb_control_transfer(m_handle,
                                     static_cast<uint8_t>(TransferType::Out),
                                     static_cast<uint8_t>(ControlRequests::ACCESSORY_SEND_HID_EVENT),
                                     m_hidId, 0, bbb.data(), bbb.size(), 0);
        SPDLOG_INFO("send 2 rc = {}", rc);
    }

    std::this_thread::sleep_for(1500ms);

    for (auto i = 0; i < 3; ++i)
    {
        rc = libusb_control_transfer(m_handle,
                                     static_cast<uint8_t>(TransferType::Out),
                                     static_cast<uint8_t>(ControlRequests::ACCESSORY_SEND_HID_EVENT),
                                     m_hidId, 0, aaa.data(), aaa.size(), 0);
        SPDLOG_INFO("send 1 rc = {}", rc);
        rc = libusb_control_transfer(m_handle,
                                     static_cast<uint8_t>(TransferType::Out),
                                     static_cast<uint8_t>(ControlRequests::ACCESSORY_SEND_HID_EVENT),
                                     m_hidId, 0, bbb.data(), bbb.size(), 0);
        SPDLOG_INFO("send 2 rc = {}", rc);
        std::this_thread::sleep_for(100ms);
    }

}

void Brute::start()
{

}
