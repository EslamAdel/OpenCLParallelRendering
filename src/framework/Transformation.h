#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include "Typedefs.hh"

struct Transformation
{
    Transformation();


    //Transformation parameters
    /**
     * @brief rotation
     */
    Coordinates3D rotation ;

    /**
     * @brief translation
     */
    Coordinates3D translation ;

    /**
     * @brief scale
     */
    Coordinates3D scale ;

    /**
     * @brief brightness
     */
    float brightness ;

    /**
     * @brief volumeDensity
     */
    float volumeDensity ;

    /**
     * @brief transferFunctionFlag
     */
    int transferFunctionFlag ;

    /**
     * @brief transferFunctionScale
     */
    float transferFunctionScale ;

    /**
     * @brief transferFunctionOffset
     */
    float transferFunctionOffset ;

};

#endif // TRANSFORMATION_H
