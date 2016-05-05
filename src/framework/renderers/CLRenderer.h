#ifndef CLRenderer_H
#define CLRenderer_H

#include "CLAbstractRenderer.h"
#include "Volume.h"
#include "Transformation.h"
#include <CLVolume.h>
#include "CLTransferFunction.h"
#include <QMap>

template < class V , class F >
class CLRenderer : public CLAbstractRenderer
{
public:
    CLRenderer( const uint64_t gpuIndex,
                const uint frameWidth , const uint frameHeight ,
                const Transformation &transformation ,
                const std::string kernelDirectory = "." );

    ~CLRenderer();

    void loadVolume( const VolumeVariant &volume ) override ;

    void applyTransformation( ) override ;

    void setFrameIndex( const uint frameIndex  ) override;

    const Coordinates3D &getCurrentCenter() const override ;

    uint getFrameIndex( ) const override ;

    const CLFrameVariant &getCLFrame( ) const override;

protected:

    void renderFrame() override ;

    void createPixelBuffer_() override ;

    void initializeKernels_() override ;

    void freeBuffers_() override ;

private:

    void paint_( );



private:

    /**
     * @brief transformation_
     */
    const Transformation &transformation_ ;

    /**
     * @brief currentCenter_
     */
    Coordinates3D currentCenter_ ;

    /**
     * @brief frameIndex_
     */
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
     * @brief inverseMatrix_
     */
    cl_mem inverseMatrix_;


    /**
     * @brief linearVolumeSampler_
     */
    cl_sampler linearVolumeSampler_;

    /**
     * @brief nearestVolumeSampler_
     */
    cl_sampler nearestVolumeSampler_;

    /**
     * @brief linearFiltering_
     */
    bool linearFiltering_;

    /**
     * @brief gridSize_
     */
    size_t gridSize_[ 2 ];


    /**
     * @brief clFrame_
     */
    CLImage2D< F > *clFrame_ ;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];
};



#endif // CLRenderer_H
