cmake_minimum_required(VERSION 3.10)
project(HotUrl)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -g -pthread")
aux_source_directory(. DIR_SRCS)
add_executable(HotUrl ${DIR_SRCS})
