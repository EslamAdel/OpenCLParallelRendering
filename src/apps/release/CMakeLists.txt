
## -------------------------- Gui SETTING--------------------------------##

message("Featured Gui Executable Setting...")

### Benchmarking Application name
set( OCL_MGPUVR_FEATURED_EXECUTABLE "oclMGPUVR_Gui" )

## BENCHMARKING source/header files.
set( OCL_MGPUVR_FEATURED_HEADER_FILES RenderingWindow_Gui.h )

set( OCL_MGPUVR_FEATURED_SOURCE_FILES
                                oclMGPUVR_Gui.cpp
                                RenderingWindow_Gui.cpp )

set( OCL_MGPUVR_FEATURED_APP_FORMS RenderingWindow_Gui.ui )


qt5_wrap_ui( OCL_MGPUVR_FEATURED_APP_FORMS_MOC ${OCL_MGPUVR_FEATURED_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_FEATURED_EXECUTABLE}
    ${OCL_MGPUVR_FEATURED_HEADER_FILES}
    ${OCL_MGPUVR_FEATURED_SOURCE_FILES}
    ${OCL_MGPUVR_FEATURED_APP_FORMS}
)



### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_FEATURED_EXECUTABLE} ${OCL_MGPUVS_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_FEATURED_EXECUTABLE} Core Widgets OpenGL Gui )


# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_FEATURED_EXECUTABLE} DESTINATION bin )


