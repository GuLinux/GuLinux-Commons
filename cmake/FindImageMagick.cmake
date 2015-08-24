include(FindPkgConfig)
pkg_check_modules(ImageMagick ImageMagick++ REQUIRED)
add_definitions(${ImageMagick_CFLAGS})