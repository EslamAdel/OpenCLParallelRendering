#ifndef TYPEDEFS_HH
#define TYPEDEFS_HH

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
/// Vector2 (Image)
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Coordinates2D
 * The XY coordinates of an image.
 */
typedef Vector2F Coordinates2D;

/**
 * @brief Dimensions2D
 * The dimensions of an image.
 */
typedef Vector2UI Dimensions2D;

/**
 * @brief Pixel2DIndex
 * An XYZ index of the voxel in a 3D volume.
 * The X and Y components specify the location of the pixel in the image.
 */
typedef Vector2UI Pixel2DIndex;

////////////////////////////////////////////////////////////////////////////////
/// Vector3 (Volume)
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Coordinates3D
 * The XYZ coordinates of a volume.
 */
typedef Vector3F Coordinates3D;

/**
 * @brief Dimensions3D
 * The dimensions of a volume.
 */
typedef Vector3UI Dimensions3D;

/**
 * @brief Voxel3DIndex
 * An XYZ index of the voxel in a 3D volume.
 * The X, Y and Z components specify the location of the voxel in the volume.
 */
typedef Vector3UI Voxel3DIndex;

////////////////////////////////////////////////////////////////////////////////
/// Vector4 (Colors)
////////////////////////////////////////////////////////////////////////////////


/**
 * @brief Color
 * A color value with xyzw components.
 */
typedef Vector4F Color;

/**
 * @brief Colors
 * A list of colors reflecting a set of points of a transfer function.
 */
typedef std::vector< Color > Colors;




#endif // TYPEDEFS_HH
