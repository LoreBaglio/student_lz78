cmake_minimum_required(VERSION 3.5)
project(student_lz78)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES lzw_codec.c compressor.c decompressor.c hash_table.c file_io.c encode.c )
add_executable(lzw_codec ${SOURCE_FILES})
target_link_libraries(lzw_codec m)


