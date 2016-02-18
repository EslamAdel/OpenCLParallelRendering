################################################################################
### Copyrights (c) 2015
### Marwan Abdellah < abdellah.marwan@gmail.com >
################################################################################

### Set the root directory from CMake the command line arguments using the 
### following command cmake .. -DOCLHWDL_ROOT=<INSTALL PREFIX> or you can 
### pass the INSTALL PREFIX using the environment variable OCLHWD_ROOT by 
### the following command export OCLHWD_ROOT=<INSTALL PREFIX>

### Find the include directory 
find_path( OCLHWDL_INCLUDE_DIR
    NAMES
        oclHWDL/oclHWDL.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        $ENV{OCLHWD_ROOT}/include
        ${OCLHWDL_ROOT}/include
)

### Find the library 
find_library( OCLHWDL_LIBRARY
    NAMES
        oclHWDL
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        $ENV{OCLHWD_ROOT}/lib
        ${OCLHWDL_ROOT}/lib
)

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( OCLHWDL
    DEFAULT_MSG OCLHWDL_LIBRARY OCLHWDL_INCLUDE_DIR
)

if( OCLHWDL_FOUND )
    message( STATUS "oclHWDL Found" )
    message("-- oclHWDL include directory : ${OCLHWDL_INCLUDE_DIR}")
    message("-- oclHWDL library directory : ${OCLHWDL_LIBRARY}")
    include_directories( ${OCLHWDL_INCLUDE_DIR} ${OCLHWDL_INCLUDE_DIR}/oclHWDL )
    link_libraries( ${OCLHWDL_LIBRARY} )
else( OCLHWDL_FOUND )
    message(FATAL_ERROR "oclHWDL NOT Found")
endif( OCLHWDL_FOUND )
