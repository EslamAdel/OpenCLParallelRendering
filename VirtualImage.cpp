#include "VirtualImage.h"
#include <stdlib.h>

VirtualImage::VirtualImage(int (&d)[2], float (&c)[3])
{
    d_[Dim::X] = d[Dim::X];
    d_[Dim::Y] = d[Dim::Y];
    center_[Dim::X] = c[Dim::X];
    center_[Dim::Y] = c[Dim::Y];
    center_[Dim::Z] = c[Dim::Z];
    id_ = rand();
}

VirtualImage::VirtualImage(const int x, const int y, float (&c)[3])
{
    d_[Dim::X] = x;
    d_[Dim::Y] = y;
    center_[Dim::X] = c[Dim::X];
    center_[Dim::Y] = c[Dim::Y];
    center_[Dim::Z] = c[Dim::Z];
    id_ = rand();

}

VirtualImage::VirtualImage(const int x, const int y)
{
    d_[Dim::X] = x;
    d_[Dim::Y] = y;
    center_[Dim::X] = static_cast<float>(x)/2;
    center_[Dim::Y] = static_cast<float>(y)/2;
    center_[Dim::Z] = 0;
    id_ = rand();

}

int VirtualImage::dim(unsigned int index) const
{
    return d_[ index ];
}

int VirtualImage::id() const
{
    return id_;
}

float (&VirtualImage::center()) [3]
{
    return center_;
}

int (&VirtualImage::dim()) [2]
{
    return d_;
}

