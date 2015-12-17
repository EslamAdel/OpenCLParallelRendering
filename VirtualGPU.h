#ifndef VIRTUALGPU_H
#define VIRTUALGPU_H
#include <QObject>
#include <VirtualExperiment.h>
#include <VirtualVolume.h>
#include <VirtualImage.h>
#include "Transformation.h"

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

class VirtualGPU : public QObject
{
    Q_OBJECT
public:
    VirtualGPU();

    void loadVolume( VirtualVolume * const volume ) ;
    void freeVolume();

    void renderVolume();
    void applyTransformation( Transformation *transformation );
    void compositeImages( QList<VirtualImage *> *images ) ;

    // nano-seconds
    double volumeRenderingTime( VirtualVolume &volume_) const;
    double imagesCompositingTime( QList<VirtualImage*> *images ) const;

    VirtualImage* resultantImage() const ;

    bool volumeExists() const;
signals :
    void finishedRendering();
    void finishedCompositing();

private:

    double volumeProcessingTime_(VirtualVolume &volume_) const;
    double imageProcessingTime_( VirtualImage  &image) const;

    bool volumeExists_;
    GPUParameters parameters_;

    VirtualVolume *volume_;
    VirtualImage *resultantImage_;
};

#endif // VIRTUALGPU_H
