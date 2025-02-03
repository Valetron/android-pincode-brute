#include <spdlog/spdlog.h>

#include "UsbHandler.h"

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%T](%l): %v");

    UsbHandler usb;
    usb.findDevice();

    return 0;
}
