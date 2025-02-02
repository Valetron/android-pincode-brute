#include <chrono>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>
#include <libusb-1.0/libusb.h>

#include "UsbHandler.h"

using namespace std::chrono_literals;

namespace
{
enum class ExcludeVendors
{
    Linux = 0x1d6b
};

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

void UsbHandler::listUsbDevices() const
{
    ssize_t rc {0};
    libusb_device** devs {nullptr};
    bool isDeviceFound {false};


    while (!isDeviceFound)
    {
        rc = libusb_get_device_list(nullptr, &devs);
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
                    // TODO: добавить строковую информацию
                    SPDLOG_INFO("Found device: idVendor - {:x}, idProduct - {:x}", desc.idVendor, desc.idProduct);
                    isDeviceFound = true;
                    break;
                }
            }

            libusb_free_device_list(devs, 1);
        }

        if (!isDeviceFound)
            std::this_thread::sleep_for(3s);
    }
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
