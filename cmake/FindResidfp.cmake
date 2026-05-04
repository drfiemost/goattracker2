# rules for finding the residfp library

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_RESIDFP libresidfp>=0.1)

find_path(RESIDFP_INCLUDE_DIR residfp/residfp.h HINTS ${PC_RESIDFP_INCLUDEDIR} ${PC_RESIDFP_INCLUDE_DIRS})
find_library(RESIDFP_LIBRARY NAMES residfp HINTS ${PC_RESIDFP_LIBDIR} ${PC_RESIDFP_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Residfp DEFAULT_MSG RESIDFP_LIBRARY RESIDFP_INCLUDE_DIR)

mark_as_advanced(RESIDFP_INCLUDE_DIR RESIDFP_LIBRARY)

set(RESIDFP_INCLUDE_DIRS ${RESIDFP_INCLUDE_DIR})
set(RESIDFP_LIBRARIES ${RESIDFP_LIBRARY})
