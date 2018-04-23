#include "Vector3.h"

template< class T >
Vector3< T >::Vector3()
{
    x = T( 0.f );
    y = T( 0.f );
    z = T( 0.f );
}

template< class T >
Vector3< T >::Vector3( const T x0, const T y0, const T z0)
{
    x = x0;
    y = y0;
    z = z0;
}

template< class T >
Vector3< T >::Vector3( const Vector3& v )
{
    assert( !v.HasNaNs());

    x = v.x;
    y = v.y;
    z = v.z;
}

template< class T >
bool Vector3< T >::HasNaNs() const
{
    return std::isnan( float( x )) || std::isnan( float( y )) || std::isnan( float( z ));
}

template< class T >
Vector3< T >& Vector3< T >::operator=( const Vector3< T >& v )
{
    assert( !v.HasNaNs());

    x = v.x;
    y = v.y;
    z = v.z;

    return *this;
}

template< class T >
Vector3< T > Vector3< T >::operator+( const Vector3< T >& v ) const
{
    assert( !v.HasNaNs());

    return Vector3< T >( x + v.x, y + v.y, z + v.z );
}

template< class T >
Vector3< T> & Vector3< T >::operator+=( const Vector3& v)
{
    assert( !v.HasNaNs());

    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

template< class T >
Vector3<T> Vector3<T>::operator-( const Vector3< T >& v ) const
{
    assert( !v.HasNaNs());

    return Vector3<T>( x - v.x, y - v.y, z - v.z );
}



template< class T >
Vector3< T >& Vector3< T >::operator-=( const Vector3< T >& v )
{
    assert( !v.HasNaNs());

    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

template< class T >
Vector3< T > Vector3< T >::operator*( const T f ) const
{
    assert( !std::isnan( f ));

    return Vector3( f * x, f * y, f * z );
}

template< class T >
Vector3< T >& Vector3< T >::operator*=( const T f )
{
    assert( !std::isnan( f ));

    x *= f;
    y *= f;
    z *= f;

    return *this;
}

template< class T >
Vector3< T > Vector3< T >::operator/( const T f ) const
{
    assert( !std::isnan( f ));

    const T inv = static_cast< T >( 1 ) / f;
    return Vector3( x * inv, y * inv, z * inv );
}

template< class T >
Vector3< T >& Vector3< T >::operator/=( const T f )
{
    assert( !std::isnan( f ));

    const float inv = 1.f / f;
    x *= inv;
    y *= inv;
    z *= inv;

    return *this;
}

template< class T >
Vector3< T > Vector3< T >::operator-() const
{
    return Vector3( -x, -y, -z );
}

template< class T >
T Vector3< T >::operator[]( const uint64_t i ) const
{
    assert( i >= 0 && i <= 2 );

    return ( &x )[i];
}

template< class T >
T& Vector3< T >::operator[]( const uint64_t i )
{
    assert( i >= 0 && i <= 2 );

    return ( &x )[i];
}

template< class T >
float Vector3< T >::lengthSquared() const
{
    return x * x + y * y + z * z;
}

template< class T >
float Vector3< T >::length() const
{
    return sqrtf( lengthSquared());
}

template< class T >
uint64_t Vector3< T >::volumeSize() const
{
    return x * y * z;
}

template< class T >
std::string Vector3< T >::toString() const
{
    std::stringstream stream;
    stream << std::setprecision(2) << "[" << x << ", " << y << ", " << z << "]";
    return stream.str();
}

template< class T >
bool Vector3< T >::isXMax() const
{
    return ( x >= y && x >= z );
}

template< class T >
bool Vector3< T >::isYMax() const
{
    return ( y >= x && y >= z );
}

template< class T >
bool Vector3< T >::isZMax() const
{
    return ( z >= x && z >= y );
}

template< class T >
bool Vector3< T >::operator==( const Vector3< T >& v ) const
{
    return ( x == v.x && y == v.y && z == v.z );
}

template< class T >
bool Vector3< T >::operator!=( const Vector3< T >& v ) const
{
    return ( x != v.x || y != v.y || z != v.z );
}

#include <Vector3.ipp>
