#include <chrono>
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>
#include <boost/program_options.hpp>

#include "Brute.h"
#include "UsbHandler.h"
#include "Settings.h"

using namespace boost::program_options;

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%T](%l): %v");

    SPDLOG_INFO("val = {}", SPDLOG_ACTIVE_LEVEL);

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

        spdlog::info("first {}, last {}, input speed {}, sleep interval {}",
                                                            stgs.firstPin,
                                                            stgs.lastPin,
                                                            stgs.inputSpeed,
                                                            stgs.sleepInterval);

        const auto incrementPin = [](std::string currentPin, const std::string& endPin) -> std::string {
            int length = currentPin.length();
            for (int i = length - 1; i >= 0; --i) {
                if (currentPin[i] < '9') {
                    currentPin[i]++;
                    break;
                } else {
                    currentPin[i] = '0';
                }
            }

            // Если длина изменилась (например, 0999 -> 1000), добавляем ведущую цифру
            if (currentPin.length() < endPin.length()) {
                currentPin = '1' + currentPin;
            }

            return currentPin;
        };

        auto currentPin = stgs.firstPin;
        while (currentPin != stgs.lastPin)
        {
            SPDLOG_DEBUG("curr = {}", currentPin);

            try
            {
                const auto length = currentPin.length();
                bool increase = true;
                for (auto i = length - 1; i >= 0 && increase; --i)
                {
                    auto& ch = currentPin.at(i);
                    if (ch < '9')
                    {
                        ch++;
                        increase = false;
                    }
                    else
                    {
                        ch = '0';
                    }
                }

                if (increase)
                    currentPin = '1' + currentPin;
            }
            catch (const std::exception& ex)
            {
                SPDLOG_ERROR("Error: {}", ex.what());
            }
        }

        // TODO: check last pin

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
