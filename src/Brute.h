#ifndef BRUTE_H
#define BRUTE_H

#include <memory>

#include "UsbHandler.h"

class Brute
{
private:
    std::unique_ptr<UsbHandler> m_usbHandler {nullptr};
};

#endif // BRUTE_H
