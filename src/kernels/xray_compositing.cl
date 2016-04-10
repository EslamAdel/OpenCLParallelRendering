
__kernel
void xray_compositing_accumulate(   __global float* finalFrame ,
                                    __global float* frame )
{
    const uint index = get_global_id(0);

    const float
    finalColor = clamp(  frame[ index ] + finalFrame[ index ]  , 0.f , 1.f );


    finalFrame[ index ] = finalColor ;
}



const sampler_t sampler =  CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_FALSE
                            | CLK_ADDRESS_CLAMP_TO_EDGE;

__kernel
void xray_compositing_patch( __write_only image2d_t collageFrame ,
                             __read_only image3d_t framesArray ,
                             __constant  uint* depthIndex )
{
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint framesCount = get_image_depth( framesArray );

    float4 color = (float4)( 0.f , 0.f , 0.f , 0.f );

    for( int i = 0 ; i < framesCount ; i++ )
    {
        const uint currentDepth = depthIndex[ i ];
        const int4 location = (int4)( x , y , currentDepth , 0 );
        const float4 sample = read_imagef( framesArray , sampler , location ) ;
        color = mix( color , sample  ,  sample.w   );
    }
    const int2 locate = (int2)( x , y );

    write_imagef( collageFrame , locate , color ) ;
}



