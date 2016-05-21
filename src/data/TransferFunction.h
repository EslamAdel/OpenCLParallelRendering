#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H
#include "Headers.h"

struct RGBA
{
    float* array;
    size_t length;
};

#include <Typedefs.hh>

class TransferFunction
{
public:
    TransferFunction( const Colors& colors );

public:

    /**
     * @brief getColors
     * @return
     */
    Colors getColors( ) const;

    /**
     * @brief getData
     * @return
     */
    RGBA getData( ) const;

private:

    /**
     * @brief colors_
     */
    Colors colors_;

    /**
     * @brief rgbaArray_
     * An array that stores the transfer function colors in a compatible format
     * with OpenCL that could be shoved directly to the device.
     * This is a one-dimensional array that has a lenght of n * 4, where n is
     * number of points in the transfer function sorted in the following order
     * [ r1 g1 b1 a1 r2 g2 b2 a2 r3 g3 b3 a3 .... .... .... .... r4 g4 b4 a4 ]
     */
    float* rgbaArray_;
};

#endif // TRANSFERFUNCTION_H
