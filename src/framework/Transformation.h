#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include "Typedefs.hh"

struct Transformation
{
    Transformation();
    //Transformation parameters
    Coordinates3D rotation ;
    Coordinates3D translation ;
    Coordinates3D scale ;
    float brightness ;
    float volumeDensity ;
    float transferFunctionScale ;
    float transferFunctionOffset ;

};

#endif // TRANSFORMATION_H
