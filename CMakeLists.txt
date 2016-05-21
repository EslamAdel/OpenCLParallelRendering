################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
# OpenCL-based Multi-GPU Volume Scales is a light framework for multi-threaded
# volume rendering of large volume on multiple GPUs connected to the system.
################################################################################

CMAKE_MINIMUM_REQUIRED( VERSION 3.1 FATAL_ERROR)
set( CMAKE_CXX_STANDARD 11 )

##-------------------- Use settings at: https://cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
# With these settings you will be able to
# execute your programs from the build tree and
# they will find the shared libraries in the build
# tree and also the shared libraries outside your project,
# when installing all executables and shared libraries
# will be relinked, so they will find all libraries they need.
########################################################################
########################################################################
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")
########################################################################
########################################################################


# clparen: Open(CL)-based Multi-GPU (Par)allel (Ren)dering.
PROJECT( clparen )

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


message("Project Executables will be built to <project_dir>/bin")
