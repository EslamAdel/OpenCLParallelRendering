#ifndef BRICKPARAMETERS_H
#define BRICKPARAMETERS_H

#include <Typedefs.hh>

// forward declaration
template< class T > class Volume;
template< class T > class SerializableVolume;

template < class T >
class BrickParameters
{
public:
    BrickParameters();

private:
    friend class Volume< T > ;
    friend class SerializableVolume< T > ;

    Dimensions3D brickDimensions_ ;
    Dimensions3D baseDimensions_ ;
    Dimensions3D origin_ ;
    Coordinates3D brickCoordinates_ ;
    Coordinates3D brickUnitCubeCenter_ ;
    Coordinates3D brickUnitCubeScaleFactors_ ;
    T *baseData_ ;

};

#endif // BRICKPARAMETERS_H
