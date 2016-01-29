


__kernel void XRayAccumulate( image2d_t baseImage ,
                              __read_only image2d_t stackedImage)
{
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);
    int2 locate;
    locate.x = x;
    locate.y = y;
    uint4 colorBase   ;
    uint4 colorStacked;

    const sampler_t sampler = 0;

    colorBase    = read_imageui( baseImage    , sampler , locate );
    colorStacked = read_imageui( stackedImage , sampler , locate );

    write_imageui( baseImage , locate, colorBase + colorStacked );

}

