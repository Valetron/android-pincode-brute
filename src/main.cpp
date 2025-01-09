#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
    spdlog::set_pattern("[%l]: %v"); // TODO: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting

    return 0;
}
