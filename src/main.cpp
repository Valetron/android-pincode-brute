#include <chrono>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>
#include <boost/program_options.hpp>

// #include "Brute.h"
// #include "UsbHandler.h"
#include "Settings.h"
#include "Utils.h"

using namespace boost::program_options;

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%T](%l): %v");

    try
    {
        Settings stgs {};
        uint32_t ispeed {0};
        uint32_t sleep {0};
        uint32_t keepScreen {0};
        std::string firtsPin {};
        std::string lastPin {};
        std::string keysCoords {};

        options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Print help")
            ("first,f", value<std::string>(&firtsPin)->default_value(stgs.firstPin), "First pin")
            ("last,l", value<std::string>(&lastPin)->default_value(stgs.lastPin), "Last pin")
            ("ispeed,i", value<uint32_t>(&ispeed)->default_value(stgs.inputSpeed.count()),
                "Pin input speed in ms")
            ("sleep,s", value<uint32_t>(&sleep)->default_value(stgs.sleepInterval.count()),
                "Seconds to wait between failed attempts")
            ("keep,k", value<uint32_t>(&keepScreen)->default_value(stgs.keepScreenOnTime.count()),
                "Interval in seconds to tap screen to prevent from turning off")
            ("coords,c", value<std::string>(&keysCoords)->multitoken(), "key coordinates")
            ("debug,d", "Enable debug messages")
            ;

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help") || argc < 2)
        {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("debug"))
            spdlog::set_level(spdlog::level::debug);

        if (vm.count("ispeed"))
            stgs.inputSpeed = std::chrono::milliseconds(ispeed);

        if (vm.count("sleep"))
            stgs.sleepInterval = std::chrono::seconds(sleep);

        if (vm.count("keep"))
            stgs.keepScreenOnTime = std::chrono::seconds(keepScreen);

        if (vm.count("coords"))
        {
            stgs.keyCoords = validateKeys(keysCoords);
            if (stgs.keyCoords.empty())
            {
                SPDLOG_ERROR("TODO");
                return 0xBAD;
            }
        }

        if (validatePin(firtsPin) && validatePin(lastPin))
        {
            // TODO: проверять на возрастание?
            stgs.firstPin = firtsPin;
            stgs.lastPin = lastPin;
        }

        // if ((stgs.lastPin < stgs.firstPin) || (stgs.firstPin.size() > stgs.lastPin.size()))
        //     std::swap(stgs.firstPin, stgs.lastPin);

        SPDLOG_INFO("Settings: {}", stgs);
    }
    catch (const std::exception& ex)
    {
        SPDLOG_ERROR(ex.what());
        return 0xBAD;
    }

    return 0;
}
