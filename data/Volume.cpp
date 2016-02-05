#include "Volume.h"
#include "VolumeUtilities.h"
#include "Logger.h"

template< class T >
Volume< T >::Volume( const std::string prefix, const bool drawBoundingBox )
    : drawBoundingBox_( drawBoundingBox )
{
    // Load the volume from a file
    loadVolumeData_( prefix );

    coordinates_ = Coordinates3D( dimensions_.x / 2.f,
                                  dimensions_.y / 2.f,
                                  dimensions_.z / 2.f);

    unitCubeCenter_ = Coordinates3D( coordinates_.x / dimensions_.x,
                                     coordinates_.y / dimensions_.y,
                                     coordinates_.z / dimensions_.z );

    unitCubeScaleFactors_ = Coordinates3D( 1.f , 1.f , 1.f );

    LOG_DEBUG("Volume Created: %dx%dx%d",
              dimensions_.x , dimensions_.y , dimensions_.z );


}

template< class T >
Volume< T >::Volume( const Coordinates3D brickCoordinates,
                     const Dimensions3D brickDimensions,
                     const Coordinates3D brickUnitCubeCenter,
                     const Coordinates3D brickUnitCubeScaleFactors,
                     T *brickData, const bool drawBoundingBox )
    : drawBoundingBox_( drawBoundingBox )
{
    coordinates_    = brickCoordinates ;
    dimensions_     = brickDimensions ;
    unitCubeCenter_ = brickUnitCubeCenter ;
    unitCubeScaleFactors_ = brickUnitCubeScaleFactors ;
    data_ = brickData ;

}

template< class T >
uint64_t Volume< T >::getSizeX() const
{
    return dimensions_.x;
}

template< class T >
uint64_t Volume< T >::getSizeY() const
{
    return dimensions_.y;
}

template< class T >
uint64_t Volume< T >::getSizeZ() const
{
    return dimensions_.z;
}

template< class T >
uint64_t Volume< T >::getSizeInBytes() const
{
    return sizeInBytes_;
}

template< class T >
void Volume< T >::loadHeaderData_( const std::string prefix )
{
    const std::string filePath = prefix + std::string( ".hdr" );

    // Open the header file
    std::ifstream hdrFileStream( filePath.c_str());

    // Volume dimensions
    hdrFileStream >> dimensions_.x;
    hdrFileStream >> dimensions_.y;
    hdrFileStream >> dimensions_.z;

    sizeInBytes_ = dimensions_.volumeSize() * sizeof( T );

    // Close the stream
    hdrFileStream.close();
}

template< class T >
void Volume< T >::loadVolumeData_( const std::string prefix )
{
    loadHeaderData_( prefix );

    // Allocate the volume
    data_ = new T[ dimensions_.volumeSize() ];

    std::string filePath = prefix + std::string( ".img" );

    // Read the volume file from the input stream
    std::ifstream imgFileStream;
    imgFileStream.open( filePath.c_str(), std::ios::in | std::ios::binary );
    if ( imgFileStream.fail() )
    {
        printf( "Could not open the volume file [%s]", filePath.c_str() );
    }

    imgFileStream.read(( char* ) data_, dimensions_.volumeSize());

    // Close the stream
    imgFileStream.close();
}

template< class T >
Dimensions3D Volume< T >::getDimensions() const
{
    return dimensions_;
}


template< class T >
Coordinates3D Volume<T>::getCubeCenter() const
{
    return coordinates_;
}

template< class T >
Coordinates3D Volume<T>::getUnitCubeCenter() const
{
    return unitCubeCenter_;
}

template< class T>
Coordinates3D Volume<T>::getUnitCubeScaleFactors() const
{
    return unitCubeScaleFactors_;
}

template< class T >
uint64_t Volume< T >::get1DIndex( const uint64_t x,
                                  const uint64_t y,
                                  const uint64_t z ) const
{
    // Flat[x + WIDTH * (y + HEIGHT * z)] = Original[x, y, z]
    return ( x + dimensions_.x * ( y + dimensions_.y * z ));
}

template< class T >
uint64_t Volume< T >::get1DIndex( const Voxel3DIndex index ) const
{
    // Flat[x + WIDTH * (y + HEIGHT * z)] = Original[x, y, z]
    return ( index.x + dimensions_.x * ( index.y + dimensions_.y * index.z ));
}


template <class T>
Volume< T >* Volume<T>::getBrick( const u_int64_t xi, const u_int64_t xf,
                                  const u_int64_t yi, const u_int64_t yf,
                                  const u_int64_t zi, const u_int64_t zf )
{
    // The dimensions of the extracted brick
    //Add 1 to solve the one pixel shift problem
    Dimensions3D brickDimensions( xf - xi + 1, yf - yi + 1 , zf - zi + 1 );

    //Subtract 1 in case of edge Brick
    if(xf == dimensions_.x)
        brickDimensions.x -= 1;
    if(yf == dimensions_.y)
        brickDimensions.y -= 1;
    if(zf == dimensions_.z)
        brickDimensions.z -= 1;

    // The center of the extraced brick, with respect to the real base volume.
    Coordinates3D brickCoordinates( xi + brickDimensions.x / 2.f,
                                    yi + brickDimensions.y / 2.f,
                                    zi + brickDimensions.z / 2.f);

    // The relative center of the brick for the OpenGL unti texture,
    // with respect to the "unity" base volume.
    Coordinates3D brickUnitCubeCenter( brickCoordinates.x / dimensions_.x,
                                       brickCoordinates.y / dimensions_.y,
                                       brickCoordinates.z / dimensions_.z );

    // Scale the brick in a unit cube for the OpenGL texture
    Coordinates3D brickUnitCubeScaleFactors
            ( float( brickDimensions.x ) / dimensions_.x,
              float( brickDimensions.y ) / dimensions_.y,
              float( brickDimensions.z ) / dimensions_.z );

    //    std::cout << "center " << brickUnitCubeCenter.x
    //              << " " << brickUnitCubeCenter.y << " "
    //              <<  brickUnitCubeCenter.z << std::endl;

    //    std::cout << brickUnitCubeScaleFactors.x << " "
    //              << brickUnitCubeScaleFactors.y << " "
    //              <<  brickUnitCubeScaleFactors.z << std::endl;

    // The array that will be filled with the brick data
    T* brickData = new T[brickDimensions.volumeSize()];

    for( uint64_t i = 0; i < brickDimensions.x; i++ )
    {
        for( uint64_t j = 0; j < brickDimensions.y; j++ )
        {
            for( uint64_t k = 0; k < brickDimensions.z; k++ )
            {
                // The 1D index of the extracted brick
                const uint64_t brickIndex =
                        VolumeUtilities::get1DIndex( i, j, k, brickDimensions );

                // The 1D index of the original 'big' volume
                const uint64_t volumeIndex =
                        get1DIndex( xi + i, yi + j, zi + k );

                brickData[brickIndex] = data_[volumeIndex];
            }
        }
    }

    return new Volume< T >( brickCoordinates,
                            brickDimensions,
                            brickUnitCubeCenter,
                            brickUnitCubeScaleFactors,
                            brickData );
}

template< class T >
T* Volume< T >::getValue( const uint64_t x,
                          const uint64_t y,
                          const uint64_t z ) const
{
    const uint64_t index = get1DIndex( x, y, z );
    T* value = new T[ 1 ];
    *value = data_[ index ];
    return value;
}

template< class T >
T *Volume<T>::getValue( const Voxel3DIndex xyz ) const
{
    const u_int64_t index = get1DIndex( xyz.x, xyz.y, xyz.z );
    T* value = new T[ 1 ];
    *value = data_[ index ];
    return value;
}

template< class T >
T* Volume< T >::getData() const
{
    return data_;
}

template< class T >
void Volume< T >::addBoundingBox_()
{
    T* ptr = data_;
    for ( u_int64_t i = 0; i < dimensions_.z; i++ )
    {
        for ( u_int64_t j = 0; j < dimensions_.y; j++ )
        {
            for ( u_int64_t k = 0; k < dimensions_.x; k++ )
            {
                if ((( i < 4 )                 && ( j < 4 ))                  ||
                        (( j < 4 )                 && ( k < 4 ))                  ||
                        (( k < 4 )                 && ( i < 4 ))                  ||
                        (( i < 4 )                 && ( j > dimensions_.y - 5 ))  ||
                        (( j < 4 )                 && ( k > dimensions_.x - 5 ))  ||
                        (( k < 4 )                 && ( i > dimensions_.z - 5 ))  ||
                        (( i > dimensions_.z - 5 ) && ( j > dimensions_.y - 5 ))  ||
                        (( j > dimensions_.y - 5 ) && ( k > dimensions_.x - 5 ))  ||
                        (( k > dimensions_.x - 5 ) && ( i > dimensions_.z - 5 ))  ||
                        (( i > dimensions_.z - 5 ) && ( j < 4 ))                  ||
                        (( j > dimensions_.y - 5 ) && ( k < 4 ))                  ||
                        (( k > dimensions_.x - 5 ) && ( i < 4 )))
                {
                    *ptr = 255;
                }
                ptr++;
            }
        }
    }
}

template< class T >
void Volume< T >::zeroPad_()
{

}

template< class T >
Image<T>* Volume< T >::getSliceX( const u_int64_t x ) const
{

    Dimensions2D sliceDimensions( dimensions_.y, dimensions_.z );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    u_int64_t sliceIndex = 0;
    for( u_int64_t i = 0; i < dimensions_.y; i++ )
    {
        for( u_int64_t j = 0; j < dimensions_.z; j++ )
        {
            sliceData[sliceIndex] = data_[get1DIndex( x, i, j )];
            sliceIndex++;
        }
    }

    Image< T >* slice = new Image< T >( sliceDimensions, sliceData );
    return slice;
}

template< class T >
Image<T>* Volume< T >::getSliceY( const u_int64_t y ) const
{
    Dimensions2D sliceDimensions( dimensions_.x, dimensions_.z );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    u_int64_t sliceIndex = 0;
    for( u_int64_t i = 0; i < dimensions_.x; i++ )
    {
        for( u_int64_t j = 0; j < dimensions_.z; j++ )
        {
            sliceData[sliceIndex] = data_[get1DIndex( i, y, j )];
            sliceIndex++;
        }
    }

    Image< T >* slice = new Image< T >( sliceDimensions, sliceData );
    return slice;
}

template< class T >
Image<T>* Volume< T >::getSliceZ( const u_int64_t z ) const
{
    Dimensions2D sliceDimensions( dimensions_.x, dimensions_.y );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    u_int64_t sliceIndex = 0;
    for( u_int64_t i = 0; i < dimensions_.x; i++ )
    {
        for( u_int64_t j = 0; j < dimensions_.y; j++ )
        {
            sliceData[sliceIndex] = data_[get1DIndex( i, j, z )];
            sliceIndex++;
        }
    }

    Image< T >* slice = new Image< T >( sliceDimensions, sliceData );
    return slice;
}

template< class T >
Image< T >* Volume< T >::getProjectionX( ) const
{
    Dimensions2D sliceDimensions( dimensions_.y, dimensions_.z );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    // Save the result in a float array to avoid the overflow.
    float* sliceDataFloat = new float[ sliceDimensions.imageSize() ];
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
        sliceDataFloat[i] = 0.f;

    for( u_int64_t sliceIdx = 0; sliceIdx < dimensions_.x; sliceIdx++ )
    {
        u_int64_t pixelIndex = 0;
        for( u_int64_t i = 0; i < dimensions_.y; i++ )
        {
            for( u_int64_t j = 0; j < dimensions_.z; j++ )
            {
                sliceDataFloat[pixelIndex] +=
                        data_[get1DIndex( sliceIdx, i, j )];
                pixelIndex++;
            }
        }
    }

    float maxValue = 0;
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        if( sliceDataFloat[i] > maxValue ) maxValue = sliceDataFloat[i];
    }

    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        sliceData[i] = sliceDataFloat[i] / maxValue * 255;
    }

    Image< T >* projection = new Image< T >( sliceDimensions, sliceData );
    return projection;
}

template< class T >
Image<T> * Volume< T >::getProjectionY() const
{
    Dimensions2D sliceDimensions( dimensions_.x, dimensions_.z );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    // Save the result in a float array to avoid the overflow.
    float* sliceDataFloat = new float[ sliceDimensions.imageSize() ];
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
        sliceDataFloat[i] = 0.f;

    for( u_int64_t sliceIdx = 0; sliceIdx < dimensions_.x; sliceIdx++ )
    {
        u_int64_t pixelIndex = 0;
        for( u_int64_t i = 0; i < dimensions_.x; i++ )
        {
            for( u_int64_t j = 0; j < dimensions_.z; j++ )
            {
                sliceDataFloat[pixelIndex] +=
                        data_[get1DIndex( i, sliceIdx, j )];
                pixelIndex++;
            }
        }
    }

    float maxValue = 0;
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        if( sliceDataFloat[i] > maxValue ) maxValue = sliceDataFloat[i];
    }

    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        sliceData[i] = sliceDataFloat[i] / maxValue * 255;
    }

    Image< T >* projection = new Image< T >( sliceDimensions, sliceData );
    return projection;
}

template< class T >
Image<T>* Volume<T>::getProjectionZ() const
{
    Dimensions2D sliceDimensions( dimensions_.x, dimensions_.y );
    T* sliceData = new T[ sliceDimensions.imageSize() ];

    // Save the result in a float array to avoid the overflow.
    float* sliceDataFloat = new float[ sliceDimensions.imageSize() ];
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
        sliceDataFloat[i] = 0.f;

    for( u_int64_t sliceIdx = 0; sliceIdx < dimensions_.z; sliceIdx++ )
    {
        u_int64_t pixelIndex = 0;
        for( u_int64_t i = 0; i < dimensions_.x; i++ )
        {
            for( u_int64_t j = 0; j < dimensions_.y; j++ )
            {
                sliceDataFloat[pixelIndex] +=
                        data_[get1DIndex( i, j, sliceIdx )];
                pixelIndex++;
            }
        }
    }

    float maxValue = 0;
    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        if( sliceDataFloat[i] > maxValue ) maxValue = sliceDataFloat[i];
    }

    for(u_int64_t i = 0; i < sliceDimensions.imageSize(); i++)
    {
        sliceData[i] = sliceDataFloat[i] / maxValue * 255;
    }

    Image< T >* projection = new Image< T >( sliceDimensions, sliceData );
    return projection;
}

template< class T >
Volume< T >::~Volume()
{
    delete [] data_;
}


#include <Volume.ipp>
