include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_library(GuLinux_Qt_Commons STATIC zoomableimage.cpp functional.cpp)
add_subdirectory(tests)