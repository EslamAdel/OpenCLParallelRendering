################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
# OpenCL-based Multi-GPU Volume Scales is a light framework for multi-threaded
# volume rendering of large volume on multiple GPUs connected to the system.
################################################################################

CMAKE_MINIMUM_REQUIRED( VERSION 3.1 )
set( CMAKE_CXX_STANDARD 11 )


# OpenCL-based Multi-GPU 'Volume Scales'
PROJECT( parallel-rendering-multi-gpu )

# Add the CMAKE directory into the source tree
SET( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

if( APPLE )
    # Check if environment variable QTDIR is set.
    # Extra security for windows environment as well.
    if( DEFINED ENV{QTDIR} )
        set( CMAKE_PREFIX_PATH $ENV{QTDIR} ${CMAKE_PREFIX_PATH} )
    else( )
        message( FATAL_ERROR "ERROR: Environment variable QTDIR is not set. Please locate your Qt folder QT5DIR." )
    endif ( )
endif( )

INCLUDE( OpenCL )
include( Qt )
include( OCLHWDL )

# Add the src directory to the CMake tree and set the source/header/ui files
add_subdirectory( src )


