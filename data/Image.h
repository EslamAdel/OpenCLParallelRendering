#ifndef IMAGE_H
#define IMAGE_H

#include <Headers.hh>
#include <Typedefs.hh>
#include <QImage>

template< class T >
class Image
{
public:

    /**
     * @brief Image
     * @param dimensions
     * @param data
     */
    Image( const Dimensions2D dimensions, T* data );

public: // Public functions

    /**
     * @brief getDimensions
     * @return The XY dimensions of the image.
     */
    Dimensions2D getDimensions() const;

    /**
     * @brief getSizeX
     * @return Width of the image.
     */
    uint64_t getSizeX() const;

    /**
     * @brief getSizeY
     * @return Height of the image.
     */
    uint64_t getSizeY() const;

    /**
     * @brief get1DIndex
     * Computes the 1D index of a pixel in a 2D image given by the XY
     * coordinates.
     * @param x
     * @param y
     * @param z
     * @return 1D index of the requested pixel.
     */
    uint64_t get1DIndex( const uint64_t x,
                         const uint64_t y ) const;

    /**
     * @brief get1DIndex
     * Computes the 1D index of a pixel in a 2D image given by the XY
     * coordinates.
     * @param index
     * @return
     */
    uint64_t get1DIndex( const Pixel2DIndex index ) const;

    /**
     * @brief getSizeInBytes
     * @return Size of the image in bytes.
     */
    virtual uint64_t getSizeInBytes() const;

    /**
     * @brief getData
     * @return A pointer to the data of the volume.
     */
    virtual T* getData() const;

    /**
     * @brief getValue
     * Gets the value of a pixel in the volume specified by the XY coordinates.
     * @param x
     * @param y
     * @return Pixel value at the given coordinates.
     */
    virtual T* getValue( const uint64_t x, const uint64_t y ) const;

    /**
     * @brief getValue
     * @param xy
     * @return Pixel value at the given pixel index.
     */
    virtual T* getValue( const Pixel2DIndex xy ) const;

protected: // Protected (private) member variables

    /**
     * @brief dimensions_
     */
    Dimensions2D dimensions_;

    /**
     * @brief sizeInBytes_
     */
    u_int64_t sizeInBytes_;

    /**
     * @brief data_
     */
    T* data_;
};

/**
 * @brief Image8
 * An unsigned 8-bit Image.
 */
typedef Image< u_int8_t > Image8;

/**
 * @brief Image16
 * An unsigned 16-bit Image.
 */
typedef Image< u_int16_t > Image16;

/**
 * @brief Image32
 * An unsigned 32-bit Image.
 */
typedef Image< u_int32_t > Image32;

/**
 * @brief Image64
 * An unsigned 64-bit Image.
 */
typedef Image< u_int64_t > Image64;

/**
 * @brief ImageF
 * An single precision Image.
 */
typedef Image< float > ImageF;

/**
 * @brief ImageD
 * An double precision Image.
 */
typedef Image< double > ImageD;

#endif // IMAGE_H
