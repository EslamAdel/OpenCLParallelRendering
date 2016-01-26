QT += core
QT += gui
QT += opengl

TARGET = parallel-rendering-multi-gpu
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

INCLUDEPATH += system/
INCLUDEPATH += data/
INCLUDEPATH += common/
INCLUDEPATH += auxillary/
INCLUDEPATH += geometry/
INCLUDEPATH += opencl/
INCLUDEPATH += kernel/
INCLUDEPATH += rendering/
INCLUDEPATH += /usr/local/include/oclHWDL/
INCLUDEPATH += /usr/local/cuda-7.5/include/
INCLUDEPATH += framework/
INCLUDEPATH += gui/

LIBS += -L/usr/local/lib -loclHWDL
LIBS += -L/usr/lib/x86_64-linux-gnu/ -lOpenCL

SOURCES += main.cpp \
    system/Logger.cpp \
    data/Image.cpp \
    data/TransferFunction.cpp \
    data/Volume.cpp \
    auxillary/glm/detail/glm.cpp \
    geometry/Vector2.cpp \
    geometry/Vector3.cpp \
    geometry/Vector4.cpp \
    opencl/CLRenderingKernel.cpp \
    opencl/CLTransferFunction.cpp \
    opencl/CLVolume.cpp \
    opencl/CLXRayRenderingKernel.cpp \
    rendering/CLContext.cpp \
    system/Utilities.cpp \
    system/oclUtils.cpp \
    framework/ParallelRendering.cpp \
    framework/RenderingNode.cpp \
    framework/TaskRender.cpp \
    framework/TaskCollect.cpp \
    framework/TaskComposite.cpp \
    auxillary/VolumeUtilities.cpp \
    data/Transformation.cpp \
    gui/RenderingWindow.cpp

HEADERS += \
    system/Colors.hh \
    system/Logger.h \
    system/LogLevel.hh \
    data/Image.h \
    data/TransferFunction.h \
    data/Volume.h \
    common/Headers.hh \
    common/Typedefs.hh \
    auxillary/glm/detail/_features.hpp \
    auxillary/glm/detail/_fixes.hpp \
    auxillary/glm/detail/_noise.hpp \
    auxillary/glm/detail/_swizzle.hpp \
    auxillary/glm/detail/_swizzle_func.hpp \
    auxillary/glm/detail/_vectorize.hpp \
    auxillary/glm/detail/func_common.hpp \
    auxillary/glm/detail/func_exponential.hpp \
    auxillary/glm/detail/func_geometric.hpp \
    auxillary/glm/detail/func_integer.hpp \
    auxillary/glm/detail/func_matrix.hpp \
    auxillary/glm/detail/func_packing.hpp \
    auxillary/glm/detail/func_trigonometric.hpp \
    auxillary/glm/detail/func_vector_relational.hpp \
    auxillary/glm/detail/intrinsic_common.hpp \
    auxillary/glm/detail/intrinsic_exponential.hpp \
    auxillary/glm/detail/intrinsic_geometric.hpp \
    auxillary/glm/detail/intrinsic_integer.hpp \
    auxillary/glm/detail/intrinsic_matrix.hpp \
    auxillary/glm/detail/intrinsic_trigonometric.hpp \
    auxillary/glm/detail/intrinsic_vector_relational.hpp \
    auxillary/glm/detail/precision.hpp \
    auxillary/glm/detail/setup.hpp \
    auxillary/glm/detail/type_float.hpp \
    auxillary/glm/detail/type_gentype.hpp \
    auxillary/glm/detail/type_half.hpp \
    auxillary/glm/detail/type_int.hpp \
    auxillary/glm/detail/type_mat.hpp \
    auxillary/glm/detail/type_mat2x2.hpp \
    auxillary/glm/detail/type_mat2x3.hpp \
    auxillary/glm/detail/type_mat2x4.hpp \
    auxillary/glm/detail/type_mat3x2.hpp \
    auxillary/glm/detail/type_mat3x3.hpp \
    auxillary/glm/detail/type_mat3x4.hpp \
    auxillary/glm/detail/type_mat4x2.hpp \
    auxillary/glm/detail/type_mat4x3.hpp \
    auxillary/glm/detail/type_mat4x4.hpp \
    auxillary/glm/detail/type_vec.hpp \
    auxillary/glm/detail/type_vec1.hpp \
    auxillary/glm/detail/type_vec2.hpp \
    auxillary/glm/detail/type_vec3.hpp \
    auxillary/glm/detail/type_vec4.hpp \
    auxillary/glm/gtc/bitfield.hpp \
    auxillary/glm/gtc/color_space.hpp \
    auxillary/glm/gtc/constants.hpp \
    auxillary/glm/gtc/epsilon.hpp \
    auxillary/glm/gtc/integer.hpp \
    auxillary/glm/gtc/matrix_access.hpp \
    auxillary/glm/gtc/matrix_integer.hpp \
    auxillary/glm/gtc/matrix_inverse.hpp \
    auxillary/glm/gtc/matrix_transform.hpp \
    auxillary/glm/gtc/noise.hpp \
    auxillary/glm/gtc/packing.hpp \
    auxillary/glm/gtc/quaternion.hpp \
    auxillary/glm/gtc/random.hpp \
    auxillary/glm/gtc/reciprocal.hpp \
    auxillary/glm/gtc/round.hpp \
    auxillary/glm/gtc/type_precision.hpp \
    auxillary/glm/gtc/type_ptr.hpp \
    auxillary/glm/gtc/ulp.hpp \
    auxillary/glm/gtc/vec1.hpp \
    auxillary/glm/gtx/associated_min_max.hpp \
    auxillary/glm/gtx/bit.hpp \
    auxillary/glm/gtx/closest_point.hpp \
    auxillary/glm/gtx/color_space.hpp \
    auxillary/glm/gtx/color_space_YCoCg.hpp \
    auxillary/glm/gtx/common.hpp \
    auxillary/glm/gtx/compatibility.hpp \
    auxillary/glm/gtx/component_wise.hpp \
    auxillary/glm/gtx/dual_quaternion.hpp \
    auxillary/glm/gtx/euler_angles.hpp \
    auxillary/glm/gtx/extend.hpp \
    auxillary/glm/gtx/extended_min_max.hpp \
    auxillary/glm/gtx/fast_exponential.hpp \
    auxillary/glm/gtx/fast_square_root.hpp \
    auxillary/glm/gtx/fast_trigonometry.hpp \
    auxillary/glm/gtx/gradient_paint.hpp \
    auxillary/glm/gtx/handed_coordinate_space.hpp \
    auxillary/glm/gtx/hash.hpp \
    auxillary/glm/gtx/integer.hpp \
    auxillary/glm/gtx/intersect.hpp \
    auxillary/glm/gtx/io.hpp \
    auxillary/glm/gtx/log_base.hpp \
    auxillary/glm/gtx/matrix_cross_product.hpp \
    auxillary/glm/gtx/matrix_decompose.hpp \
    auxillary/glm/gtx/matrix_interpolation.hpp \
    auxillary/glm/gtx/matrix_major_storage.hpp \
    auxillary/glm/gtx/matrix_operation.hpp \
    auxillary/glm/gtx/matrix_query.hpp \
    auxillary/glm/gtx/matrix_transform_2d.hpp \
    auxillary/glm/gtx/mixed_product.hpp \
    auxillary/glm/gtx/norm.hpp \
    auxillary/glm/gtx/normal.hpp \
    auxillary/glm/gtx/normalize_dot.hpp \
    auxillary/glm/gtx/number_precision.hpp \
    auxillary/glm/gtx/optimum_pow.hpp \
    auxillary/glm/gtx/orthonormalize.hpp \
    auxillary/glm/gtx/perpendicular.hpp \
    auxillary/glm/gtx/polar_coordinates.hpp \
    auxillary/glm/gtx/projection.hpp \
    auxillary/glm/gtx/quaternion.hpp \
    auxillary/glm/gtx/range.hpp \
    auxillary/glm/gtx/raw_data.hpp \
    auxillary/glm/gtx/rotate_normalized_axis.hpp \
    auxillary/glm/gtx/rotate_vector.hpp \
    auxillary/glm/gtx/scalar_multiplication.hpp \
    auxillary/glm/gtx/scalar_relational.hpp \
    auxillary/glm/gtx/simd_mat4.hpp \
    auxillary/glm/gtx/simd_quat.hpp \
    auxillary/glm/gtx/simd_vec4.hpp \
    auxillary/glm/gtx/spline.hpp \
    auxillary/glm/gtx/std_based_type.hpp \
    auxillary/glm/gtx/string_cast.hpp \
    auxillary/glm/gtx/transform.hpp \
    auxillary/glm/gtx/transform2.hpp \
    auxillary/glm/gtx/type_aligned.hpp \
    auxillary/glm/gtx/vector_angle.hpp \
    auxillary/glm/gtx/vector_query.hpp \
    auxillary/glm/gtx/wrap.hpp \
    auxillary/glm/common.hpp \
    auxillary/glm/exponential.hpp \
    auxillary/glm/ext.hpp \
    auxillary/glm/fwd.hpp \
    auxillary/glm/geometric.hpp \
    auxillary/glm/glm.hpp \
    auxillary/glm/integer.hpp \
    auxillary/glm/mat2x2.hpp \
    auxillary/glm/mat2x3.hpp \
    auxillary/glm/mat2x4.hpp \
    auxillary/glm/mat3x2.hpp \
    auxillary/glm/mat3x3.hpp \
    auxillary/glm/mat3x4.hpp \
    auxillary/glm/mat4x2.hpp \
    auxillary/glm/mat4x3.hpp \
    auxillary/glm/mat4x4.hpp \
    auxillary/glm/matrix.hpp \
    auxillary/glm/packing.hpp \
    auxillary/glm/trigonometric.hpp \
    auxillary/glm/vec2.hpp \
    auxillary/glm/vec3.hpp \
    auxillary/glm/vec4.hpp \
    auxillary/glm/vector_relational.hpp \
    auxillary/half.hpp \
    geometry/Vector2.h \
    geometry/Vector3.h \
    geometry/Vector4.h \
    opencl/CLRenderingKernel.h \
    opencl/CLTransferFunction.h \
    opencl/CLVolume.h \
    opencl/CLXRayRenderingKernel.h \
    rendering/CLContext.h \
    system/Utilities.h \
    system/oclUtils.h \
    framework/ParallelRendering.h \
    framework/RenderingNode.h \
    data/VolumeVariant.hh \
    data/ImageVariant.hh \
    framework/TaskRender.h \
    framework/TaskCollect.h \
    framework/TaskComposite.h \
    auxillary/VolumeUtilities.h \
    data/Transformation.h \
    gui/RenderingWindow.h

FORMS += \
    gui/RenderingWindow.ui

DISTFILES += \
    kernels/volumeRender.cl \
    kernels/xray.cl


