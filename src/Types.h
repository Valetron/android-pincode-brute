#ifndef TYPES_H
#define TYPES_H

extern "C"
{
#include <libusb-1.0/libusb.h>
}

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

enum class Protocol : char
{
    None = 0,
    AOA = 1,
    AOA2 = 2
};

#endif // TYPES_H
