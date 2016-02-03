

//__kernel
//void xray_compositing(  __global image2d_t collageFrame ,
//                        uint collageWidth ,
//                        uint collageHeight ,
//                        __global image2d_t frame,
//                        uint frameWidth ,
//                        uint frameHeight ,
//                        float frameCenterX ,
//                        float frameCenterY  )
//{

//    const uint x = get_global_id(0);
//    const uint y = get_global_id(1);

//    float2 collageLocate ;
//    collageLocate.x = x + frameCenterX - frameWidth  / 2.f ;
//    collageLocate.y = y + frameCenterY - frameHeight / 2.f ;

//    float2 framelocate;
//    frameLocate.x = x;
//    frameLocate.y = y;

//    uint4 collageColor ;
//    uint4 frameColor  ;

//    const sampler_t sampler = 0;

//    collageColor = read_imageui( collageFrame , sampler , collageLocate );
//    frameColor  = read_imageui( frame  , sampler , frameLocate );

//    int2 collageLocate_int ;
//    collageLocate_int.x = round( collageLocate.x );
//    collageLocate_int.y = round( collageLocate.y );

//    write_imageui( collageFrame , collageLocate_int , collageColor + frameColor );


//}


__kernel
void xray_compositing(  __global uint* collageFrame ,
                        __global uint* frame )
{
    const uint index = get_global_id(0);

    collageFrame[ index ] += frame[ index ];
}


__kernel
void rewind_buffer(  __global uint* frame )
{
    const uint index = get_global_id(0);
    frame[ index ] = 0 ;
}


//__kernel
//void rewind_image2d(  __global image2d_t frame ,
//                        uint width ,
//                        uint height )
//{

//    const uint x = get_global_id(0);
//    const uint y = get_global_id(1);

//    int2 locate ;
//    locate.x = x ;
//    locate.y = y ;

//    if( x < width && y < height )
//        write_imageui( frame , locate , 0 );

//}
