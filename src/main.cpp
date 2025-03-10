#include <spdlog/spdlog.h>

#include "Brute.h"
#include "UsbHandler.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%T](%l): %v");

    try
    {
        UsbHandler usb;

        uint16_t vendorId {0};
        uint16_t productId {0};
        std::tie(vendorId, productId) = usb.findDevice();

        Brute brute{vendorId, productId};
        brute.connect();

    }
    catch (const std::exception& ex)
    {
        SPDLOG_ERROR("{}", ex.what());
        return 0xBAD;
    }

    return 0;
}
