#ifndef VOLUME_H
#define VOLUME_H



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <memory>

#include <QVector>

#include "Headers.h"
#include "Typedefs.hh"
#include "Image.h"
#include "BrickParameters.h"
#include "VolumeUtilities.h"

// Forward declaration
template< class T > class SerializableVolume;


template < class T >
class Volume
{
protected:
    friend class SerializableVolume< T > ;

public: // Constructors


    Volume( const bool drawBoundingBox = false );

    /**
     * @brief Volume
     * @param prefix
     */
    Volume( const std::string prefix,
            const bool memoryMapVolume = true ,
            const bool drawBoundingBox = false );

    /**
     * @brief Volume
     * @param brickCoordinates
     * @param brickDimensions
     * @param brickUnitCubeCenter
     * @param brickUnitCubeScaleFactors
     * @param brickData
     * @param drawBoundingBox
     */
    Volume( const Coordinates3D brickCoordinates ,
            const Dimensions3D brickDimensions ,
            const Coordinates3D brickUnitCubeCenter ,
            const Coordinates3D brickUnitCubeScaleFactors ,
            T *brickData,
            const bool drawBoundingBox = false ) ;


    /**
     * @brief Volume
     * @param brickParameters
     */
    Volume( const BrickParameters< T > brickParameters ,
            const bool drawBoundingBox = false );


    ~Volume();

public: // Public functions

    Volume< T > &operator=( const Volume< T > &volume );

    /**
     * @brief getDimensions
     * @return The XYZ dimensions of the volume.
     */
    Dimensions3D getDimensions() const;


    /**
     * @brief getCubeCenter
     * @return
     */
    Coordinates3D getCubeCenter() const;

    /**
     * @brief getUnitCubeCenter
     * @return
     */
    Coordinates3D getUnitCubeCenter() const;

    /**
     * @brief getUnitCubeScaleFactors
     * @return
     */
    Coordinates3D getUnitCubeScaleFactors() const;

    /**
     * @brief getSizeX
     * @return Width of the volume.
     */
    uint64_t getSizeX() const;

    /**
     * @brief getSizeY
     * @return Height of the volume.
     */
    uint64_t getSizeY() const;

    /**
     * @brief getSizeZ
     * @return Depth of the volume.
     */
    uint64_t getSizeZ() const;

    /**
     * @brief getSizeInBytes
     * @return Size of the volume in bytes.
     */
    virtual uint64_t getSizeInBytes() const;

    /**
     * @brief getValue
     * Gets the value of a voxel in the volume specified by the XYZ coordinates.
     * @param x
     * @param y
     * @param z
     * @return Voxel value at the given coordinates.
     */
    virtual T* getValue( const uint64_t x, const uint64_t y, const uint64_t z ) const;

    /**
     * @brief getValue
     * @param xyz
     * @return Voxel value at the given voxel index.
     */
    virtual T* getValue( const Voxel3DIndex xyz ) const;

    /**
     * @brief getData
     * @return A pointer to the data of the volume.
     */
    virtual T* getData() const;


    /**
     * @brief operator []
     * @param index
     * @return
     */
    T &operator[]( const uint64_t index );

    /**
     * @brief copyData
     * @param data
     */
    void copyData( const T *data );

    /**
     * @brief copyData
     * @param data
     */
    void copyData( const BrickParameters< T > &brickParameters );

    /**
     * @brief getMampAddr
     * @return
     */
    virtual T* getMmapAddr() const;

    /**
     * @brief loadFile
     * @param prefix
     * @param memoryMapVolume
     */
    void loadFile( const std::string prefix,
                   const bool memoryMapVolume = true );
    /**
     * @brief get1DIndex
     * Computes the 1D index of a voxel in a 3D volume given by the XYZ
     * coordinates.
     * @param x
     * @param y
     * @param z
     * @return 1D index of the requested voxel.
     */
    uint64_t get1DIndex( const uint64_t x,
                         const uint64_t y,
                         const uint64_t z ) const;

    /**
     * @brief get1DIndex
     * Computes the 1D index of a voxel in a 3D volume given by the XYZ
     * coordinates.
     * @param index
     * @return
     */
    uint64_t get1DIndex( const Voxel3DIndex index ) const;

    /**
     * @brief getSliceX
     * Extracts an orthogonal slice from the volume along spefific x coordinate.
     * @param x
     * @return
     */
    Image<T>* getSliceX( const u_int64_t x ) const;

    /**
     * @brief getSliceY
     * Extracts an orthogonal slice from the volume along spefific y coordinate.
     * @param y
     * @return
     */
    Image<T>* getSliceY( const u_int64_t y ) const;

    /**
     * @brief getSliceZ
     * Extracts an orthogonal slice from the volume along spefific z coordinate.
     * @param z
     * @return
     */
    Image<T>* getSliceZ( const u_int64_t z ) const;

    /**
     * @brief getProjectionX
     * Computes the projection of the volume along the X axis.
     * @return
     */
    Image<T>* getProjectionX() const;

    /**
     * @brief getProjectionY
     * Computes the projection of the volume along the Y axis.
     * @return
     */
    Image<T>* getProjectionY() const;

    /**
     * @brief getProjectionZ
     * Computes the projection of the volume along the Z axis.
     * @return
     */
    Image<T>* getProjectionZ() const;


    /**
     * @brief getBrick
     * @param xi
     * @param xf
     * @param yi
     * @param yf
     * @param zi
     * @param zf
     * @return
     */
    Volume<T> *getBrick( const u_int64_t xi ,
                         const u_int64_t xf ,
                         const u_int64_t yi ,
                         const u_int64_t yf ,
                         const u_int64_t zi ,
                         const u_int64_t zf ) const ;


    /**
     * @brief getBrickParameters
     * @param xi
     * @param xf
     * @param yi
     * @param yf
     * @param zi
     * @param zf
     * @return
     */
    BrickParameters< T > getBrickParameters( const u_int64_t xi ,
                                             const u_int64_t xf ,
                                             const u_int64_t yi ,
                                             const u_int64_t yf ,
                                             const u_int64_t zi ,
                                             const u_int64_t zf ) const ;

    /**
     * @brief getBricksXAxis
     * @param partitions
     * @return
     */
    QVector< Volume< T > *> getBricksXAxis( uint partitions ) const;

    /**
     * @brief getBricksYAxis
     * @param partitions
     * @return
     */
    QVector< Volume< T > *> getBricksYAxis( uint partitions ) const;

    /**
     * @brief getBricksZAxis
     * @param partitions
     * @return
     */
    QVector< Volume< T > *> getBricksZAxis( uint partitions ) const;


    /**
     * @brief heuristicBricking
     * @param partitions
     * @return
     */
    QVector< Volume< T > *>
    heuristicBricking( const uint partitions ) const;

    /**
     * @brief weightedBricking1D
     * @param scores
     * @return
     */
    QVector< BrickParameters< T > >
    weightedBricking1D( const QVector< uint > &scores ) const ;

    /**
     * @brief weightedBrickingWithCopy1D
     * @param scores
     * @return
     */
    QVector< Volume< T > *>
    weightedBrickingWithCopy1D( const QVector<uint> &scores ) const ;

    /**
     * @brief fromBrickParameters
     * @param brickParamters
     * @return
     */
    static Volume< T >* fromBrickParameters(
            const BrickParameters< T > &brickParamters );

    /**
     * @brief bufferDeleter
     * @param p
     */
    static void bufferDeleter( T  *p );

protected: // Protected functions

    /**
     * @brief loadHeaderData_
     * Loads a meta file that contains all the meta data of the volume
     * including its type and dimensions.
     * @param prefix
     */
    void loadHeaderData_( const std::string prefix );

    /**
     * @brief loadVolumeData_
     * Loads the content of the volume from the HDD into an array on the CPU
     * memory.
     * @param prefix
     */
    void loadVolumeData_( const std::string prefix );

    /**
     * @brief mapVolumeData
     * @param prefix
     */
    void mapVolumeData_( const std::string prefix );

    /**
     * @brief addBoundingBox_
     * Adds the bounding box to the data
     */
    void addBoundingBox_();

    /**
     * @brief zeroPad_
     * Gets the largest dimension of the volume and zeropads the other
     * dimensions.
     */
    virtual void zeroPad_();

protected:
    /**
     * @brief dimensions_
     */
    Dimensions3D dimensions_;

    /**
     * @brief coordinates_
     */
    Coordinates3D coordinates_ ;

    /**
     * @brief unitCubeCenter_
     */
    Coordinates3D unitCubeCenter_;

    /**
     * @brief unitCubeScaleFactors_
     */
    Coordinates3D unitCubeScaleFactors_;

    /**
     * @brief sizeInBytes_
     */
    u_int64_t sizeInBytes_;

    /**
     * @brief data_
     */
    std::shared_ptr< T > data_;

    /**
     * @brief mmapAddr_
     */
    std::shared_ptr< T > mmapAddr_;

    /**
     * @brief drawBoundingBox_
     */
    const bool drawBoundingBox_;


};

/**
 * @brief Volume8
 * An unsigned 8-bit Volume.
 */
typedef Volume< u_int8_t > Volume8;

/**
 * @brief Volume16
 * An unsigned 16-bit Volume.
 */
typedef Volume< u_int16_t > Volume16;

/**
 * @brief Volume32
 * An unsigned 32-bit Volume.
 */
typedef Volume< u_int32_t > Volume32;

/**
 * @brief Volume64
 * An unsigned 64-bit Volume.
 */
typedef Volume< u_int64_t > Volume64;

/**
 * @brief VolumeF
 * An single precision Volume.
 */
typedef Volume< float > VolumeF;

/**
 * @brief VolumeD
 * An double precision Volume.
 */
typedef Volume< double > VolumeD;




#endif // VOLUME_H
