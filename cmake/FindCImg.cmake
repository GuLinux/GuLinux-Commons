# - Try to find CImg
# Once done this will define
#  CImg_FOUND - System has CImg
#  CImg_INCLUDE_DIRS - directories with CImg headers

find_path(CImg_INCLUDE_DIR CImg.h PATH_SUFFIXES CImg )
find_path(CImg_plugins_INCLUDE_DIR plugins/jpeg_buffer.h PATH_SUFFIXES CImg )
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CImg  DEFAULT_MSG CImg_INCLUDE_DIR CImg_plugins_INCLUDE_DIR)
mark_as_advanced(CImg_INCLUDE_DIR CImg_plugins_INCLUDE_DIR )
set(CImg_INCLUDE_DIRS ${CImg_INCLUDE_DIR} ${CImg_plugins_INCLUDE_DIR} )