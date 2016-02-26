
/**
 * @brief rgbaFloatToInt
 * Converts a float4 with RGBA components into a 32-bit unsigned integer, at
 * which each byte represents a color component. This step is required to
 * optimize the way the final color array that represents the image is created.
 * It basically converts a 4 x 32-bit float vector into a single 32-bit value.
 * @param rgba The color vector in RGBA fashion.
 * @return A 32-bit integer representing the color.
 * @note Use the swizzeling operator to access the individual color components
 * from the rgba vector and use shift operation to sort them in the unsigned int.
 */
uint rgbaFloatToInt( float4 rgba )
{
    rgba.x = clamp( rgba.x, 0.f, 1.f);
    rgba.y = clamp( rgba.y, 0.f, 1.f);
    rgba.z = clamp( rgba.z, 0.f, 1.f);
    rgba.w = clamp( rgba.w, 0.f, 1.f);
    return (( uint )( rgba.w * 255.f ) << 24 ) |     // Alpha
           (( uint )( rgba.z * 255.f ) << 16 ) |     // Blue
           (( uint )( rgba.y * 255.f ) << 8  ) |     // Green
            ( uint )( rgba.x * 255.f );              // Red
}

float4 convertColorToRGBAF( uint Color )
{
    float4 rgba ;
    rgba.x = ( Color & 0xFF ) / 255.f ; Color >>= 8;
    rgba.y = ( Color & 0xFF ) / 255.f ; Color >>= 8;
    rgba.z = ( Color & 0xFF ) / 255.f ; Color >>= 8;
    rgba.w = ( Color & 0xFF ) / 255.f ;



    return rgba ;
}

__kernel
void xray_compositing_accumulate(   __global uint* collageFrame ,
                                    __global uint* frame )
{
    const uint index = get_global_id(0);

    float4 frameRGBA = convertColorToRGBAF( frame[ index ]  );

    float4 collageRGBA = convertColorToRGBAF( collageFrame[ index ] );


    float4 finalColor;

    finalColor = clamp(  collageRGBA + frameRGBA  , 0.f , 1.f );

    collageFrame[ index ] = rgbaFloatToInt( finalColor ) ;
}



const sampler_t sampler =  CLK_FILTER_NEAREST | CLK_NORMALIZED_COORDS_TRUE
                            | CLK_ADDRESS_CLAMP_TO_EDGE ;

__kernel
void xray_compositing_patch( __write_only image2d_t collageFrame ,
                             __read_only image3d_t framesArray )
{

    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint framesCount = get_image_depth( framesArray );





    float4 color = (float4)( 0.f , 0.f , 0.f , 0.f );

    for( int i = 0 ; i < framesCount - 2  ; i++ )
    {
        int4 location = (int4)( x , y , i , 0 );
        float4 sample = read_imagef( framesArray , sampler , location ) ;
        //color = mix( color, sample , sample.w );
        color = clamp( color + sample , 0.f , 1.f );
    }
    int2 locate = (int2)( x , y );


    write_imagef( collageFrame , locate , color ) ;
}



