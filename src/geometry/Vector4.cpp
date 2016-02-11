#include "Vector4.h"

template< class T >
Vector4< T >::Vector4()
{
    x = T( 0.f );
    y = T( 0.f );
    z = T( 0.f );
}

template< class T >
Vector4< T >::Vector4(const T x0, const T y0, const T z0, const T w0 )
{
    x = x0;
    y = y0;
    z = z0;
    w = w0;
}

template< class T >
Vector4< T >::Vector4( const Vector4& v )
{
    assert( !v.HasNaNs());

    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}

template< class T >
bool Vector4< T >::HasNaNs() const
{
    return isnan( float( x )) || isnan( float( y ))
            || isnan( float( z )) || isnan( float( w ));
}

template< class T >
Vector4< T >& Vector4< T >::operator=( const Vector4< T >& v )
{
    assert( !v.HasNaNs());

    x = v.x;
    y = v.y;
    z = v.z;
    z = v.w;

    return *this;
}

template< class T >
Vector4< T > Vector4< T >::operator+( const Vector4< T >& v ) const
{
    assert( !v.HasNaNs());

    return Vector4< T >( x + v.x, y + v.y, z + v.z, w + v.w );
}

template< class T >
Vector4< T> & Vector4< T >::operator+=( const Vector4& v)
{
    assert( !v.HasNaNs());

    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;

    return *this;
}

template< class T >
Vector4<T> Vector4<T>::operator-( const Vector4< T >& v ) const
{
    assert( !v.HasNaNs());

    return Vector4<T>( x - v.x, y - v.y, z - v.z, w - v.w );
}



template< class T >
Vector4< T >& Vector4< T >::operator-=( const Vector4< T >& v )
{
    assert( !v.HasNaNs());

    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;

    return *this;
}

template< class T >
Vector4< T > Vector4< T >::operator*( const T f ) const
{
    assert( !isnan( f ));

    return Vector4( f * x, f * y, f * z, f * w );
}

template< class T >
Vector4< T >& Vector4< T >::operator*=( const T f )
{
    assert( !isnan( f ));

    x *= f;
    y *= f;
    z *= f;
    w *= f;

    return *this;
}

template< class T >
Vector4< T > Vector4< T >::operator/( const T f ) const
{
    assert( !isnan( f ));

    const T inv = static_cast< T >( 1 ) / f;
    return Vector4( x * inv, y * inv, z * inv, w * inv );
}

template< class T >
Vector4< T >& Vector4< T >::operator/=( const T f )
{
    assert( !isnan( f ));

    const float inv = 1.f / f;
    x *= inv;
    y *= inv;
    z *= inv;
    w *= inv;

    return *this;
}

template< class T >
Vector4< T > Vector4< T >::operator-() const
{
    return Vector4( -x, -y, -z, -w );
}

template< class T >
T Vector4< T >::operator[]( const uint64_t i ) const
{
    assert( i >= 0 && i <= 3 );

    return ( &x )[i];
}

template< class T >
T& Vector4< T >::operator[]( const uint64_t i )
{
    assert( i >= 0 && i <= 3 );

    return ( &x )[i];
}


template< class T >
std::string Vector4< T >::toString() const
{
    std::stringstream stream;
    stream << std::setprecision(2)
           << "[" << x << ", " << y << ", " << z << ", " << w << "]";
    return stream.str();
}

template< class T >
bool Vector4< T >::operator==( const Vector4< T >& v ) const
{
    return ( x == v.x && y == v.y && z == v.z && w == v.w );
}

template< class T >
bool Vector4< T >::operator!=( const Vector4< T >& v ) const
{
    return ( x != v.x || y != v.y || z != v.z || w != v.w );
}

#include <Vector4.ipp>
