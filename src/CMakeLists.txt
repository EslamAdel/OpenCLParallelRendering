################################################################################
# Copyright (C) 2015 Marwan Abdellah <abdellah.marwan@gmail.com>
################################################################################

### Adding the source files to the CMake source tree
SET( CLPAREN_SOURCE_FILES
    framework/renderers/CLAbstractRenderer.cpp
    framework/compositors/CLAbstractCompositor.cpp
    framework/compositors/CLCompositorAccumulate.cpp
    framework/calibrators/AbstractCalibrator.cpp
    framework/calibrators/Calibrator.cpp
    framework/calibrators/TaskCalibrate.cpp
    framework/parallel/ParallelRendering.cpp
    framework/renderers/CLRenderer.cpp
    framework/renderers/TaskRender.cpp
    framework/parallel/TaskCollect.cpp
    framework/compositors/TaskComposite.cpp
    framework/compositors/CLCompositor.cpp
    framework/parallel/TaskMakePixmap.cpp
    framework/virtual/VirtualParallelRendering.cpp
    framework/virtual/VirtualCLRenderer.cpp
    framework/virtual/VirtualCLCompositor.cpp
    framework/virtual/VirtualTaskCollect.cpp
    data/Transformation.cpp
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
    opencl/clkernels/CLIsoSurfaceRenderingKernel.cpp
    opencl/clkernels/CLMinIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLMaxIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.cpp
    opencl/clkernels/CLIsoSurfaceCompositingKernel.cpp
    opencl/cldata/CLBuffer.cpp
    profiling/ProfilingExterns.cpp
    system/oclUtils.cpp
    system/Utilities.cpp
    system/Logger.cpp
)

### Adding the header files to the CMake source tree
SET( CLPAREN_HEADER_FILES
    framework/clparen.h
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
    framework/parallel/ParallelRendering.h
    framework/parallel/TaskCollect.h
    framework/compositors/TaskComposite.h
    framework/parallel/TaskMakePixmap.h
    framework/renderers/TaskRender.h
    framework/virtual/VirtualCLCompositor.h
    framework/virtual/VirtualCLCompositor.ipp
    framework/virtual/VirtualParallelRendering.h
    framework/virtual/VirtualCLRenderer.h
    framework/virtual/VirtualCLRenderer.ipp
    framework/virtual/VirtualTaskCollect.h
    data/Transformation.h
    auxillary/half.hpp
    auxillary/VolumeUtilities.h
    common/Headers.hh
    common/Typedefs.hh
    data/Image.h
    data/ImageVariant.hh
    data/TransferFunction.h
    data/Volume.h
    data/Volume.ipp
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
    opencl/clkernels/CLIsoSurfaceRenderingKernel.h
    opencl/clkernels/CLMinIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLMaxIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLAverageIntensityProjectionCompositingKernel.h
    opencl/clkernels/CLIsoSurfaceCompositingKernel.h
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
set( CLPAREN_KERNELS
    kernels/volumeRender.cl
    kernels/xray.cl
    kernels/xray_compositing.cl
    kernels/maxIntensityProjection.cl
    kernels/maxIntensityProjection_compositing.cl
    kernels/minIntensityProjection.cl
    kernels/minIntensityProjection_compositing.cl
    kernels/averageIntensityProjection.cl
    kernels/averageIntensityProjection_compositing.cl
    kernels/isoSurface.cl
    kernels/isoSurface_compositing.cl
)


### Include the directories to the CMake source tree

include_directories( framework )
include_directories( framework/parallel )
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
foreach( kernel_file ${CLPAREN_KERNELS} )
    list( APPEND CLPAREN_KERNEL_FILES
         "${CMAKE_CURRENT_SOURCE_DIR}/${kernel_file}" )
    message(${CMAKE_CURRENT_SOURCE_DIR}/${kernel_file})
endforeach( )

install( FILES ${CLPAREN_KERNELS} DESTINATION share )

### ---------------------- RELEASE LIBRARY SETTING ---------------------------###

# Create a library with the main compoments of the framework and link the apps
# against it later to reduce the compilation time
set( CLPAREN_LIBRARY "clparen" )

add_library( ${CLPAREN_LIBRARY} SHARED
    ${CLPAREN_SOURCE_FILES}
    ${CLPAREN_HEADER_FILES}
    ${CLPAREN_KERNELS}  )


install( TARGETS ${CLPAREN_LIBRARY} DESTINATION lib )
install( FILES ${CLPAREN_HEADER_FILES} DESTINATION include/${CLPAREN_LIBRARY} )

### ---------------------- BENCHMARK LIBRARY SETTING ---------------------------###

# Create same library, but with activation of BENCHMARKING Definition.
set( CLPAREN_LIBRARY_BENCHMARK "clparen_benchmark" )


# same library with BENCHMARKING defined.
add_library( ${CLPAREN_LIBRARY_BENCHMARK} SHARED
    ${CLPAREN_SOURCE_FILES}
    ${CLPAREN_HEADER_FILES}
    ${CLPAREN_KERNELS} )


target_compile_definitions( ${CLPAREN_LIBRARY_BENCHMARK} INTERFACE BENCHMARKING)

set_target_properties(  ${CLPAREN_LIBRARY_BENCHMARK}
                        PROPERTIES COMPILE_DEFINITIONS "BENCHMARKING" )

install( TARGETS ${CLPAREN_LIBRARY_BENCHMARK} DESTINATION lib )

### ------------------------------------------------------------------------------###

# Add the applications
add_subdirectory( apps )
