#####################################################################
# Copyright © 2011-2016,
# Marwan Abdellah: <abdellah.marwan@gmail.com>
#####################################################################

find_path(OPENCL_INCLUDE_DIR
    NAMES
        CL/cl.h OpenCL/cl.h
    PATHS
        $ENV{AMDAPPSDKROOT}/include
        $ENV{INTELOCLSDKROOT}/include
	/usr/local/cuda-5.0/include/
	/usr/local/cuda-5.5/include/
	/usr/local/cuda-6.0/include/
        /usr/local/cuda-6.5/include/
	/usr/local/cuda-7.0/include/
	/usr/local/cuda-7.5/include/
        # Legacy Stream SDK
        $ENV{ATISTREAMSDKROOT}/include
)
message(${OPENCL_INCLUDE_DIR})

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	SET(OPENCL_LIB_SEARCH_PATH
		${OPENCL_LIB_SEARCH_PATH}
		$ENV{AMDAPPSDKROOT}/lib/x86
		$ENV{INTELOCLSDKROOT}/lib/x86
		/usr/local/cuda/lib64
		# Legacy Stream SDK
		$ENV{ATISTREAMSDKROOT}/lib/x86)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET(OPENCL_LIB_SEARCH_PATH
		${OPENCL_LIB_SEARCH_PATH}
		$ENV{AMDAPPSDKROOT}/lib/x86_64
		$ENV{INTELOCLSDKROOT}/lib/x64
		$ENV{NVSDKCOMPUTE_ROOT}/OpenCL/common/lib/x64
		/usr/local/cuda/lib64
		# Legacy stream SDK
		$ENV{ATISTREAMSDKROOT}/lib/x86_64)
endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

find_library(OPENCL_LIBRARY
    NAMES
        OpenCL
    PATHS ${OPENCL_LIB_SEARCH_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenCL
    DEFAULT_MSG
    OPENCL_LIBRARY OPENCL_INCLUDE_DIR)

if(OPENCL_FOUND)
  set(OPENCL_LIBRARIES ${OPENCL_LIBRARY})
endif(OPENCL_FOUND)

mark_as_advanced (
  OPENCL_INCLUDE_DIR
  OPENCL_LIBRARY
)

INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )
LINK_LIBRARIES(${OPENCL_LIBRARIES})
