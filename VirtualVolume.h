#ifndef VIRTUALVOLUME_H
#define VIRTUALVOLUME_H

#include <QList>

class VirtualVolume
{

enum Dim : quint8 { X=0 , Y , Z };

public:
    VirtualVolume(const int x, const int y , const int z, float (&center)[3] );

//    VirtualVolume( const VirtualVolume &copy );

    VirtualVolume(const int (&dimensions)[3] ,const float (&center)[3] );

    VirtualVolume(const int x, const int y , const int z );

    float (&center())[3];
    int (&dim()) [3];
    Dim maxDim() const ;

    QList<VirtualVolume *> *subVolumes1D( const int nBricks );

private:
    int d_[3];
    float center_[3];
};

#endif // VIRTUALVOLUME_H
