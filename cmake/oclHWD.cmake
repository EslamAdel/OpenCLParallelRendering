#####################################################################
# Copyright © 2011-2015,
# Marwan Abdellah: <abdellah.marwan@gmail.com>
#####################################################################

find_path(OCLHWD_INCLUDE_DIR clHWD.h
	PATHS
	/usr/local/include/oclHWD/
        /usr/include/oclHWD
        /usr/local/include/oclHWD
        /opt/local/include/oclHWD
)

    find_library(OCLHWD_LIBRARY NAMES oclHWD
      PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
    )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(OCLHWD DEFAULT_MSG
                                      OCLHWD_LIBRARY
                                      OCLHWD_INCLUDE_DIR)

    if(OCLHWD_FOUND)
        message(STATUS "OCLHWD Found")
        message("-- OCLHWD include directory : ${OCLHWD_INCLUDE_DIR}")
        message("-- OCLHWD library directory : ${OCLHWD_LIBRARY}")
        include_directories(${OCLHWD}/OCLHWD)
        link_libraries(${OCLHWD_LIBRARY})
    else(OCLHWD_FOUND)
        MESSAGE(FATAL_ERROR "OCLHWD NOT Found")
    endif(OCLHWD_FOUND)




