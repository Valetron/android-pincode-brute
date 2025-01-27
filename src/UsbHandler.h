#ifndef USBHANDLER_H
#define USBHANDLER_H

#include <libusb-1.0/libusb.h>

class UsbHandler final
{
public:
    UsbHandler();
    ~UsbHandler();
    void listUsbDevices() const;

private:
    libusb_context** m_context {nullptr};
};

#endif // USBHANDLER_H
