#include "Transformation.h"

Transformation::Transformation()
{

}

Transformation::Transformation(const Transformation &copy)
{
    *this = copy;
}

Transformation &Transformation::operator=(const Transformation &rhs)
{
    translation = rhs.translation;

    rotation = rhs.rotation;

    scale = rhs.scale;

    volumeDensity = rhs.volumeDensity;

    imageBrightness = rhs.imageBrightness;
}

//float Transformation::volumeDensity() const
//{
//    return volumeDensity_;
//}

//float Transformation::brightness() const
//{
//    return imageBrightness_;
//}

//Coordinates3D &Transformation::rotation()
//{
//    return rotation_;
//}

//Coordinates3D &Transformation::translation()
//{
//    return translation_;
//}

//Coordinates3D &Transformation::scale()
//{
//    return scale_;
//}


