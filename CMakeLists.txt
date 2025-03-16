cmake_minimum_required(VERSION 3.10)
set(QT_CREATOR_SKIP_CONAN_SETUP ON)

project(android-pin-brute LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(VRFindBoost)
include(VRFindSpdlog)
include(VRFindLibusb)
include(VRFindLibhid)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

# target_compile_options(${PROJECT_NAME} PUBLIC -fno-rtti)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog ${libusb_LIBRARIES} Boost::program_options)
