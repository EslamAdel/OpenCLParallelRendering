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

set_property( TARGET ${OCL_MGPUVR_EXECUTABLE} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/bin )

### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_EXECUTABLE} ${OCL_MGPUVS_LIBRARY} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_EXECUTABLE} Core Widgets OpenGL Gui )

# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_EXECUTABLE} DESTINATION bin )




## -------------------------- BENCHMARK SETTING--------------------------------##

message("Benchmarking Executable Setting...")

### Benchmarking Application name
set( OCL_MGPUVR_BENCHMARK_EXECUTABLE "oclMGPUVR_benchmark" )

## BENCHMARKING source/header files.
set( OCL_MGPUVR_BENCHMARK_HEADER_FILES ${OCL_MGPUVR_HEADER_FILES}
                                        CommandLineParser_benchmark.h )

set( OCL_MGPUVR_BENCHMARK_SOURCE_FILES
                                oclMGPUVR_benchmark.cpp
                                CommandLineParser.cpp
                                CommandLineParser_benchmark.cpp
                                RenderingWindow.cpp )

set( OCL_MGPUVR_BENCHMARK_APP_FORMS ${OCL_MGPUVR_APP_FORMS} )

### Generate the executable
add_executable( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE}
    ${OCL_MGPUVR_BENCHMARK_HEADER_FILES}
    ${OCL_MGPUVR_BENCHMARK_SOURCE_FILES}
    ${OCL_MGPUVR_BENCHMARK_APP_FORMS}
)



### Link the application against the oclMGPUVR library
target_link_libraries( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} ${OCL_MGPUVS_LIBRARY_BENCHMARK} )

### Make sure to use the required Qt modules
qt5_use_modules( ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} Core Widgets OpenGL Gui )


# Install the executable (generate with 'make install' in the bin directory)
install( TARGETS ${OCL_MGPUVR_BENCHMARK_EXECUTABLE} DESTINATION bin )



###--------------------------------------------------------------------------###

