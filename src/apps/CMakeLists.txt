################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################


# Configure the shell script to export LD_LIBRARY_PATH=/path/to/qt/libraries
get_target_property(QtCore_location Qt5::Core  LOCATION)
get_filename_component( MyQtLibsDir ${QtCore_location} DIRECTORY)
set( APPLICATION_RUNSCRIPT_RAW ${CMAKE_CURRENT_SOURCE_DIR}/runscript.sh.in )

add_subdirectory(develop)
add_subdirectory(release)


### Copy all the kernels to the build directory to avoid the kernel loading mess
foreach( kernel_file ${OCL_MGPUVS_KERNEL_FILES} )
    file( COPY ${kernel_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message( "Copying ..." )
    message(${kernel_file})
    message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

endforeach( )


