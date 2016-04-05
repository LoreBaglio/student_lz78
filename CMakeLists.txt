cmake_minimum_required(VERSION 3.5)
project(student_lz78)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES lz_command.c file_io.c file_io.h compressor.c compressor.h decompressor.c decompressor.h)
add_executable(lz_command ${SOURCE_FILES} lz_command.c file_io.c file_io.h compressor.c compressor.h decompressor.c decompressor.h)

