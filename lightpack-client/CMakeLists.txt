cmake_minimum_required(VERSION 3.11)
project(lightpack_client C)

set(CMAKE_C_STANDARD 11)

add_executable(lightpack_client main.c hsv.h
        kiss_fft130/kiss_fft.c kiss_fft130/kiss_fft.h
        kiss_fft130/_kiss_fft_guts.h
        kiss_fft130/kiss_fftr.c kiss_fft130/kiss_fftr.h
        spectrum.c send.c send.h spectrum.c spectrum.h scroll.c scroll.c scroll.h)
target_link_libraries(lightpack_client "pulse")
target_link_libraries(lightpack_client "pulse-simple")
target_link_libraries(lightpack_client "m")
