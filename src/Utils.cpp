#include "Utils.h"

#include <algorithm>
#include <sstream>

#include <spdlog/spdlog.h>

TKeyCoordinates validateKeys(const std::string& pins)
{
    if (pins.empty())
    {
        SPDLOG_ERROR("Pins settings empty");
        return {};
    }

    TKeyCoordinates result;
    std::stringstream ss(pins);
    std::string token;

    // Разделяем строку по запятым
    while (std::getline(ss, token, ','))
    {
        if (token.empty())
            continue;

        // Пример токена: "0:100;200"
        const auto key = token[0];  // первая буква — это цифра или 'e'

        const size_t colonPos = token.find(':');
        const size_t semicolonPos = token.find(';');

        if (colonPos == std::string::npos || semicolonPos == std::string::npos)
            continue;

        int x = std::stoi(token.substr(colonPos + 1, semicolonPos - colonPos - 1));
        int y = std::stoi(token.substr(semicolonPos + 1));

        result[key] = {x, y};
    }

    return result;
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


///////////////////////////
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vector>
#include <string>

namespace po = boost::program_options;

struct Entry {
    std::string key;
    int a;
    int b;
};

// Обязательная специализация для boost::program_options
void validate(boost::any& v,
              const std::vector<std::string>& values,
              Entry*, int)
{
    // проверка, что значение одно
    po::validators::check_first_occurrence(v);
    const std::string& s = po::validators::get_single_string(values);

    // ожидаем строку вида "ключ:число;число"
    std::vector<std::string> parts;
    boost::split(parts, s, boost::is_any_of(":;"));

    if (parts.size() != 3)
        throw po::validation_error(po::validation_error::invalid_option_value, s);

    Entry e;
    e.key = parts[0];
    e.a   = std::stoi(parts[1]);
    e.b   = std::stoi(parts[2]);

    v = e;
}

int main(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("entry,e", po::value<std::vector<Entry>>()->multitoken(), "custom entries");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("entry")) {
        auto entries = vm["entry"].as<std::vector<Entry>>();
        for (auto& e : entries) {
            std::cout << e.key << " => " << e.a << ", " << e.b << "\n";
        }
    }
}
