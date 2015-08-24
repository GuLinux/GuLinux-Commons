include(FindPkgConfig)
pkg_check_modules(GraphicsMagick GraphicsMagick++ REQUIRED)
add_definitions(${GraphicsMagick_CFLAGS})