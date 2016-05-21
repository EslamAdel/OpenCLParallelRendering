#ifndef VECTOR2_H
#define VECTOR2_H

#include "Headers.hh"

template < class T >
class Vector2
{
public:

    /**
     * @brief Vector2
     */
    Vector2();

    /**
     * @brief Vector2
     * @param x0
     * @param y0
     * @param z0
     */
    Vector2( const T x0, const T y0 );

    /**
     * @brief Vector2
     * @param v
     */
    Vector2( const Vector2& v );

    /**
     * @brief hasNaNs
     * @return
     */
    bool hasNaNs() const;

    /**
     * @brief set
     * @param v
     */
    void set( const Vector2& v );

    /**
     * @brief set
     * @param x0
     * @param y0
     */
    void set( const T x0, const T y0 );

    /**
     * @brief equal
     * Compares with epsilon.
     * @param v
     * @param eps
     * @return
     */
    bool equal(  const Vector2& v, const float eps = 0.00001 );

    /**
     * @brief lengthSquared
     * @return
     */
    float lengthSquared() const;

    /**
     * @brief length
     * @return
     */
    float length() const;

    /**
     * @brief distance
     * Computes the distance between two vectors.
     * @param v
     * @return
     */
    float distance( const Vector2& v ) const;

    /**
     * @brief normalize
     * @return
     */
    Vector2& normalize();

    /**
     * @brief dot
     * @param v
     * @return
     */
    float dot( const Vector2& v );

    /**
     * @brief cross
     * @param v
     * @return
     */
    float cross( const Vector2& v );

    /**
     * @brief imageSize
     * @return
     */
    uint64_t imageSize() const;

    /**
     * @brief toString
     * @return
     */
    std::string toString() const;

public:

    /**
     * @brief operator =
     * @param v
     * @return
     */
    Vector2 &operator=( const Vector2& v );

    /**
     * @brief operator +
     * @param v
     * @return
     */
    Vector2 operator+( const Vector2& v ) const;

    /**
     * @brief operator +=
     * @param v
     * @return
     */
    Vector2& operator+=( const Vector2& v ) ;

    /**
     * @brief operator -
     * @param v
     * @return
     */
    Vector2 operator-( const Vector2& v ) const ;

    /**
     * @brief operator -=
     * @param v
     * @return
     */
    Vector2& operator-=( const Vector2& v ) ;

    /**
     * @brief operator *
     * @param f
     * @return
     */
    Vector2 operator*( const T f ) const ;

    /**
     * @brief operator *=
     * @param f
     * @return
     */
    Vector2 &operator*=( const T f );

    /**
     * @brief operator /
     * @param f
     * @return
     */
    Vector2 operator/( const T f ) const ;

    /**
     * @brief operator /=
     * @param f
     * @return
     */
    Vector2 &operator/=( const T f ) ;

    /**
     * @brief operator -
     * @return
     */
    Vector2 operator-() const;

    /**
     * @brief operator []
     * @param i
     * @return
     */
    T operator[]( const u_int64_t i ) const ;

    /**
     * @brief operator []
     * @param i
     * @return
     */
    T& operator[]( const u_int64_t i );

    /**
     * @brief operator <<
     * @param stream
     * @return
     */
    std::ostream& operator<<( std::ostream& stream );

    /**
     * @brief operator ==
     * @param v
     * @return
     */
    bool operator==( const Vector2& v ) const ;

    /**
     * @brief operator !=
     * @param v
     * @return
     */
    bool operator!=( const Vector2& v ) const ;

    /**
     * @brief operator <
     * @param v
     * @return
     */
    bool operator<( const Vector2& v ) const;

    /**
     * @brief operator >
     * @param v
     * @return
     */
    bool operator>( const Vector2& v ) const;

public:

    /**
     * @brief x
     */
    T x;

    /**
     * @brief y
     */
    T y;
};

/**
 * @brief Vector2I
 * A 64-bit integer 2D vector.
 */
typedef Vector2< int64_t > Vector2I;

/**
 * @brief Vector3UI
 * A 64-bit unsigned 2D integer vector.
 */
typedef Vector2< u_int64_t > Vector2UI;

/**
 * @brief Vector3F
 * A single precision 2D vector.
 */
typedef Vector2< float > Vector2F;

/**
 * @brief Vector3D
 * A double precision 2D vector
 */
typedef Vector2< double > Vector2D;

#endif // VECTOR2_H
