#ifndef VIRTUALGPU_H
#define VIRTUALGPU_H
#include <VirtualExperiment.h>
#include <VirtualVolume.h>
#include <VirtualImage.h>


//GPU parameters: some heuristic quasai-linear equation to mimic the gpu processing time
// a*(xyz) + b*(xy) + c*(xz) + d*(zy) + e*(x) + f*(y) + g*(z)
struct GPUParameters
{
    GPUParameters();
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    double g;
};

class VirtualGPU
{
    Q_OBJECT
public:
    VirtualGPU();

    void renderVolume( VirtualVolume &volume );
    void compositeImages( QList<VirtualImage *> *images ) ;

    // nano-seconds
    double volumeRenderingTime( VirtualVolume &volume_) ;
    double imagesCompositingTime( QList<VirtualImage*> *images ) ;

    VirtualImage* resultantImage() const ;

signals :
    void finishedRendering();
    void finishedCompositing();

private:

    double volumeProcessingTime_(VirtualVolume &volume_);
    double imageProcessingTime_( VirtualImage  &image);


    GPUParameters parameters_;
    VirtualImage *resultantImage_;
};

#endif // VIRTUALGPU_H
