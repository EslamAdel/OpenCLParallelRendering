#include "Volume.h"
#include "Logger.h"

template< class T >
Volume< T >::Volume( const std::string prefix, const bool drawBoundingBox )
    : drawBoundingBox_( drawBoundingBox ),
      data_( nullptr ),
      mmapAddr_( nullptr )
{
    //Map Volume instead of Loading in Memory
    mapVolumeData( prefix );

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
    : drawBoundingBox_( drawBoundingBox ),
      data_( nullptr ),
      mmapAddr_( nullptr )
{

    coordinates_    = brickCoordinates ;
    dimensions_     = brickDimensions ;
    unitCubeCenter_ = brickUnitCubeCenter ;
    unitCubeScaleFactors_ = brickUnitCubeScaleFactors ;
    data_ = brickData ;

//    LOG_DEBUG("Coor(%f,%f,%f),D(%d,%d,%d),unitCenter(%f,%f,%f),"
//              "unitScale(%f,%f,%f)",
//              coordinates_.x , coordinates_.y , coordinates_.z ,
//              dimensions_.x , dimensions_.y , dimensions_.z ,
//              unitCubeCenter_.x , unitCubeCenter_.y , unitCubeCenter_.z ,
//              unitCubeScaleFactors_.x , unitCubeScaleFactors_.y ,
//              unitCubeScaleFactors_.z );
    //    addBoundingBox_();
}

template< class T >
Volume< T >::Volume( const BrickParameters< T > brickParameters ,
                     const bool drawBoundingBox )
    : drawBoundingBox_( drawBoundingBox ),
      data_( nullptr ),
      mmapAddr_( nullptr )
{

    // The array that will be filled with the brick data
    data_ = new T[ brickParameters.dimensions_.volumeSize() ];
    dimensions_ = brickParameters.dimensions_ ;
    coordinates_ = brickParameters.coordinates_ ;
    unitCubeCenter_ = brickParameters.unitCubeCenter_ ;
    unitCubeScaleFactors_ = brickParameters.unitCubeScaleFactors_ ;

//    LOG_DEBUG("Coor(%f,%f,%f),D(%d,%d,%d),unitCenter(%f,%f,%f),"
//              "unitScale(%f,%f,%f)",
//              coordinates_.x , coordinates_.y , coordinates_.z ,
//              dimensions_.x , dimensions_.y , dimensions_.z ,
//              unitCubeCenter_.x , unitCubeCenter_.y , unitCubeCenter_.z ,
//              unitCubeScaleFactors_.x , unitCubeScaleFactors_.y ,
//              unitCubeScaleFactors_.z );

    for( uint64_t i = 0; i < brickParameters.dimensions_.x; i++ )
    {
        for( uint64_t j = 0; j < brickParameters.dimensions_.y; j++ )
        {
            for( uint64_t k = 0; k < brickParameters.dimensions_.z; k++ )
            {
                // The 1D index of the extracted brick
                const uint64_t brickIndex =
                        VolumeUtilities::get1DIndex(
                            i, j, k, brickParameters.dimensions_ );

                // The 1D index of the original 'big' volume
                const uint64_t volumeIndex =
                        VolumeUtilities::get1DIndex(
                            brickParameters.origin_.x + i,
                            brickParameters.origin_.y + j,
                            brickParameters.origin_.z + k ,
                            brickParameters.baseDimensions_ );

                //Get brick data from the big volume
                data_[ brickIndex ] =
                        brickParameters.baseData_[ volumeIndex ] ;
            }
        }
    }

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
    return dimensions_.volumeSize() * sizeof( T );
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
void Volume< T >::mapVolumeData( const std::string prefix )
{
    int fd;

    //Load the header file .hdr
    loadHeaderData_( prefix );

    //Get the .img file's path
    std::string filePath = prefix + std::string( ".img" );

    //Open the file
    fd = open(filePath.c_str(), O_RDONLY);
    if ( fd == -1 )
    {
        LOG_ERROR( "Could not open the volume file [%s]", filePath.c_str( ));
    }

    //Map the volume to virtual addresses
    mmapAddr_= ( T* ) mmap( NULL , sizeInBytes_ , PROT_READ ,
                            MAP_PRIVATE , fd , 0 );

    //Close the file
    close(fd);
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
                                  const u_int64_t zi, const u_int64_t zf ) const
{
    const BrickParameters< T > brickParameters =
            getBrickParameters( xi , xf , yi , yf , zi , zf );


//    T *brickData = new T[ brickParameters.dimensions_.volumeSize() ];

//    uint64_t sum = 0;
//    for( uint64_t i = 0; i < brickParameters.dimensions_.x; i++ )
//    {
//        for( uint64_t j = 0; j < brickParameters.dimensions_.y; j++ )
//        {
//            for( uint64_t k = 0; k < brickParameters.dimensions_.z; k++ )
//            {
//                // The 1D index of the extracted brick
//                const uint64_t brickIndex =
//                        VolumeUtilities::get1DIndex(
//                            i, j, k, brickParameters.dimensions_ );

//                // The 1D index of the original 'big' volume
//                const uint64_t volumeIndex =
//                        get1DIndex( brickParameters.origin_.x + i,
//                                    brickParameters.origin_.y + j,
//                                    brickParameters.origin_.z + k );

//                //Get brick data from the big volume
//                brickData[ brickIndex ] =
//                        brickParameters.baseData_[ volumeIndex ] ;

//                sum += brickData[ brickIndex ];
//            }
//        }
//    }
//    LOG_DEBUG("sum=%d",sum);


    return new Volume< T >( brickParameters );
    //    return new Volume< T >( brickParameters.brickCoordinates_ ,
    //                            brickParameters.brickDimensions_ ,
    //                            brickParameters.brickUnitCubeCenter_ ,
    //                            brickParameters.brickUnitCubeScaleFactors_ ,
    //                            brickData );
}

template< class T >
BrickParameters<T> Volume< T >::getBrickParameters( const u_int64_t xi ,
                                                    const u_int64_t xf ,
                                                    const u_int64_t yi ,
                                                    const u_int64_t yf ,
                                                    const u_int64_t zi ,
                                                    const u_int64_t zf ) const
{
    // The dimensions of the extracted brick
    const Dimensions3D brickDimensions( xf - xi , yf - yi , zf - zi );


    const Coordinates3D offset( coordinates_.x - dimensions_.x / 2.f ,
                                coordinates_.y - dimensions_.y / 2.f ,
                                coordinates_.z - dimensions_.z / 2.f );

    // The center of the extraced brick, with respect to the real base volume.
    const Coordinates3D brickCoordinates( xi + brickDimensions.x / 2.f +
                                          offset.x ,
                                          yi + brickDimensions.y / 2.f +
                                          offset.y ,
                                          zi + brickDimensions.z / 2.f +
                                          offset.z );

    // The relative center of the brick for the OpenGL unti texture,
    // with respect to the "unity" base volume.
    const Coordinates3D brickUnitCubeCenter(
                brickCoordinates.x  /
                ( dimensions_.x / unitCubeScaleFactors_.x )  ,

                brickCoordinates.y  /
                ( dimensions_.y / unitCubeScaleFactors_.y ) ,

                brickCoordinates.z /
                ( dimensions_.z / unitCubeScaleFactors_.z ));

    // Scale the brick in a unit cube for the OpenGL texture
    const Coordinates3D brickUnitCubeScaleFactors
            ( float( brickDimensions.x * unitCubeScaleFactors_.x ) /
              dimensions_.x,

              float( brickDimensions.y * unitCubeScaleFactors_.y ) /
              dimensions_.y,

              float( brickDimensions.z * unitCubeScaleFactors_.z ) /
              dimensions_.z );

    BrickParameters< T > brickParameters;
    brickParameters.origin_.x = xi ;
    brickParameters.origin_.y = yi ;
    brickParameters.origin_.z = zi ;

    brickParameters.baseData_ = ( data_ == nullptr )? mmapAddr_ : data_ ;
    brickParameters.baseDimensions_ = dimensions_ ;
    brickParameters.dimensions_ = brickDimensions ;
    brickParameters.coordinates_ = brickCoordinates ;
    brickParameters.unitCubeCenter_ = brickUnitCubeCenter ;
    brickParameters.unitCubeScaleFactors_ = brickUnitCubeScaleFactors ;

    return brickParameters ;

}

template< class T >
QVector< Volume< T > * > Volume< T >::getBricksXAxis( uint partitions ) const
{
    // Decompose the base volume for each rendering device
    // evenly over the X-axis.
    const uint64_t newXDimension =  dimensions_.x / partitions  ;


    QVector<Volume<T> *> bricks =  QVector< Volume< T > *>( );

    //Extract Brick For each node
    for( auto i = 0 ; i < partitions - 1 ; i++ )
    {
        auto *brick = getBrick( newXDimension*i ,
                                newXDimension*( i + 1 )  ,
                                0,
                                dimensions_.y  ,
                                0,
                                dimensions_.z  );

        bricks.push_back( brick );
    }

    //The Last node will have the entire remaining brick
    auto brick = getBrick( newXDimension*( partitions - 1 ) ,
                           dimensions_.x ,
                           0,
                           dimensions_.y ,
                           0,
                           dimensions_.z );

    bricks.push_back( brick );

    return bricks ;

}

template< class T >
QVector< Volume< T > * > Volume< T >::getBricksYAxis( uint partitions ) const
{
    // Decompose the base volume for each rendering device
    // evenly over the X-axis.
    const uint64_t newYDimension =  dimensions_.y / partitions  ;


    QVector<Volume<T> *> bricks =  QVector< Volume< T > *>( );

    //Extract Brick For each node
    for( auto i = 0 ; i < partitions - 1 ; i++ )
    {
        auto *brick = getBrick( 0,
                                dimensions_.x ,
                                newYDimension*i,
                                newYDimension*( i + 1 )  ,
                                0,
                                dimensions_.z  );

        bricks.push_back( brick );
    }

    //The Last node will have the entire remaining brick
    auto brick = getBrick( 0 ,
                           dimensions_.x ,
                           newYDimension*( partitions - 1 ) ,
                           dimensions_.y ,
                           0,
                           dimensions_.z );

    bricks.push_back( brick );

    return bricks ;
}

template< class T >
QVector< Volume< T > * > Volume< T >::getBricksZAxis(uint partitions) const
{
    // Decompose the base volume for each rendering device
    // evenly over the X-axis.
    const uint64_t newZDimension =  dimensions_.z / partitions  ;


    QVector<Volume<T> *> bricks =  QVector< Volume< T > *>( );

    //Extract Brick For each node
    for( auto i = 0 ; i < partitions - 1 ; i++ )
    {
        auto *brick = getBrick( 0 ,
                                dimensions_.x ,
                                0,
                                dimensions_.y ,
                                newZDimension*i ,
                                newZDimension*( i + 1 )  );

        bricks.push_back( brick );
    }

    //The Last node will have the entire remaining brick
    auto brick = getBrick( 0 ,
                           dimensions_.x ,
                           0,
                           dimensions_.y ,
                           newZDimension*( partitions - 1 ),
                           dimensions_.z );

    bricks.push_back( brick );

    return bricks ;
}

template< class T >
QVector< Volume< T > * >
Volume< T >::heuristicBricking( const uint partitions ) const
{
    if ( partitions == 0 )
        return QVector< Volume< T > *>();


    LOG_DEBUG( "partitions = %d ", partitions );
    if( partitions % 2 == 0 )
    {
        LOG_DEBUG("Internal node.. (%d partitions)" , partitions );
        QVector< Volume< T > *> internalBricks  = QVector< Volume< T > *>( );

        if( dimensions_.isXMax( ))
            internalBricks << getBricksXAxis( 2 );

        else if( dimensions_.isYMax( ))
            internalBricks << getBricksYAxis( 2 );

        else
            internalBricks << getBricksZAxis( 2 );

        return  internalBricks[ 0 ]->heuristicBricking( partitions / 2 )
                << internalBricks[ 1 ]->heuristicBricking( partitions / 2 );
    }
    else
    {
        if( dimensions_.isXMax( ))
            return getBricksXAxis( partitions );

        else if( dimensions_.isYMax( ))
            return getBricksYAxis( partitions );

        else
            return getBricksZAxis( partitions );
    }
}

template< class T >
QVector< BrickParameters< T > >
Volume< T >::weightedBricking1D( const QVector< uint > &scores )
{
    uint totalScore = 0 ;
    for( const uint score : scores )
        totalScore += score ;

    QVector< float > xDimensionFractions ;
    for( const uint score : scores )
        xDimensionFractions.append( (float)score / (float)totalScore );

    QVector< u_int64_t > newXDimensions;
    for( const float xFraction : xDimensionFractions )
        newXDimensions.append(  xFraction * dimensions_.x );


    QVector< BrickParameters< T > > bricks;
    u_int64_t xInitial = 0;
    u_int64_t xFinal = 0 ;

    for( uint i = 0 ; i < newXDimensions.size() - 1 ; i++ )
    {
        xInitial = xFinal ;
        xFinal += newXDimensions[ i ];

        BrickParameters< T > brick = getBrickParameters( xInitial ,
                                                         xFinal ,
                                                         0 ,
                                                         dimensions_.y ,
                                                         0 ,
                                                         dimensions_.z );
        bricks.push_back( brick );

    }

    BrickParameters< T > brick = getBrickParameters( xFinal ,
                                                     dimensions_.x ,
                                                     0 ,
                                                     dimensions_.y ,
                                                     0 ,
                                                     dimensions_.z );

    bricks.push_back( brick );

    return bricks;

}

template< class T >
T* Volume< T >::getValue( const uint64_t x,
                          const uint64_t y,
                          const uint64_t z ) const
{
    const uint64_t index = get1DIndex( x, y, z );
    T* value = new T[ 1 ];
    *value = data_ != NULL ? data_[ index ] : mmapAddr_[index];
    return value;
}

template< class T >
T *Volume<T>::getValue( const Voxel3DIndex xyz ) const
{
    const u_int64_t index = get1DIndex( xyz.x, xyz.y, xyz.z );
    T* value = new T[ 1 ];
    *value = data_ != NULL ? data_[index] : mmapAddr_[index];
    return value;
}

template< class T >
T* Volume< T >::getData() const
{
    return data_;
}

template< class T >
T* Volume< T >::getMampAddr() const
{
    return mmapAddr_;
}

template< class T >
void Volume< T >::addBoundingBox_()
{
    T* ptr = ( data_ == nullptr )? mmapAddr_ : data_ ;
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
            sliceData[sliceIndex] = data_ != nullptr ? data_[get1DIndex(x, i, j)]
                                             : mmapAddr_[get1DIndex(x, i, j)];
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
            sliceData[sliceIndex] = data_ != nullptr ? data_[get1DIndex(i, y ,j)]
                                             : mmapAddr_[get1DIndex(i, y , j)];
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
            sliceData[sliceIndex] = data_ != nullptr ? data_[get1DIndex(i, j ,z)]
                                             : mmapAddr_[get1DIndex(i, j ,z)];
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
                        data_ != nullptr ? data_[get1DIndex(sliceIdx, i, j)]
                                 : mmapAddr_[get1DIndex(sliceIdx, i, j)];
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
                        data_ != nullptr ? data_[get1DIndex(i, sliceIdx , j)]
                                 : mmapAddr_[get1DIndex(i, sliceIdx , j)];

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
                        data_ != nullptr ? data_[get1DIndex(i, j ,sliceIdx )]
                                 : mmapAddr_[get1DIndex(i, j ,sliceIdx)];
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
