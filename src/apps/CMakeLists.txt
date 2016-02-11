################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################


### Application name
set( OCL_MGPUVR_EXECUTABLE "oclMGPUVR" )

### Application source/header/ui files
set( OCL_MGPUVR_HEADER_FILES RenderingWindow.h )
set( OCL_MGPUVR_SOURCE_FILES oclMGPUVS.cpp RenderingWindow.cpp )
set( OCL_MGPUVR_APP_FORMS RenderingWindow.ui )
qt5_wrap_ui( OCL_MGPUVS_APP_FORMS_MOC ${OCL_MGPUVR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_EXECUTABLE}
    ${OCL_MGPUVR_SOURCE_FILES}
    ${OCL_MGPUVR_HEADER_FILES}
    ${OCL_MGPUVR_APP_FORMS}
)



### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_EXECUTABLE} ${OCL_MGPUVS_LIBRARY} )


### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_EXECUTABLE} Core Widgets OpenGL Gui )


### Copy all the kernels to the build directory to avoid the kernel loading mess
foreach( kernel_name ${OCL_MGPUVS_KERNELS} )
    file( COPY ${CMAKE_SOURCE_DIR}/src/${kernel_name} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    message(${CMAKE_SOURCE_DIR})
endforeach( )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_EXECUTABLE} DESTINATION bin )
