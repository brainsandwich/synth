cmake_minimum_required(VERSION 3.0)
project(uss)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXTRA_CXX_FLAGS Wall pedantic)

file(GLOB_RECURSE AUDIO_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(uss ${AUDIO_SOURCES})
target_include_directories(uss PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(Portaudio)
target_link_libraries(uss ${PORTAUDIO_LIBRARIES})
target_link_libraries(uss pthread)

add_subdirectory(test)
