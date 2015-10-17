set(Wt_Commons_Library_SRCS
  compositeresource.cpp 
  migratedbo.cpp 
  quitresource.cpp 
  wform.cpp 
  wglyphicon.cpp 
  whtmltxt.cpp
  wtr.cpp
  whtmltemplateslocalizedstrings.cpp
  cookieslawdisclaimer.cpp
  wserverconfigurationreader.cpp
)
set(HAVE_WT_JSON_SERIALIZE Off CACHE BOOL "Build with Wt JSON serialize support (Default: Off)")
if(HAVE_WT_JSON_SERIALIZE)
  set(Wt_Commons_Library_SRCS
    ${Wt_Commons_Library_SRCS} 
    json_object.cpp 
  )
endif(HAVE_WT_JSON_SERIALIZE)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(Wt_Commons_Library STATIC ${Wt_Commons_Library_SRCS})
add_subdirectory(tests)
