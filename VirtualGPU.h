#ifndef VIRTUALGPU_H
#define VIRTUALGPU_H
#include <QObject>
#include "VirtualExperiment.h"
#include "VirtualVolume.h"
#include "VirtualImage.h"
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
    void applyTransformation( const Transformation *transformation );
    void compositeImages( const std::vector<const VirtualImage *> &images ) const ;

    // nano-seconds
    double volumeRenderingTime( VirtualVolume &volume_) const;
    double imagesCompositingTime( const std::vector<const VirtualImage*> &images ) const;

    VirtualImage* resultantImage() const ;

    bool volumeExists() const;
signals :
    void finishedRendering() const;
    void finishedCompositing() const;

private:

    double volumeProcessingTime_( VirtualVolume &volume_) const;
    double imageProcessingTime_( const VirtualImage  &image) const;

    bool volumeExists_;
    GPUParameters parameters_;

    VirtualVolume *volume_;
    mutable VirtualImage *resultantImage_;
};

#endif // VIRTUALGPU_H
