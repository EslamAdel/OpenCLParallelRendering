


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
