#ifndef VOLUMEUTILITIES_H
#define VOLUMEUTILITIES_H

#include <Typedefs.hh>

namespace VolumeUtilities
{

/**
 * @brief get1DIndex
 * Convertes a 3D index of a voxel into 1D index.
 * @param x x-coordinate
 * @param y y-coordinate
 * @param z z-coordinate
 * @param width Volume width
 * @param height Volume height
 * @return a 1D index of the requested voxel
 */
uint64_t get1DIndex( const uint64_t x, const uint64_t y, const uint64_t z,
                     const uint64_t width, const uint64_t height );

/**
 * @brief get1DIndex
 * Convertes a 3D index of a voxel into 1D index.
 * @param x x-coordinate
 * @param y y-coordinate
 * @param z z-coordinate
 * @param dimensions Volume dimensions
 * @return a 1D index of the requested voxel
 */
uint64_t get1DIndex( const uint64_t x, const uint64_t y, const uint64_t z,
                     const Dimensions3D dimensions );

/**
 * @brief get1DIndex
 * Convertes a 3D index of a voxel into 1D index.
 * @param index 3D index
 * @param dimensions Volume dimensions
 * @return a 1D index of the requested voxel
 */
uint64_t get1DIndex( const Voxel3DIndex index, const Dimensions3D dimensions );
}

#endif // VOLUMEUTILITIES_H
