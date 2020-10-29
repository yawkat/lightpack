cmake_minimum_required(VERSION 3.11)
project(lightpack C)

set(CMAKE_C_STANDARD 11)

SET(MCU "atmega328p")
if (MCU STREQUAL "atmega328p")
    SET(F_CPU "16000000")
elseif (MCU STREQUAL "atmega8")
    SET(F_CPU "8000000")
else()
    message(SEND_ERROR "Unknown MCU")
endif ()

SET(CMAKE_C_COMPILER avr-gcc)
SET(CMAKE_CXX_COMPILER avr-g++)

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_executable(main.o main.c
        hsv.h
        light_ws2812.h light_ws2812.c ws2812_config.h
        uart.c uart.h
        fast_hsv2rgb.c fast_hsv2rgb.h)

add_custom_command(
        OUTPUT "main.hex"
        DEPENDS main.o
        COMMAND /usr/bin/avr-objcopy -j .text -j .data -O ihex main.o main.hex)

add_custom_target(
        flash
        COMMAND /usr/bin/avrdude -p ${MCU} -c stk500 -U flash:w:main.hex:i -P /dev/ttyACM0
        DEPENDS main.hex)
