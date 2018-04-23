#ifndef VECTOR4_H
#define VECTOR4_H

#include "Headers.hh"
#include <math.h>

template < class T >
class Vector4
{
public:

    /**
     * @brief Vector4
     */
    Vector4();

    /**
     * @brief Vector4
     * @param x0
     * @param y0
     * @param z0
     */
    Vector4( const T x0, const T y0, const T z0, const T w0 );

    /**
     * @brief Vector4
     * @param v
     */
    Vector4( const Vector4& v );


    /**
     * @brief HasNaNs
     * @return
     */
    bool HasNaNs() const;

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
    Vector4 &operator=( const Vector4& v );

    /**
     * @brief operator +
     * @param v
     * @return
     */
    Vector4 operator+( const Vector4& v ) const;

    /**
     * @brief operator +=
     * @param v
     * @return
     */
    Vector4& operator+=( const Vector4& v ) ;

    /**
     * @brief operator -
     * @param v
     * @return
     */
    Vector4 operator-( const Vector4& v ) const ;

    /**
     * @brief operator -=
     * @param v
     * @return
     */
    Vector4& operator-=( const Vector4& v ) ;

    /**
     * @brief operator *
     * @param f
     * @return
     */
    Vector4 operator*( const T f ) const ;

    /**
     * @brief operator *=
     * @param f
     * @return
     */
    Vector4 &operator*=( const T f );

    /**
     * @brief operator /
     * @param f
     * @return
     */
    Vector4 operator/( const T f ) const ;

    /**
     * @brief operator /=
     * @param f
     * @return
     */
    Vector4 &operator/=( const T f ) ;

    /**
     * @brief operator -
     * @return
     */
    Vector4 operator-() const;

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
     * @brief operator ==
     * @param v
     * @return
     */
    bool operator==( const Vector4& v ) const ;

    /**
     * @brief operator !=
     * @param v
     * @return
     */
    bool operator!=( const Vector4& v ) const ;

public:

    /**
     * @brief x
     */
    T x;

    /**
     * @brief y
     */
    T y;

    /**
     * @brief z
     */
    T z;

    /**
     * @brief w
     */
    T w;
};

/**
 * @brief Vector4I
 * A 64-bit integer vector.
 */
typedef Vector4< int64_t > Vector4I;

/**
 * @brief Vector4UI
 * A 64-bit unsigned integer vector.
 */
typedef Vector4< u_int64_t > Vector4UI;

/**
 * @brief Vector4F
 * A single precision vector.
 */
typedef Vector4< float > Vector4F;

/**
 * @brief Vector4D
 * A double precision vector
 */
typedef Vector4< double > Vector4D;

#endif // VECTOR4_H
