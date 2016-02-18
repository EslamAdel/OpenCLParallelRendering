################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

add_subdirectory(develop)
add_subdirectory(release)


### Copy all the kernels to the build directory to avoid the kernel loading mess
foreach( kernel_file ${OCL_MGPUVS_KERNEL_FILES} )
    file( COPY ${kernel_file} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message( "Copying ..." )
    message(${kernel_file})
    message(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

endforeach( )


