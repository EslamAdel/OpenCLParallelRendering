#ifndef CLRenderer_H
#define CLRenderer_H

#include "CLAbstractRenderer.h"
#include "Volume.h"
#include "Transformation.h"
#include <CLVolume.h>
#include <CLXRayRenderingKernel.h>


template < class V , class F >
class CLRenderer : public CLAbstractRenderer
{
public:
    CLRenderer( const uint64_t gpuIndex,
                const uint frameWidth , const uint frameHeight ,
                const Transformation &transformation );

    ~CLRenderer();

    void loadVolume( const VolumeVariant &volume ) override ;

    void applyTransformation( ) override ;

    void setFrameIndex( const uint frameIndex  ) override;

    const Coordinates3D &getCurrentCenter() const override ;

    uint getFrameIndex( ) const override ;

    /**
      * @brief getKernel
      * @return
      */
    cl_kernel getKernel( ) const override;


    CLFrameVariant &getCLFrame( ) override;

protected:

    void renderFrame() override ;

    void createPixelBuffer_() override ;

    void initializeKernel_() override ;

    void handleKernel_( std::string string = "" ) override;

    void freeBuffers_() override ;

private:

    void paint( );


private:

    const Transformation &transformation_ ;
    /**
     * @brief currentCenter_
     */
    Coordinates3D currentCenter_ ;

    uint frameIndex_ ;


    /**
     * @brief volume_
     */
    const Volume< V >* volume_;

    /**
     * @brief clVolume_
     */
    CLVolume< V >* clVolume_;


    /**
     * @brief kernelContext_
     */
    oclHWDL::KernelContext* kernelContext_;
    /**
     * @brief kernel_
     */
    cl_kernel kernel_;

    /**
     * @brief inverseMatrix_
     */
    cl_mem inverseMatrix_;

    /**
     * @brief transferFunctionArray_
     */
    cl_mem transferFunctionArray_;

    /**
     * @brief linearVolumeSampler_
     */
    cl_sampler linearVolumeSampler_;

    /**
     * @brief nearestVolumeSampler_
     */
    cl_sampler nearestVolumeSampler_;

    /**
     * @brief transferFunctionSampler_
     */
    cl_sampler transferFunctionSampler_;

    /**
     * @brief tfOffset_
     */
    float tfOffset_;

    /**
     * @brief tfScale_
     */
    float tfScale_;

    /**
     * @brief linearFiltering_
     */
    bool linearFiltering_;

    /**
     * @brief gridSize_
     */
    size_t gridSize_[ 2 ];

    /**
     * @brief renderingKernels_
     */
    std::vector< CLRenderingKernel* > renderingKernels_;

    /**
     * @brief activeRenderingKernel_
     */
    CLRenderingKernel* activeRenderingKernel_;

    /**
     * @brief clFrame_
     */
    CLFrame< F > *clFrame_ ;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];


};



#endif // CLRenderer_H
