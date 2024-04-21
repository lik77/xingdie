cmake_minimum_required(VERSION 3.12)
project(c__)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})
set(CMAKE_CXX_STANDARD 11)

add_executable(c__ xiaoneisai.cpp)

target_link_libraries(c__ ${OpenCV_LIBS})
