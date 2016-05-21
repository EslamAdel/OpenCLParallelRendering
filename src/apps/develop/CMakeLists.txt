## -------------------------- DEVELOP SETTING----------------------------------##

message("Develop Setting...")
### Application name
set( OCL_MGPUVR_EXECUTABLE "oclMGPUVR_Develop" )

### Application source/header/ui files
set( OCL_MGPUVR_HEADER_FILES CommandLineParser.h RenderingWindow.h )
set( OCL_MGPUVR_SOURCE_FILES oclMGPUVR.cpp CommandLineParser.cpp RenderingWindow.cpp )
set( OCL_MGPUVR_APP_FORMS RenderingWindow.ui )
qt5_wrap_ui( OCL_MGPUVR_APP_FORMS_MOC ${OCL_MGPUVR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_EXECUTABLE}
    ${OCL_MGPUVR_SOURCE_FILES}
    ${OCL_MGPUVR_HEADER_FILES}
    ${OCL_MGPUVR_APP_FORMS}
)


### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_EXECUTABLE} ${CLPAREN_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_EXECUTABLE} Core Widgets OpenGL Gui )

## Generate the run script.
## Add the shell script to run the application.
#set( OCL_MGPUVR_RUNSCRIPT ${OCL_MGPUVR_EXECUTABLE}.sh )

#set( APP_EXECUTABLE ${OCL_MGPUVR_EXECUTABLE} )
#configure_file( ${APPLICATION_RUNSCRIPT_RAW} ${OCL_MGPUVR_RUNSCRIPT} @ONLY)

#install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${OCL_MGPUVR_RUNSCRIPT} DESTINATION bin
#    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ
#    GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )


# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_EXECUTABLE} DESTINATION bin )


## -------------------------- BENCHMARK SETTING--------------------------------##

message("Benchmarking Executable Setting...")

### Benchmarking Application name
set( OCL_MGPUVR_BENCHMARK_EXECUTABLE "oclMGPUVR_Benchmark" )

## BENCHMARKING source/header files.
set( OCL_MGPUVR_BENCHMARK_HEADER_FILES ${OCL_MGPUVR_HEADER_FILES}
                                        CommandLineParser_Benchmark.h )

set( OCL_MGPUVR_BENCHMARK_SOURCE_FILES
                                oclMGPUVR_Benchmark.cpp
                                CommandLineParser.cpp
                                CommandLineParser_Benchmark.cpp
                                RenderingWindow.cpp )

set( OCL_MGPUVR_BENCHMARK_APP_FORMS ${OCL_MGPUVR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE}
    ${OCL_MGPUVR_BENCHMARK_HEADER_FILES}
    ${OCL_MGPUVR_BENCHMARK_SOURCE_FILES}
    ${OCL_MGPUVR_BENCHMARK_APP_FORMS}
)

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} ${CLPAREN_LIBRARY_BENCHMARK} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} Core Widgets OpenGL Gui )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} DESTINATION bin )

###------------------------------- VIRTUAL SETTING --------------------------------###


message("Virtual Setting...")
### Application name
set( OCL_MGPUVR_VIRTUAL_EXECUTABLE "oclMGPUVR_Virtual" )

### Application source/header/ui files
set( OCL_MGPUVR_VIRTUAL_HEADER_FILES CommandLineParser.h RenderingWindow.h
                                     CommandLineParser_Virtual.h
                                     )
set( OCL_MGPUVR_VIRTUAL_SOURCE_FILES oclMGPUVR_Virtual.cpp
                                     CommandLineParser.cpp
                                     CommandLineParser_Virtual.cpp
                                     RenderingWindow.cpp )

set( OCL_MGPUVR_VIRTUAL_APP_FORMS RenderingWindow.ui )
qt5_wrap_ui( OCL_MGPUVR_VIRTUAL_APP_FORMS_MOC ${OCL_MGPUVR_VIRTUAL_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_VIRTUAL_EXECUTABLE}
    ${OCL_MGPUVR_VIRTUAL_HEADER_FILES}
    ${OCL_MGPUVR_VIRTUAL_SOURCE_FILES}
    ${OCL_MGPUVR_VIRTUAL_APP_FORMS}
)

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_VIRTUAL_EXECUTABLE} ${CLPAREN_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_VIRTUAL_EXECUTABLE} Core Widgets OpenGL Gui )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_VIRTUAL_EXECUTABLE} DESTINATION bin )

## -------------------------- Calibrate SETTING--------------------------------##

message("Calibrate Executable Setting...")

### Calibrate Application name
set( OCL_MGPUVR_CALIBRATE_EXECUTABLE "oclMGPUVR_Calibrate" )

set( OCL_MGPUVR_CALIBRATE_HEADER_FILES CommandLineParser.h )
set( OCL_MGPUVR_CALIBRATE_SOURCE_FILES
                                oclMGPUVR_Calibrate.cpp
                                CommandLineParser.cpp )

### Generate the executable
add_executable( ${OCL_MGPUVR_CALIBRATE_EXECUTABLE}
    ${OCL_MGPUVR_CALIBRATE_SOURCE_FILES}
)


### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_CALIBRATE_EXECUTABLE} ${CLPAREN_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_CALIBRATE_EXECUTABLE} Core )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_CALIBRATE_EXECUTABLE} DESTINATION bin )
