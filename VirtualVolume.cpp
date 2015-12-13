#include "VirtualVolume.h"


VirtualVolume::VirtualVolume(const int x, const int y, const int z , float (&center)[3])
{
    d_[Dim::X] = x;
    d_[Dim::Y] = y;
    d_[Dim::Z] = z;
    center_[Dim::X] = center[Dim::X];
    center_[Dim::Y] = center[Dim::Y];
    center_[Dim::Z] = center[Dim::Z];
}

//VirtualVolume::VirtualVolume(const VirtualVolume &copy)
//{
//    d_[Dim::X] = copy.dim()[Dim::X];
//    d_[Dim::Y] = copy.dim()[Dim::Y];
//    d_[Dim::Z] = copy.dim()[Dim::Z];
//    center_[Dim::X] = copy.center()[Dim::X];
//    center_[Dim::Y] = copy.center()[Dim::Y];
//    center_[Dim::Z] = copy.center()[Dim::Z];
//}

VirtualVolume::VirtualVolume(const int (&dimensions)[3], const float (&center)[3])
{
    d_[Dim::X] = dimensions[Dim::X];
    d_[Dim::Y] = dimensions[Dim::Y];
    d_[Dim::Z] = dimensions[Dim::Z];
    center_[Dim::X] = center[Dim::X];
    center_[Dim::Y] = center[Dim::Y];
    center_[Dim::Z] = center[Dim::Z];

}

VirtualVolume::VirtualVolume(const int x, const int y, const int z)
{
    d_[Dim::X] = x;
    d_[Dim::Y] = y;
    d_[Dim::Z] = z;
    center_[Dim::X] = static_cast<float>(x)/2;
    center_[Dim::X] = static_cast<float>(y)/2;
    center_[Dim::X] = static_cast<float>(z)/2;
}

int ( &VirtualVolume::dim() ) [3]
{
    return d_;
}

VirtualVolume::Dim VirtualVolume::maxDim() const
{
    return (d_[0] > d_[1] )? (d_[0] > d_[2])? Dim::X : Dim::Z  : (d_[1] > d_[2])? Dim::Y : Dim::Z ;
}

QList<VirtualVolume *> *VirtualVolume::subVolumes1D(const int nBricks)
{
    QList<VirtualVolume *> *bricks = new QList<VirtualVolume *> ;

    Dim maxDimension = this->maxDim();


    int (&newDim)[3] = dim();
    float (&newCenter)[3] = center();

    int step = static_cast<float>( newDim[ maxDimension ] )/nBricks;
    newDim[ maxDimension ] = step;

    for( auto i=0 ; i< nBricks ; i++ )
    {
        newCenter[maxDimension] += step ;
        VirtualVolume *newBrick = new VirtualVolume(newDim,newCenter);
        bricks->append( newBrick );
    }

    return bricks;
}

float ( &VirtualVolume::center() ) [3]
{
    return center_;
}

