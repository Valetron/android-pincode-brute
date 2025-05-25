#ifndef USBHANDLER_H
#define USBHANDLER_H

#include <string>
#include <vector>
#include <utility>

extern "C"
{
#include <libusb-1.0/libusb.h>
}

#include "Types.h"

using TDeviceIds = std::pair<uint16_t, uint16_t>;

class UsbHandler final
{
public:
    UsbHandler();
    ~UsbHandler();
    void run();

private:
    TDeviceIds findDevice();
    bool isVendorValid(uint16_t vendorId) const;
    std::pair<bool, std::string> checkProtocol();
    bool connectStylus(const std::vector<uint8_t>& data);
    void printDeviceInfo(libusb_device* dev, const libusb_device_descriptor& desc) const;
    int sendEvent(TransferType direction, ControlRequests request, uint16_t index, const uint8_t* data, uint16_t dataLen, uint32_t timeout);

private:
    const uint16_t m_hidId {0};
    bool m_isHidRegistered {false};
    libusb_device_handle* m_handle {nullptr};
};

#endif // USBHANDLER_H
