#ifndef VIRTUALIMAGE_H
#define VIRTUALIMAGE_H


class VirtualImage
{
    enum Dim: int { X=0 , Y  , Z } ;

public:
    VirtualImage(int (&d) [2] , float (&c) [3] );

    VirtualImage( const int x , const int y  , float (&c) [3] );

    VirtualImage( const int x , const int y ) ;

    int dim( unsigned int index ) const;

    int (&dim()) [2];
    float (&center()) [3];
    int id() const;
private :
    int d_[2];
    float center_[3];
    int id_;
};

#endif // VIRTUALIMAGE_H
