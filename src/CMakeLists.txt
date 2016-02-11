################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Add the binary directory to the include and set off the mocs.
include_directories( ${PROJECT_BINARY_DIR} )



message( "${FRAMEWORK_SOURCE}" )






### Adding the source files to the CMake source tree
SET( OCL_MGPUVS_SOURCE_FILES
    framework/ParallelRendering.cpp
    framework/RenderingNode.cpp
    framework/TaskRender.cpp
    framework/TaskCollect.cpp
    framework/TaskComposite.cpp
    framework/CompositingNode.cpp
    framework/CLContext.cpp
    framework/TaskMakePixmap.cpp
    framework/virtual/VirtualParallelRendering.cpp
    framework/virtual/VirtualRenderingNode.cpp
    framework/virtual/VirtualCompositingNode.cpp
    framework/virtual/VirtualTaskCollect.cpp
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
    profiling/ProfilingExterns.cpp
    system/oclUtils.cpp
    system/Utilities.cpp
    system/Logger.cpp
)

### Adding the header files to the CMake source tree
SET( OCL_MGPUVS_HEADER_FILES
    framework/CLContext.h
    framework/CLContext.ipp
    framework/CompositingNode.h
    framework/RenderingNode.h
    framework/TaskCollect.h
    framework/TaskComposite.h
    framework/TaskMakePixmap.h
    framework/TaskRender.h
    framework/virtual/VirtualCompositingNode.h
    framework/virtual/VirtualParallelRendering.h
    framework/virtual/VirtualRenderingNode.h
    framework/virtual/VirtualTaskCollect.h
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

# Create a library with the main compoments of the framework and link the apps
# against it later to reduce the compilation time
set( OCL_MGPUVS_LIBRARY "oclMGPUVS" )



add_library( ${OCL_MGPUVS_LIBRARY}
    ${OCL_MGPUVS_SOURCE_FILES}
    ${OCL_MGPUVS_HEADER_FILES}
    ${OCL_MGPUVS_KERNELS} )

target_include_directories( ${OCL_MGPUVS_LIBRARY}  PUBLIC framework )

set_property( TARGET ${OCL_MGPUVS_LIBRARY} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/lib${OCL_MGPUVS_LIBRARY} )



install( TARGETS ${OCL_MGPUVS_LIBRARY} DESTINATION lib )
install( FILES ${OCL_MGPUVS_HEADER_FILES} DESTINATION include )
install( FILES ${OCL_MGPUVS_KERNELS} DESTINATION share )

# Add the applications
add_subdirectory( apps )


