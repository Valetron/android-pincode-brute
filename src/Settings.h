#ifndef SETTINGS_H
#define SETTINGS_H

#include <chrono>
#include <string>
#include <unordered_map>

using namespace std::chrono;
using namespace std::chrono_literals;

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
    milliseconds inputSpeed {50ms};
    seconds sleepInterval {30s};
    const seconds keepScreenOnTime {5s};
    TKeyCoordinates keyCoords { {'0', {}},
                                                {'1', {}},
                                                {'2', {}},
                                                {'3', {}},
                                                {'4', {}},
                                                {'5', {}},
                                                {'6', {}},
                                                {'7', {}},
                                                {'8', {}},
                                                {'9', {}},
                                                {'o', {}} };
};

#endif // SETTINGS_H
