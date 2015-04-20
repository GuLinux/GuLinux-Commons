set(Wt_Commons_Library_SRCS
  compositeresource.cpp 
  migratedbo.cpp 
  quitresource.cpp 
  wform.cpp 
  wglyphicon.cpp 
  whtmltemplateslocalizedstrings.cpp
)
set(HAVE_WT_JSON_SERIALIZE Off CACHE BOOL "Build with Wt JSON serialize support (Default: Off)")
if(HAVE_WT_JSON_SERIALIZE)
  set(Wt_Commons_Library_SRCS
    ${Wt_Commons_Library_SRCS} 
    json_object.cpp 
  )
endif(HAVE_WT_JSON_SERIALIZE)
add_library(Wt_Commons_Library STATIC ${Wt_Commons_Library_SRCS})
add_subdirectory(tests)
