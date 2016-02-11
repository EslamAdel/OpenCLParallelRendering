#ifndef VECTOR3_H
#define VECTOR3_H

#include <Headers.hh>

template < class T >
class Vector3
{
public:

    /**
     * @brief Vector3
     */
    Vector3();

    /**
     * @brief Vector3
     * @param x0
     * @param y0
     * @param z0
     */
    Vector3( const T x0, const T y0, const T z0 );

    /**
     * @brief Vector3
     * @param v
     */
    Vector3( const Vector3& v );


    /**
     * @brief HasNaNs
     * @return
     */
    bool HasNaNs() const;

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
     * @brief volumeSize
     * @return
     */
    uint64_t volumeSize() const;

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
    Vector3 &operator=( const Vector3& v );

    /**
     * @brief operator +
     * @param v
     * @return
     */
    Vector3 operator+( const Vector3& v ) const;

    /**
     * @brief operator +=
     * @param v
     * @return
     */
    Vector3& operator+=( const Vector3& v ) ;

    /**
     * @brief operator -
     * @param v
     * @return
     */
    Vector3 operator-( const Vector3& v ) const ;

    /**
     * @brief operator -=
     * @param v
     * @return
     */
    Vector3& operator-=( const Vector3& v ) ;

    /**
     * @brief operator *
     * @param f
     * @return
     */
    Vector3 operator*( const T f ) const ;

    /**
     * @brief operator *=
     * @param f
     * @return
     */
    Vector3 &operator*=( const T f );

    /**
     * @brief operator /
     * @param f
     * @return
     */
    Vector3 operator/( const T f ) const ;

    /**
     * @brief operator /=
     * @param f
     * @return
     */
    Vector3 &operator/=( const T f ) ;

    /**
     * @brief operator -
     * @return
     */
    Vector3 operator-() const;

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
    bool operator==( const Vector3& v ) const ;

    /**
     * @brief operator !=
     * @param v
     * @return
     */
    bool operator!=( const Vector3& v ) const ;

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
};

/**
 * @brief Vector3I
 * A 64-bit integer vector.
 */
typedef Vector3< int64_t > Vector3I;

/**
 * @brief Vector3UI
 * A 64-bit unsigned integer vector.
 */
typedef Vector3< u_int64_t > Vector3UI;

/**
 * @brief Vector3F
 * A single precision vector.
 */
typedef Vector3< float > Vector3F;

/**
 * @brief Vector3D
 * A double precision vector
 */
typedef Vector3< double > Vector3D;

#endif // VECTOR3_H
