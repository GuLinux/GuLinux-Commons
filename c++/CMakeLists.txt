cmake_minimum_required(VERSION 2.8.11)
project(GuLinux-Commons-c++)
enable_testing()
set(CMAKE_CXX_STANDARD 11)
find_package(Boost REQUIRED)
set(GuLinux_c++_Commons_SRCS settings.cpp demangle.cpp)

if(NOT WIN32)
  set(GuLinux_c++_Commons_SRCS ${GuLinux_c++_Commons_SRCS} backtrace.cpp)
else(NOT WIN32)
  add_definitions(-DWIN32)
endif(NOT WIN32)

add_library(GuLinux_c++_Commons STATIC ${GuLinux_c++_Commons_SRCS})

option(enable_gulinux_commons_cpp_testing "Enable GuLinux-Commons c++ module testing" Off)
if(enable_gulinux_commons_cpp_testing)
  add_subdirectory(tests)
endif(enable_gulinux_commons_cpp_testing)
  
