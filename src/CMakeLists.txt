################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Add the binary directory to the include and set off the mocs.
include_directories( ${PROJECT_BINARY_DIR} )



### Adding the source files to the CMake source tree
SET( OCL_MGPUVS_SOURCE_FILES
    framework/clparen.h
    framework/renderers/CLAbstractRenderer.cpp
    framework/compositors/CLAbstractCompositor.cpp
    framework/compositors/CLCompositorAccumulate.cpp
    framework/calibrators/AbstractCalibrator.cpp
    framework/calibrators/Calibrator.cpp
    framework/calibrators/TaskCalibrate.cpp
    framework/ParallelRendering.cpp
    framework/renderers/CLRenderer.cpp
    framework/TaskRender.cpp
    framework/TaskCollect.cpp
    framework/TaskComposite.cpp
    framework/compositors/CLCompositor.cpp
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
    data/BrickParameters.cpp
    geometry/Vector2.cpp
    geometry/Vector3.cpp
    geometry/Vector4.cpp
    opencl/cldata/CLFrame.cpp
    opencl/cldata/CLImage2D.cpp
    opencl/cldata/CLImage2DArray.cpp
    opencl/clkernels/CLRenderingKernel.cpp
    opencl/clkernels/CLCompositingKernel.cpp
    opencl/cldata/CLTransferFunction.cpp
    opencl/cldata/CLVolume.cpp
    opencl/clkernels/CLXRayCompositingKernel.cpp
    opencl/clkernels/CLXRayRenderingKernel.cpp
    opencl/clkernels/CLMaxIntensityProjectionRenderingKernel.cpp
    opencl/clkernels/CLMinIntensityProjectionRenderingKernel.cpp
    opencl/clkernels/CLAverageIntensityProjectionRenderingKernel.cpp
    opencl/clkernels/CLMinIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLMaxIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.cpp
    opencl/cldata/CLBuffer.cpp
    profiling/ProfilingExterns.cpp
    system/oclUtils.cpp
    system/Utilities.cpp
    system/Logger.cpp
)

### Adding the header files to the CMake source tree
SET( OCL_MGPUVS_HEADER_FILES
    framework/renderers/CLAbstractRenderer.h
    framework/compositors/CLAbstractCompositor.h
    framework/compositors/CLCompositorAccumulate.h
    framework/compositors/CLCompositorAccumulate.ipp
    framework/compositors/CLCompositor.h
    framework/compositors/CLCompositor.ipp
    framework/renderers/CLRenderer.h
    framework/renderers/CLRenderer.ipp
    framework/calibrators/AbstractCalibrator.h
    framework/calibrators/Calibrator.h
    framework/calibrators/Calibrator.ipp
    framework/calibrators/TaskCalibrate.h
    framework/ParallelRendering.h
    framework/TaskCollect.h
    framework/TaskComposite.h
    framework/TaskMakePixmap.h
    framework/TaskRender.h
    framework/virtual/VirtualCLCompositor.h
    framework/virtual/VirtualCLCompositor.ipp
    framework/virtual/VirtualParallelRendering.h
    framework/virtual/VirtualCLRenderer.h
    framework/virtual/VirtualCLRenderer.ipp
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
    data/BrickParameters.h
    data/BrickParameters.ipp
    geometry/Vector2.h
    geometry/Vector3.h
    geometry/Vector4.h
    geometry/Vector2.ipp
    geometry/Vector3.ipp
    geometry/Vector4.ipp
    opencl/cldata/clData.h
    opencl/cldata/CLFrame.h
    opencl/cldata/CLFrame.ipp
    opencl/cldata/CLImage2D.h
    opencl/cldata/CLImage2D.ipp
    opencl/cldata/CLImage2DArray.h
    opencl/cldata/CLImage2DArray.ipp
    opencl/cldata/CLTransferFunction.h
    opencl/cldata/CLVolume.h
    opencl/cldata/CLVolume.ipp
    opencl/clkernels/clKernel.h
    opencl/clkernels/CLXRayCompositingKernel.h
    opencl/clkernels/CLRenderingKernel.h
    opencl/clkernels/CLCompositingKernel.h
    opencl/clkernels/CLXRayRenderingKernel.h
    opencl/clkernels/CLMaxIntensityProjectionRenderingKernel.h
    opencl/clkernels/CLMinIntensityProjectionRenderingKernel.h
    opencl/clkernels/CLAverageIntensityProjectionRenderingKernel.h
    opencl/clkernels/CLMinIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLMaxIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.h
    opencl/cldata/CLBuffer.h
    opencl/cldata/CLFrameVariants.hh
    opencl/cldata/CLVolumeVariants.hh
    profiling/Profiles.hh
    profiling/ProfilingExterns.h
    system/Logger.h
    system/LogLevel.hh
    system/oclUtils.h
    system/Utilities.h
    system/Colors.hh )


### Adding the kernels to the CMake source tree
set( OCL_MGPUVS_KERNELS
    kernels/volumeRender.cl
    kernels/xray.cl
    kernels/xray_compositing.cl
    kernels/maxIntensityProjection.cl
    kernels/maxIntensityProjection_compositing.cl
    kernels/minIntensityProjection.cl
    kernels/minIntensityProjection_compositing.cl
    kernels/averageIntensityProjection.cl
    kernels/averageIntensityProjection_compositing.cl

)


### Include the directories to the CMake source tree

include_directories( framework )
include_directories( framework/virtual )
include_directories( framework/compositors )
include_directories( framework/renderers )
include_directories( framework/calibrators )
include_directories( auxillary )
include_directories( common )
include_directories( data )
include_directories( geometry )
include_directories( kernels )
include_directories( opencl )
include_directories( opencl/cldata )
include_directories( opencl/clkernels )
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

add_library( ${OCL_MGPUVS_LIBRARY} SHARED
    ${OCL_MGPUVS_SOURCE_FILES}
    ${OCL_MGPUVS_HEADER_FILES}
    ${OCL_MGPUVS_KERNELS} )


set_property( TARGET ${OCL_MGPUVS_LIBRARY} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/lib${OCL_MGPUVS_LIBRARY}.so )

install( TARGETS ${OCL_MGPUVS_LIBRARY} DESTINATION lib )
install( FILES ${OCL_MGPUVS_HEADER_FILES} DESTINATION include/${OCL_MGPUVS_LIBRARY} )


### ---------------------- BENCHMARK LIBRARY SETTING ---------------------------###



# Create same library, but with activation of BENCHMARKING Definition.
set( OCL_MGPUVS_LIBRARY_BENCHMARK "oclMGPUVS_benchmark" )


# same library with BENCHMARKING defined.
add_library( ${OCL_MGPUVS_LIBRARY_BENCHMARK} SHARED
    ${OCL_MGPUVS_SOURCE_FILES}
    ${OCL_MGPUVS_HEADER_FILES}
    ${OCL_MGPUVS_KERNELS} )


target_compile_definitions( ${OCL_MGPUVS_LIBRARY_BENCHMARK} INTERFACE BENCHMARKING)

set_target_properties(  ${OCL_MGPUVS_LIBRARY_BENCHMARK}
                        PROPERTIES COMPILE_DEFINITIONS "BENCHMARKING" )
# BENCHMARKING defined
set_property( TARGET ${OCL_MGPUVS_LIBRARY_BENCHMARK} PROPERTY IMPORTED_LOCATION
    ${CMAKE_BINARY_DIRECTORY}/lib${OCL_MGPUVS_LIBRARY}.so )


install( TARGETS ${OCL_MGPUVS_LIBRARY_BENCHMARK} DESTINATION lib )


### ------------------------------------------------------------------------------###


install( FILES ${OCL_MGPUVS_KERNELS} DESTINATION share )

# Add the applications
add_subdirectory( apps )


