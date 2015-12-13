#ifndef VIRTUALNODE_H
#define VIRTUALNODE_H
#include "VirtualGPU.h"
#include "VirtualVolume.h"


class VirtualNode
{
public:
    VirtualNode();

    void setVolume( const VirtualVolume &volume );

    bool volumeExist() const ;

private :
    bool volumeExist_;
    VirtualGPU vGPU_;
    VirtualVolume subVolume_ ; //sort last

};

#endif // VIRTUALNODE_H
