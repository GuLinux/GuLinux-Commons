file(GLOB_RECURSE Wt_Commons_Library_SRCS *.cpp)
add_library(Wt_Commons_Library STATIC ${Wt_Commons_Library_SRCS})
add_subdirectory(tests)