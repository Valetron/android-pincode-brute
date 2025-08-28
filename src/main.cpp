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

        options_description desc("Allowed options");
        desc.add_options()
            ("help", "Print help")
            ("first", value<std::string>(&firtsPin)->default_value(stgs.firstPin), "First pin")
            ("last", value<std::string>(&lastPin)->default_value(stgs.lastPin), "Last pin")
            ("ispeed", value<uint32_t>(&ispeed)->default_value(stgs.inputSpeed.count()), "pin input speed in ms")
            ("sleep", value<uint32_t>(&sleep)->default_value(stgs.sleepInterval.count()), "wait latency in sec")
            ("keep", value<uint32_t>(&keepScreen)->default_value(stgs.keepScreenOnTime.count()), "keep screen turned on in seconds")
            // ("coords", value<TKeyCoordinates>(&stgs.keyCoords), "key coordinates")
            ("debug", "Enable debug messages")
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

        // if (!vm.count("coords") || !validateKeys(stgs.keyCoords))
        // {

        // }

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
