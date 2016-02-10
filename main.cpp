#include <QApplication>
#include <iostream>
#include "Logger.h"
#include "RenderingWindow.h"
#include "ProfilingExterns.h"






int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    //TODO: Using getopt to parse terminal arguments.


    LOG_INFO( "Loading volume" );
    const std::string prefix = VOLUME_PREFIX;
    Volume< uchar >* volume = new Volume< uchar >( prefix );

    // Virtual GPUs to test the cases of many nodes, more than the actual
    // count of GPUs in device.
#ifdef VIRTUAL_GPUS
    //argument 3 is count of virtual GPUs.
    auto parallelRenderer_ =
            new VirtualParallelRendering( volume , FRAME_WIDTH , FRAME_HEIGHT ,
                                          VIRTUAL_GPUS );
#else
    auto parallelRenderer_ =
            new ParallelRendering( volume , FRAME_WIDTH , FRAME_HEIGHT);
#endif


#ifdef PROFILE_SINGLE_GPU

    parallelRenderer_->addRenderingNode( DEPLOY_GPU_INDEX );
#else

    for( auto i = 0 ; i < parallelRenderer_->getMachineGPUsCount()  ; i++ )
    {
        LOG_DEBUG( "Deploy GPU#%d" , i );
        parallelRenderer_->addRenderingNode( i );
    }

#endif

    LOG_DEBUG( "Distribute Volume" );
    parallelRenderer_->distributeBaseVolume1D( );

    LOG_DEBUG("Add Compositing Node");
    //The Compositing GPU will be the RenderingGPU with index 0 .
    parallelRenderer_->addCompositingNode( 0 );


    RenderingWindow rw( parallelRenderer_ );
    rw.show();

    return a.exec();
}



//if( i < frameContainers_.size() )
//{
//    frameContainers_[ i ]->setEnabled( true );
//    labels[ i ]->setText( QString( labels[ i ]->text( ))
//                          + QString( "(active)" ));
//}
