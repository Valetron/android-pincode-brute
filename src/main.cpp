#include <chrono>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>
#include <boost/program_options.hpp>

// #include "Brute.h"
// #include "UsbHandler.h"
#include "Settings.h"

using namespace boost::program_options;

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%T](%l): %v");

    try
    {
        Settings stgs {};
        uint32_t ispeed {0};
        uint32_t sleep {0};
        bool isDebug {false};

        options_description desc("Allowed options");
        desc.add_options()
            ("help", "Print help")
            ("first", value<std::string>(&stgs.firstPin)->default_value(stgs.firstPin), "First pin")
            ("last", value<std::string>(&stgs.lastPin)->default_value(stgs.lastPin), "Last pin")
            // ("ispeed", value<uint32_t>(&ispeed)->default_value(stgs.inputSpeed.count()), "pin input speed in ms")
            // ("sleep", value<uint32_t>(&sleep)->default_value(stgs.sleepInterval.count()), "wait latency in sec")
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
        {

        }

        if (vm.count("coords"))
        {

        }

        // if ((stgs.lastPin < stgs.firstPin) || (stgs.firstPin.size() > stgs.lastPin.size()))
        //     std::swap(stgs.firstPin, stgs.lastPin);

        // TODO: validate pins

        SPDLOG_INFO("first pin {}, last pin {}, input speed {}, sleep interval {}",
                                                            stgs.firstPin,
                                                            stgs.lastPin,
                                                            stgs.inputSpeed,
                                                            stgs.sleepInterval);
    }
    catch (const std::exception& ex)
    {
        SPDLOG_ERROR(ex.what());
        return 0xBAD;
    }

    return 0;
}
