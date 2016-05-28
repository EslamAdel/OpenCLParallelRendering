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
            const CLData::FRAME_CHANNEL_ORDER channelOrder =
            CLData::FRAME_CHANNEL_ORDER::ORDER_DEFAULT,
            const std::string kernelDirectory = DEFAULT_KERNELS_DIRECTORY );


    ~CLRenderer();

    /**
     * @brief loadVolume
     * @param volume
     */
    void loadVolume( VolumeVariant &volume ) Q_DECL_OVERRIDE ;

    /**
     * @brief applyTransformation
     */
    void applyTransformation( ) Q_DECL_OVERRIDE ;

    /**
     * @brief setFrameIndex
     * @param frameIndex
     */
    void setFrameIndex( const uint frameIndex  ) Q_DECL_OVERRIDE;

    /**
     * @brief getCurrentCenter
     * @return
     */
    const Coordinates3D &getCurrentCenter() const Q_DECL_OVERRIDE ;

    /**
     * @brief getFrameIndex
     * @return
     */
    uint getFrameIndex( ) const Q_DECL_OVERRIDE ;

    /**
     * @brief getCLFrame Returns the pixel buffer being rendered to.
     * @return
     */
    const CLData::CLFrameVariant &getCLFrame( ) const Q_DECL_OVERRIDE;

    /**
     * @brief getCLVolume
     * @return
     */
    CLData::CLVolume< V > *getCLVolume();

    /**
     * @brief reloadVolume If data underlying CLData::CLVolume updated, call this function to load the new buffer at host to the device.
     * @param blocking If set CL_TRUE, then this function will block until data transferred to device.
     */
    void reloadVolume( const cl_bool blocking );



    /**
     * @brief copyHostData Extract volume data from BrickParameters structure to the buffer underlying CLData::CLVolume.
     * @param brickParameters Data to be extracted to the underlying buffer in CLData::CLVolume [in].
     */
    void copyHostData( const BrickParameters< V > &brickParameters );


    bool isRenderingModeSupported( CLKernel::RenderingMode mode ) Q_DECL_OVERRIDE;


    void updateTransferFunction( float *transferFunction ) Q_DECL_OVERRIDE;

protected:

    /**
     * @brief renderFrame
     */
    void renderFrame() Q_DECL_OVERRIDE ;

    /**
     * @brief createPixelBuffer_
     */
    void createPixelBuffer_() Q_DECL_OVERRIDE ;

    /**
     * @brief initializeKernels_
     */
    void initializeKernels_() Q_DECL_OVERRIDE ;

    /**
     * @brief freeBuffers_
     */
    void freeBuffers_() Q_DECL_OVERRIDE ;

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
     * @brief linearFiltering_
     */
    bool linearFiltering_;

    /**
     * @brief gridSize_
     */
    size_t gridSize_[ 2 ];


    /**
     * @brief transferFunction_
     */
    float* transferFunction_;


    /**
     * @brief clFrame_
     */
    CLData::CLImage2D< F > *clFrame_ ;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrixArray_[ 12 ];

    /**
     * @brief frameChannelOrder_
     */
    const CLData::FRAME_CHANNEL_ORDER frameChannelOrder_ ;

};



}
}

#endif // CLRenderer_H
