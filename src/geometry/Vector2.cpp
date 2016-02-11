#include "Vector2.h"

template< class T >
Vector2< T >::Vector2()
{
    x = T( 0.f );
    y = T( 0.f );
}

template< class T >
Vector2< T >::Vector2( const T x0, const T y0 )
{
    x = x0;
    y = y0;
}

template< class T >
Vector2< T >::Vector2( const Vector2< T >& v )
{
    assert( !v.hasNaNs());

    x = v.x;
    y = v.y;
}

template< class T >
bool Vector2< T >::hasNaNs() const
{
    return isnan( float( x )) || isnan( float( y ));
}

template< class T >
void Vector2< T >::set( const Vector2< T >& v )
{
    x = v.x;
    y = v.y;
}

template< class T >
void Vector2< T >::set( const T x0, const T y0 )
{
    x = x0;
    y = y0;
}

template< class T >
bool Vector2< T >::equal(  const Vector2< T >& v, float eps )
{
    float xx = static_cast<float>( x - v.x );
    float yy = static_cast<float>( y - v.y );
    return ( fabs( xx ) < eps && fabs( yy ) < eps );
}

template< class T >
float Vector2< T >::lengthSquared() const
{
    return x * x + y * y;
}

template< class T >
float Vector2< T >::length() const
{
     return sqrtf( lengthSquared());
}

template< class T >
float Vector2< T >::distance( const Vector2< T >& v ) const
{
    return sqrtf((v.x-x)*(v.x-x) + (v.y-y)*(v.y-y));
}

template< class T >
Vector2< T >& Vector2< T >::normalize()
{
    const float invLength = 1.0f / sqrtf( lengthSquared());
    x *= invLength;
    y *= invLength;
    return *this;
}

template< class T >
float Vector2< T >::dot( const Vector2<T> & v )
{
    return x * v.x + y * v.y;
}

template< class T >
float Vector2< T >::cross( const Vector2< T >& v )
{
    return ( x * v.y - y * v.x );
}

template< class T >
uint64_t Vector2< T >::imageSize() const
{
    return ( x * y );
}

template< class T >
std::string Vector2< T >::toString() const
{
    std::stringstream stream;
    stream << std::setprecision(2) << "[" << x << ", " << y << "]";
    return stream.str();
}

template< class T >
Vector2< T > & Vector2< T >::operator=( const Vector2& v )
{
    assert( !v.hasNaNs());

    x = v.x;
    y = v.y;

    return *this;
}

template< class T >
Vector2< T > Vector2< T >::operator+( const Vector2< T >& v ) const
{
    assert( !v.hasNaNs());

    return Vector2< T >( x + v.x, y + v.y );
}

template< class T >
Vector2< T >& Vector2< T >::operator+=( const Vector2& v )
{
    assert( !v.hasNaNs());

    x += v.x;
    y += v.y;

    return *this;
}

template< class T >
Vector2< T > Vector2< T >::operator-( const Vector2& v ) const
{
    assert( !v.hasNaNs());

    return Vector2( x - v.x, y - v.y );
}

template< class T >
Vector2< T >& Vector2< T >::operator-=( const Vector2& v )
{
    assert( !v.hasNaNs());

    x -= v.x;
    y -= v.y;

    return *this;
}

template< class T >
Vector2< T > Vector2< T >::operator*( const T f ) const
{
    assert( !isnan( f ));

    return Vector2( f * x, f * y );
}

template< class T >
Vector2< T >& Vector2< T >::operator*=( const T f )
{
    assert( !isnan( f ));

    x *= f;
    y *= f;

    return *this;
}

template< class T >
Vector2< T > Vector2< T >::operator/( const T f ) const
{
    assert( !isnan( f ));

    const T inv = static_cast< T >( 1 ) / f;
    return Vector2( x * inv, y * inv );
}

template< class T >
Vector2< T >& Vector2< T >::operator/=( const T f )
{
    assert( !isnan( f ));

    const float inv = 1.f / f;
    x *= inv;
    y *= inv;

    return *this;
}

template< class T >
Vector2< T > Vector2< T >::operator-() const
{
    return Vector2< T >( -x, -y );
}

template< class T >
T Vector2< T >::operator[]( const u_int64_t i ) const
{
    assert( i >= 0 && i <= 1 );

    if( i == 0 )
        return x;
    else
        return y;
}

template< class T >
T& Vector2< T >::operator[]( const u_int64_t i )
{
    assert( i >= 0 && i <= 1 );

    if( i == 0 )
        return x;
    else
        return y;
}

template< class T >
std::ostream& Vector2< T >::operator<<( std::ostream& stream )
{
    stream << "[" << x << ", " << y << "]";
    return stream;
}


template< class T >
bool Vector2< T >::operator==( const Vector2& v ) const
{
    return ( x == v.x && y == v.y );
}

template< class T >
bool Vector2< T >::operator!=( const Vector2& v ) const
{
    return ( x != v.x || y != v.y );
}

#include <Vector2.ipp>
