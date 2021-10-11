cmake_minimum_required(VERSION 3.20)
project(emerson_thermostat C)

set(CMAKE_C_STANDARD 99)

include_directories(emerson_thermostat include)
add_executable(emerson_thermostat main.c src/statemachine.c src/thermostat.c src/menu.c)
