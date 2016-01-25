#include "VirtualGPU.h"
#include <stdlib.h>
#include <QThread>
#include <iostream>


VirtualGPU::VirtualGPU()
{
    std::cout<<"\tVirtual GPU created!"<<std::endl;
}

void VirtualGPU::loadVolume( VirtualVolume * const volume)
{
    volume_ = volume;
    volumeExists_ = true;
}

void VirtualGPU::freeVolume()
{
    volume_ = nullptr;
    volumeExists_ = false;
}

void VirtualGPU::renderVolume()
{
    if( volumeExists() == false )
    {
        std::cout << "No volume exists!\n";
        return;
    }

    // convert from nano to micro
    unsigned long processingTimeN = volumeRenderingTime( *volume_ ) ;
    unsigned long processingTimeU = processingTimeN/1000 ;

    QThread::currentThread()->usleep( processingTimeU );

    resultantImage_ = new VirtualImage( volume_->dim()[0],
                                        volume_->dim()[1],
                                        volume_->center() );

    emit this->finishedRendering();
}

void VirtualGPU::applyTransformation( const Transformation *transformation)
{
    renderVolume();
}

void VirtualGPU::compositeImages(const std::vector<const VirtualImage *> &images) const
{
    if( images.empty() ) return;

    unsigned long processingTimeN = imagesCompositingTime( images ) ;
    unsigned long processingTimeU = processingTimeN/1000 ;

    QThread::currentThread()->usleep( processingTimeU );

    resultantImage_ = new VirtualImage( images.front()->dim(0) * (int)images.size() ,
                                        images.front()->dim(1) );

    emit this->finishedCompositing();
}

double VirtualGPU::volumeRenderingTime( VirtualVolume &volume_ ) const
{
    return  volumeProcessingTime_( volume_ ) *
            VirtualExperiment::processScale( VirtualExperiment::ProcessOrder::Rendering );
}

double VirtualGPU::imagesCompositingTime( const std::vector<const VirtualImage *> &images) const
{
    if ( images.empty() ) return 0 ;
    double totalTime = 0 ;
    double compositingScale = VirtualExperiment::processScale( VirtualExperiment::ProcessOrder::Compositing );
    for( auto &image : images )
    {
        totalTime += imageProcessingTime_( *image ) ;
    }
    return totalTime * compositingScale;
}

VirtualImage *VirtualGPU::resultantImage() const
{
    return resultantImage_ ;
}

bool VirtualGPU::volumeExists() const
{
    return volumeExists_;
}

double VirtualGPU::volumeProcessingTime_( VirtualVolume &volume_ ) const
{
    //non-linear
    return  parameters_.a * volume_.dim()[0] * volume_.dim()[1] * volume_.dim()[2] +
            parameters_.b * volume_.dim()[0] * volume_.dim()[1] +
            parameters_.c * volume_.dim()[0] * volume_.dim()[2] +
            parameters_.d * volume_.dim()[1] * volume_.dim()[2] +
            parameters_.e * volume_.dim()[0] +
            parameters_.f * volume_.dim()[1] +
            parameters_.g * volume_.dim()[2] ;
}

double VirtualGPU::imageProcessingTime_( const VirtualImage &image) const
{
    return parameters_.a * image.dim(0) * image.dim(1) +
           parameters_.b * image.dim(0) +
           parameters_.c * image.dim(1) ;
}



GPUParameters::GPUParameters()
{
    // a*(xyz) OR a*volume ... highest order. Should be with highest correlation with time

    a = drand48();

    // b,c,d ... 2nd order.
    b= drand48()/2;
    c= drand48()/2;
    d= drand48()/2;

    // e,f,g ... 1st order
    e= drand48()/4;
    f= drand48()/4;
    g= drand48()/4;

}
