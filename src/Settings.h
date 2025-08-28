#ifndef SETTINGS_H
#define SETTINGS_H

#include <chrono>
#include <string>
#include <unordered_map>

#include <spdlog/fmt/ostr.h>

using namespace std::chrono;

struct Point;
using TKeyCoordinates = std::unordered_map<char, Point>;

struct Point final
{
    int x {0};
    int y {0};
};

struct Settings final
{
    std::string firstPin {"0"};
    std::string lastPin {"9999"};
    milliseconds inputSpeed {50};
    seconds sleepInterval {30};
    seconds keepScreenOnTime {3};
    TKeyCoordinates keyCoords { {'0', {} },
                                {'1', {} },
                                {'2', {} },
                                {'3', {} },
                                {'4', {} },
                                {'5', {} },
                                {'6', {} },
                                {'7', {} },
                                {'8', {} },
                                {'9', {} },
                                {'o', {} } };

    template<typename OStream>
    friend OStream& operator<<(OStream& os, const Settings& obj)
    {
        fmt::format_to(std::ostream_iterator<char>(os), "\nFirst pin: {}", obj.firstPin);
        fmt::format_to(std::ostream_iterator<char>(os), "\nLast pin: {}", obj.lastPin);
        fmt::format_to(std::ostream_iterator<char>(os), "\nInput key speed: {}", obj.inputSpeed);
        fmt::format_to(std::ostream_iterator<char>(os), "\nSleep interval between input: {}", obj.sleepInterval);
        fmt::format_to(std::ostream_iterator<char>(os), "\nTime to prevent screen turned off: {}", obj.keepScreenOnTime);
        // fmt::format_to(std::ostream_iterator<char>(os), "\nKey coordinates on screen: {}", obj.keyCoords);

        return os;
    }

};

#endif // SETTINGS_H
