#include "VirtualImage.h"

VirtualImage::VirtualImage(int (&d)[2], float (&c)[3])
{
    d_[Dim::X] = d[Dim::X];
    d_[Dim::Y] = d[Dim::Y];
    center_[Dim::X] = c[Dim::X];
    center_[Dim::Y] = c[Dim::Y];
    center_[Dim::Z] = c[Dim::Z];
}

VirtualImage::VirtualImage(const int x, const int y, float (&c)[3])
{
    d_[Dim::X] = x;
    d_[Dim::Y] = y;
    center_[Dim::X] = c[Dim::X];
    center_[Dim::Y] = c[Dim::Y];
    center_[Dim::Z] = c[Dim::Z];
}

float (&VirtualImage::center()) [3]
{
    return center_;
}

int (&VirtualImage::dim()) [2]
{
    return d_;
}

