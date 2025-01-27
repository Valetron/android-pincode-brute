#include <string>
#include <iostream>

#include <spdlog/spdlog.h>
#include <libusb-1.0/libusb.h>

#include "UsbHandler.h"

namespace
{
enum class ExcludeVendors
{
    Linux = 0x1d6b
};
}

UsbHandler::UsbHandler()
{
    int rc {0};
#if defined(LIBUSB_API_VERSION) && (LIBUSB_API_VERSION >= 0x0100010A)
    rc = libusb_init_context(m_context, nullptr, nullptr);
#else
    rc = libusb_init(m_context);
#endif
    if (rc != 0)
    {
        SPDLOG_ERROR("Error init libusb context: ", rc);
    }
}

UsbHandler::~UsbHandler()
{
    libusb_exit(*m_context);
}

void UsbHandler::listUsbDevices() const
{
    ssize_t rc {0};
    libusb_device** devs {nullptr};

    rc = libusb_get_device_list(nullptr, &devs);
    if (rc < 0)
    {
        SPDLOG_ERROR("No USB devices found");
        return;
    }
    else
    {
        for (size_t i = 0; i < rc; ++i)
        {
            libusb_device_descriptor desc {};
            auto res = libusb_get_device_descriptor(devs[i], &desc);

            if (static_cast<decltype(desc.idVendor)>(ExcludeVendors::Linux) == desc.idVendor)
                continue;

            SPDLOG_INFO("idVendor - {}, idProduct - {}, manufacturer - {}", desc.idVendor, desc.idProduct, desc.bDeviceProtocol);
        }
    }

    std::string userInput {};
    std::cout << "Choose USB: ";
    std::getline(std::cin, userInput);

    // FIXME: core dumped
    libusb_free_device_list(devs, 1); // NOTE: https://libusb.sourceforge.io/api-1.0/group__libusb__dev.html#gac0fe4b65914c5ed036e6cbec61cb0b97
}
