#ifndef BRUTE_H
#define BRUTE_H

#include <string>
#include <utility>
#include <cstdint>

#include <libusb-1.0/libusb.h>

#include "Utils.h"

enum class TransferType
{
    In = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR,
    Out = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR
};

enum class ControlRequests
{
    ACCESSORY_GET_PROTOCOL = 51,
    ACCESSORY_GET_INFO = 52,
    ACCESSORY_START_MODE = 53,
    ACCESSORY_REGISTER_HID = 54,
    ACCESSORY_UNREGISTER_HID = 55,
    ACCESSORY_SET_HID_REPORT_DESC = 56,
    ACCESSORY_SEND_HID_EVENT = 57
};

class Brute
{
public:
    Brute(uint16_t vendorId, uint16_t productId);

private:
    bool connect();
    std::pair<bool, std::string> checkProtocol();

private:
    Protocol m_protocol {Protocol::None};
    libusb_device_handle* m_handle {nullptr};
};

#endif // BRUTE_H
