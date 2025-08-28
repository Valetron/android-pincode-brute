#include <algorithm>
#include <spdlog/spdlog.h>

#include "Utils.h"

bool validateKeys(const TKeyCoordinates& pins)
{
    if (pins.empty())
    {
        SPDLOG_ERROR("Pins settings empty");
        return false;
    }

    // TODO: сделать проверку вводимых данных

    return true;
}

bool validatePin(const std::string& pin)
{
    if (pin.empty())
    {
        SPDLOG_ERROR("Pin is empty");
        return false;
    }

    return std::all_of(pin.cbegin(), pin.cend(), [](unsigned char c) { return std::isdigit(c); });
}
