################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Application name
set( OCL_USR_APP_EXECUTABLE "oclUSR" )

### Application source/header/ui files
set( OCL_USR_APP_SOURCES main.cpp 
    ColorMapWidget.cpp
    HoverPoints.cpp
    TFWidget.cpp
    MainWindow.cpp
    TransferFunctionEditor.cpp
    oclUSRWindow.cpp
    CommandLineParser.cpp
    )
set( OCL_USR_APP_HEADERS 
    ColorMapWidget.h
    TFWidget.h
    HoverPoints.h
    Utilities.h
    oclUSRWindow.h
    MainWindow.h
    TransferFunctionEditor.cpp
    CommandLineParser.h
    )
set( OCL_USR_APP_FORMS 
    TransferFunctionEditor.ui
    MainWindow.ui
    oclUSRWindow.ui
    )
qt5_wrap_ui( OCL_USR_APP_FORMS_MOC ${OCL_USR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_USR_APP_EXECUTABLE}
    ${OCL_USR_APP_SOURCES}
    ${OCL_USR_APP_HEADERS}
    ${OCL_USR_APP_FORMS_MOC}
    )

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_USR_APP_EXECUTABLE} ${CLPAREN_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_USR_APP_EXECUTABLE} Core Widgets OpenGL Gui )



# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_USR_APP_EXECUTABLE} DESTINATION bin )
