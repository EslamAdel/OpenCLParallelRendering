#include "VolumeUtilities.h"

uint64_t VolumeUtilities::get1DIndex( const uint64_t x,
                                      const uint64_t y,
                                      const uint64_t z,
                                      const uint64_t width,
                                      const uint64_t height )
{
    // Flat[x + WIDTH * (y + HEIGHT * z)] = Original[x, y, z]
    return ( x + width * ( y + height * z ));
}

uint64_t VolumeUtilities::get1DIndex( const uint64_t x,
                                      const uint64_t y,
                                      const uint64_t z,
                                      const Dimensions3D dimensions )
{
    // Flat[x + WIDTH * (y + HEIGHT * z)] = Original[x, y, z]
    return ( x + dimensions.x * ( y + dimensions.y * z ));
}

uint64_t VolumeUtilities::get1DIndex( const Voxel3DIndex index,
                                      const Dimensions3D dimensions )
{
    // Flat[x + WIDTH * (y + HEIGHT * z)] = Original[x, y, z]
    return ( index.x + dimensions.x * ( index.y + dimensions.y * index.z ));
}
