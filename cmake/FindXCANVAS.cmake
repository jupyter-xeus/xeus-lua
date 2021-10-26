# - Try to find wren
# Once done this will define
# XCANVAS_FOUND - System has wren
# XCANVAS_INCLUDE_DIRS - The wren include directories
# XCANVAS_LIBRARIES - The libraries needed to use wren

find_package(PkgConfig)
find_path(XCANVAS_INCLUDE_DIR xcanvas/xcanvas.hpp)

find_library(XCANVAS_LIBRARY NAMES xcanvas)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and setXCANVAS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(XCANVAS  DEFAULT_MSG
                                 XCANVAS_LIBRARY XCANVAS_INCLUDE_DIR)

mark_as_advanced(XCANVAS_INCLUDE_DIR XCANVAS_LIBRARY )

set(XCANVAS_LIBRARIES ${XCANVAS_LIBRARY} )
set(XCANVAS_INCLUDE_DIRS ${XCANVAS_INCLUDE_DIR} )
