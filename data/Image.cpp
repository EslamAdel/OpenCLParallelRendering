#include "Image.h"

template< class T >
Image< T >::Image( const Dimensions2D dimensions, T* data )
{
    dimensions_ = dimensions;
    data_ = data;
}

template< class T >
void Image< T >::setImageData(T *data)
{
    data_ = data;
}

template< class T >
Dimensions2D Image< T >::getDimensions() const
{
    return dimensions_;
}

template< class T >
uint64_t Image< T >::getSizeX() const
{
    return dimensions_.x;
}

template< class T >
uint64_t Image< T >::getSizeY() const
{
    return dimensions_.y;
}

template< class T >
uint64_t Image< T >::getSizeInBytes() const
{
    return dimensions_.imageSize() * sizeof( T );
}

template< class T >
T* Image< T >::getData() const
{
    return data_;
}

template< class T >
uint64_t Image< T >::get1DIndex( const uint64_t x, const uint64_t y ) const
{
    // Flat[x + WIDTH * y] = Original[x, y]
    return ( x + ( dimensions_.x * y ));
}

template< class T >
uint64_t Image< T >::get1DIndex( const Pixel2DIndex index ) const
{
    // Flat[x + WIDTH * y] = Original[x, y]
    return ( index.x + ( dimensions_.x * index.y ));
}

template< class T >
T* Image<T>::getValue( const uint64_t x, const uint64_t y ) const
{
    const uint64_t index = get1DIndex( x, y );
    T* value = new T[ 1 ];
    *value = data_[ index ];
    return value;
}

template< class T >
T* Image< T >::getValue( const Pixel2DIndex xy ) const
{
    const u_int64_t index = get1DIndex( xy.x, xy.y );
    T* value = new T[ 1 ];
    *value = data_[ index ];
    return value;
}

#include <Image.ipp>
