################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Add the binary directory to the include and set off the mocs.
include_directories( ${PROJECT_BINARY_DIR} )



### Adding the source files to the CMake source tree
SET( OCL_MGPUVS_SOURCE_FILES
    framework/ParallelRendering.cpp
    framework/CLRenderer.cpp
    framework/TaskRender.cpp
    framework/TaskCollect.cpp
    framework/TaskComposite.cpp
    framework/CLCompositor.cpp
    framework/CLContext.cpp
    framework/TaskMakePixmap.cpp
    framework/virtual/VirtualParallelRendering.cpp
    framework/virtual/VirtualCLRenderer.cpp
    framework/virtual/VirtualCLCompositor.cpp
    framework/virtual/VirtualTaskCollect.cpp
    framework/Transformation.cpp
    auxillary/VolumeUtilities.cpp
    data/Image.cpp
    data/TransferFunction.cpp
    data/Volume.cpp
    geometry/Vector2.cpp
    geometry/Vector3.cpp
    geometry/Vector4.cpp
    opencl/CLFrame.cpp
    opencl/CLImage2D.cpp
    opencl/CLImage2DArray.cpp
    opencl/CLRenderingKernel.cpp
    opencl/CLRewindFrameKernel.cpp
    opencl/CLTransferFunction.cpp
    opencl/CLVolume.cpp
    opencl/CLXRayCompositingKernel.cpp
    opencl/CLXRayRenderingKernel.cpp
    opencl/CLBuffer.cpp
    profiling/ProfilingExterns.cpp
    system/oclUtils.cpp
    system/Utilities.cpp
    system/Logger.cpp
)

### Adding the header files to the CMake source tree
SET( OCL_MGPUVS_HEADER_FILES
    framework/CLContext.h
    framework/CLContext.ipp
    framework/CLCompositor.h
    framework/CLRenderer.h
    framework/TaskCollect.h
    framework/TaskComposite.h
    framework/TaskMakePixmap.h
    framework/TaskRender.h
    framework/virtual/VirtualCLCompositor.h
    framework/virtual/VirtualParallelRendering.h
    framework/virtual/VirtualCLRenderer.h
    framework/virtual/VirtualTaskCollect.h
    framework/Transformation.h
    auxillary/half.hpp
    auxillary/VolumeUtilities.h
    common/Headers.hh
    common/Typedefs.hh
    data/Image.h
    data/ImageVariant.hh
    data/TransferFunction.h
    data/Volume.h
    data/VolumeVariant.hh
    geometry/Vector2.h
    geometry/Vector3.h
    geometry/Vector4.h
    opencl/CLFrame.h
    opencl/CLFrame.ipp
    opencl/CLImage2D.h
    opencl/CLImage2D.ipp
    opencl/CLImage2DArray.h
    opencl/CLRenderingKernel.h
    opencl/CLRewindFrameKernel.h
    opencl/CLTransferFunction.h
    opencl/CLVolume.h
    opencl/CLVolume.ipp
    opencl/CLXRayCompositingKernel.h
    opencl/CLXRayRenderingKernel.h
    opencl/CLBuffer.h
    profiling/Profiles.hh
    profiling/ProfilingExterns.h
    system/Logger.h
    system/LogLevel.hh
    system/oclUtils.h
    system/Utilities.h
    system/Colors.hh  )


### Adding the kernels to the CMake source tree
set( OCL_MGPUVS_KERNELS
    kernels/volumeRender.cl
    kernels/xray.cl
    kernels/xray_compositing.cl
)


### Include the directories to the CMake source tree

include_directories( framework )
include_directories( framework/virtual )
include_directories( auxillary )
include_directories( common )
include_directories( data )
include_directories( geometry )
include_directories( kernels )
include_directories( opencl )
include_directories( rendering )
include_directories( system )
include_directories( profiling )
 

### Add all the kernel files to a list in order to be able to copy them to the
### directories where the executables of the 'apps' will be generated.
### This will facilitate testing the implementation without installing any
### artifacts.
foreach( kernel_file ${OCL_MGPUVS_KERNELS} )
    list( APPEND OCL_MGPUVS_KERNEL_FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/${kernel_file}" )
    message(${CMAKE_CURRENT_SOURCE_DIR}/${kernel_file})
endforeach( )



### ---------------------- RELEASE LIBRARY SETTING ---------------------------###


# Create a library with the main compoments of the framework and link the apps
# against it later to reduce the compilation time
set( OCL_MGPUVS_LIBRARY "oclMGPUVS" )

add_library( ${OCL_MGPUVS_LIBRARY}
    ${OCL_MGPUVS_SOURCE_FILES}
    ${OCL_MGPUVS_HEADER_FILES}
    ${OCL_MGPUVS_KERNELS} )


set_property( TARGET ${OCL_MGPUVS_LIBRARY} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/lib${OCL_MGPUVS_LIBRARY} )

install( TARGETS ${OCL_MGPUVS_LIBRARY} DESTINATION lib )
install( FILES ${OCL_MGPUVS_HEADER_FILES} DESTINATION include )


### ---------------------- BENCHMARK LIBRARY SETTING ---------------------------###



# Create same library, but with activation of BENCHMARKING Definition.
set( OCL_MGPUVS_LIBRARY_BENCHMARK "oclMGPUVS_benchmark" )


# same library with BENCHMARKING defined.
add_library( ${OCL_MGPUVS_LIBRARY_BENCHMARK}
    ${OCL_MGPUVS_SOURCE_FILES}
    ${OCL_MGPUVS_HEADER_FILES}
    ${OCL_MGPUVS_KERNELS} )


target_compile_definitions( ${OCL_MGPUVS_LIBRARY_BENCHMARK} INTERFACE BENCHMARKING)

set_target_properties(  ${OCL_MGPUVS_LIBRARY_BENCHMARK}
                        PROPERTIES COMPILE_DEFINITIONS "BENCHMARKING" )
# BENCHMARKING defined
set_property( TARGET ${OCL_MGPUVS_LIBRARY_BENCHMARK} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/lib${OCL_MGPUVS_LIBRARY} )


install( TARGETS ${OCL_MGPUVS_LIBRARY_BENCHMARK} DESTINATION lib )
install( FILES ${OCL_MGPUVS_HEADER_FILES} DESTINATION include )




### ------------------------------------------------------------------------------###


install( FILES ${OCL_MGPUVS_KERNELS} DESTINATION share )

# Add the applications
add_subdirectory( apps )


