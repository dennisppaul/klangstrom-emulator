cmake_minimum_required(VERSION 3.12)

project(klangstrom-emulator)

if(NOT DEFINED KLST_ENV)
    message(FATAL_ERROR "KLST_ENV is not defined")
endif()

add_library(klangstrom-emulator 
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cores/sdl2/ArduinoFunctions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cores/sdl2/KlangstromEmulator.cpp
)

target_include_directories(klangstrom-emulator PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/emulator/cores/sdl2
)

target_compile_definitions(klangstrom-emulator PRIVATE KLST_ENV=${KLST_ENV})
target_link_libraries(klangstrom-emulator PRIVATE umgebung-lib)
