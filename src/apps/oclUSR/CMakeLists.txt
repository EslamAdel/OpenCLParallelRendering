################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Application name
set( OCL_USR_APP_EXECUTABLE "oclUSR" )


### To use the CommandLineParser class as is.
include_directories( ../develop )

### Application source/header/ui files
set( OCL_USR_APP_SOURCES oclUSR.cpp
    ColorMapWidget.cpp
    HoverPoints.cpp
    TFWidget.cpp
    MainWindow.cpp
    TransferFunctionEditor.cpp
    oclUSRWindow.cpp
    CommandLineParser_SortFirst.cpp
    ../develop/CommandLineParser.cpp
    ../develop/CommandLineParser_Benchmark.cpp

    )
set( OCL_USR_APP_HEADERS 
    ColorMapWidget.h
    TFWidget.h
    HoverPoints.h
    Utilities.h
    oclUSRWindow.h
    MainWindow.h
    TransferFunctionEditor.cpp
    CommandLineParser_SortFirst.h
    ../develop/CommandLineParser.h
    ../develop/CommandLineParser_Benchmark.h

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




## -------------------------- BENCHMARK SETTING--------------------------------##

message("Benchmarking Executable Setting...")

### Benchmarking Application name
set( OCL_USR_BENCHMARK_EXECUTABLE "oclUSR_Benchmark" )

## BENCHMARKING source/header files.
set( OCL_USR_BENCHMARK_HEADER_FILES
    ../develop/CommandLineParser.h
    ../develop/CommandLineParser_Benchmark.h
    CommandLineParser_SortFirst.h )

set( OCL_USR_BENCHMARK_SOURCE_FILES
    oclUSR_Benchmark.cpp
    ../develop/CommandLineParser_Benchmark.cpp
    ../develop/CommandLineParser.cpp
    CommandLineParser_SortFirst.cpp )

set( OCL_USR_BENCHMARK_APP_FORMS ${OCL_MGPUVR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_USR_BENCHMARK_EXECUTABLE}
    ${OCL_USR_BENCHMARK_HEADER_FILES}
    ${OCL_USR_BENCHMARK_SOURCE_FILES}
    ${OCL_USR_BENCHMARK_APP_FORMS}
    )

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_USR_BENCHMARK_EXECUTABLE} ${CLPAREN_LIBRARY_BENCHMARK} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_USR_BENCHMARK_EXECUTABLE} Core Widgets OpenGL Gui )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_USR_BENCHMARK_EXECUTABLE} DESTINATION bin )
