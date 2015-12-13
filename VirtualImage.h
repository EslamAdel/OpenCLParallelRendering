#ifndef VIRTUALIMAGE_H
#define VIRTUALIMAGE_H


class VirtualImage
{
    enum Dim: int { X=0 , Y  , Z } ;

public:
    VirtualImage( int (&dim) [2] , float (&center) [3] );

private :
    int d_[2];
    float center_[3];
};

#endif // VIRTUALIMAGE_H
