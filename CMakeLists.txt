cmake_minimum_required(VERSION 3.0)
project(audio)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXTRA_CXX_FLAGS Wall pedantic)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE AUDIO_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(audio ${AUDIO_SOURCES})
target_link_libraries(audio portaudio pthread)

add_subdirectory(test)
