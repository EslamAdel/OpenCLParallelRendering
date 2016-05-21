
## -------------------------- Gui SETTING--------------------------------##
message("Featured Gui Executable Setting...")

### Benchmarking Application name
set( OCL_MGPUVR_GUI_EXECUTABLE "oclMGPUVR_Gui" )

## BENCHMARKING source/header files.
set( OCL_MGPUVR_GUI_HEADER_FILES
    RenderingWindow_Gui.h
    ../develop/CommandLineParser.h
    GPUListItemWidget.h
    GPUListView.h
    TaskInitializeFramework.h )

set( OCL_MGPUVR_GUI_SOURCE_FILES
    oclMGPUVR_Gui.cpp
    RenderingWindow_Gui.cpp
    ../develop/CommandLineParser.cpp
    GPUListView.cpp
    GPUListItemWidget.cpp
    TaskInitializeFramework.cpp )

set( OCL_MGPUVR_GUI_APP_FORMS RenderingWindow_Gui.ui )

set( OCL_MGPUVR_GUI_RESOURCES GpuIcons.qrc )

include_directories( ../develop)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

qt5_wrap_ui( OCL_MGPUVR_GUI_APP_FORMS_MOC ${OCL_MGPUVR_GUI_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_GUI_EXECUTABLE}
    ${OCL_MGPUVR_GUI_HEADER_FILES}
    ${OCL_MGPUVR_GUI_SOURCE_FILES}
    ${OCL_MGPUVR_GUI_APP_FORMS}
    ${OCL_MGPUVR_GUI_RESOURCES}
    )

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_GUI_EXECUTABLE} ${CLPAREN_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_GUI_EXECUTABLE} Core Widgets OpenGL Gui )

## Generate the run script.
## Add the shell script to run the application.
#set( OCL_MGPUVR_GUI_RUNSCRIPT ${OCL_MGPUVR_GUI_EXECUTABLE}.sh )

#set( APP_EXECUTABLE ${OCL_MGPUVR_GUI_EXECUTABLE} )
#configure_file( ${APPLICATION_RUNSCRIPT_RAW} ${OCL_MGPUVR_GUI_RUNSCRIPT} @ONLY)

#install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${OCL_MGPUVR_GUI_RUNSCRIPT} DESTINATION bin
#    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
#    GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_GUI_EXECUTABLE} DESTINATION bin )
