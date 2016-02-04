
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
    rgba.z = ( Color & 0xFF ) / 255.f ; Color >>= 8;
    rgba.y = ( Color & 0xFF ) / 255.f ; Color >>= 8;
    rgba.x = ( Color & 0xFF ) / 255.f ; Color >>= 8;
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

    float4 finalColor = clamp( frameRGBA + collageRGBA , 0.f , 1.f ) ;

    collageFrame[ index ] = rgbaFloatToInt( finalColor ) ;
}


__kernel
void xray_compositing_patch( __global uint* collageFrame ,
                             __read_only image3d_t framesArray )
{

    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint imageWidth = get_image_width( framesArray );
    const uint framesCount = get_image_depth( framesArray );

    int4 locate = (int4)( x , y , 0 , 1 );


    float4 color = (float4)( 0.f , 0.f , 0.f , 0.f );

    uint index = 0 ;
    for( index = 0 ; index < framesCount ; index++ )
    {
        locate.z = index ;
        color = clamp( color + read_imagef( framesArray , locate ) , 0.f , 1.f ) ;
    }

    index = imageWidth * y + x ;
    collageFrame[ index ] =  rgbaFloatToInt( color );
}



__kernel
void rewind_buffer(  __global uint* frame )
{
const uint index = get_global_id(0);
frame[ index ] = 0 ;

}


