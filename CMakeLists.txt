cmake_minimum_required(VERSION 3.5)
project(Crash)

add_executable(Crash WIN32 crash.cpp icon.rc)

find_package(Threads REQUIRED)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")
