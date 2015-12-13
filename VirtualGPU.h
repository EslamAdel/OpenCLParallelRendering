#ifndef VIRTUALGPU_H
#define VIRTUALGPU_H
#include <VirtualExperiment.h>
#include <VirtualVolume.h>

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
public:
    VirtualGPU();

    double volumeRenderingTime( VirtualVolume &volume_) ;


private:

    double volumeProcessingTime_(VirtualVolume &volume_);

    GPUParameters parameters_;
};

#endif // VIRTUALGPU_H
