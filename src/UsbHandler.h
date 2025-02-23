#ifndef USBHANDLER_H
#define USBHANDLER_H

#include <utility>

#include <libusb-1.0/libusb.h>

using TDeviceIds = std::pair<uint16_t, uint16_t>;

class UsbHandler final
{
public:
    UsbHandler();
    ~UsbHandler();
    TDeviceIds findDevice() const;

private:
    bool isVendorValid(uint16_t vendorId) const;
    void printDeviceInfo(libusb_device* dev, const libusb_device_descriptor& desc) const;

private:
    // libusb_context** m_context {nullptr}; // DELETEME:
};

#endif // USBHANDLER_H
