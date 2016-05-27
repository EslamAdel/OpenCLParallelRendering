#ifndef CLRenderer_H
#define CLRenderer_H

#include "CLAbstractRenderer.h"
#include "Volume.h"
#include "Transformation.h"
#include "CLData.hh"
#include <QMap>


namespace clparen {
namespace Renderer {


template < class V , class F >
class CLRenderer : public CLAbstractRenderer
{
public:
    /**
     * @brief CLRenderer
     * @param gpuIndex
     * @param frameWidth
     * @param frameHeight
     * @param transformation
     * @param kernelDirectory
     */
    CLRenderer(
            const uint64_t gpuIndex,
            const Transformation &transformation ,
            const Dimensions2D frameDimensions ,
            const std::string kernelDirectory = DEFAULT_KERNELS_DIRECTORY );


    ~CLRenderer();

    /**
     * @brief loadVolume
     * @param volume
     */
    void loadVolume( VolumeVariant &volume ) override ;

    /**
     * @brief applyTransformation
     */
    void applyTransformation( ) override ;

    /**
     * @brief setFrameIndex
     * @param frameIndex
     */
    void setFrameIndex( const uint frameIndex  ) override;

    /**
     * @brief getCurrentCenter
     * @return
     */
    const Coordinates3D &getCurrentCenter() const override ;

    /**
     * @brief getFrameIndex
     * @return
     */
    uint getFrameIndex( ) const override ;

    /**
     * @brief getCLFrame
     * @return
     */
    const CLData::CLFrameVariant &getCLFrame( ) const override;

    /**
     * @brief getCLVolume
     * @return
     */
    CLData::CLVolume< V > *getCLVolume();

    /**
     * @brief reloadVolume
     */
    void reloadVolume( const cl_bool blocking );


    /**
     * @brief copyHostData
     * @param brickParameters
     */
    void copyHostData( const BrickParameters< V > &brickParameters );



protected:

    /**
     * @brief renderFrame
     */
    void renderFrame() override ;

    /**
     * @brief createPixelBuffer_
     */
    void createPixelBuffer_() override ;

    /**
     * @brief initializeKernels_
     */
    void initializeKernels_() override ;

    /**
     * @brief freeBuffers_
     */
    void freeBuffers_() override ;

private:

    /**
     * @brief paint_
     */
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
    Volume< V >* volume_;

    /**
     * @brief clVolume_
     */
    CLData::CLVolume< V >* clVolume_;

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
    CLData::CLImage2D< F > *clFrame_ ;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];
};



}
}

#endif // CLRenderer_H
