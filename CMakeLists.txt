cmake_minimum_required(VERSION 3.16)
project(MotorSim)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

include_directories(include)

add_executable(MotorSim
    src/main.cpp
    src/Engine.cpp
    src/Piston.cpp
)

target_link_libraries(MotorSim sfml-graphics sfml-window sfml-system)
