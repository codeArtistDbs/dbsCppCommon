cmake_minimum_required(VERSION 3.5.1)
project(Common)
set(CMAKE_INSTALL_PREFIX ../../)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -ggdb -Wall -fPIC") #-O0 -ggdb
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 11)
include_directories(../../include)
file(GLOB_RECURSE SRC_LIST ./Common.cpp)
add_library(Common STATIC ${SRC_LIST})
