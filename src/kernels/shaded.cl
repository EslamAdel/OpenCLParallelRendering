/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */



// intersect ray with a box
// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

int intersectBox(float4 r_o, float4 r_d, float4 boxmin, float4 boxmax, float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    float4 invR = (float4)(1.0f,1.0f,1.0f,1.0f) / r_d;
    float4 tbot = invR * (boxmin - r_o);
    float4 ttop = invR * (boxmax - r_o);

    // re-order intersections to find smallest and largest on each axis
    float4 tmin = min(ttop, tbot);
    float4 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));

    *tnear = largest_tmin;
    *tfar = smallest_tmax;

    return smallest_tmax > largest_tmin;
}

uint rgbaFloatToInt(float4 rgba)
{
    rgba.x = clamp(rgba.x,0.0f,1.0f);
    rgba.y = clamp(rgba.y,0.0f,1.0f);
    rgba.z = clamp(rgba.z,0.0f,1.0f);
    rgba.w = clamp(rgba.w,0.0f,1.0f);
    return ((uint)(rgba.w*255.0f)<<24) | ((uint)(rgba.z*255.0f)<<16) | ((uint)(rgba.y*255.0f)<<8) | (uint)(rgba.x*255.0f);
}


__kernel void
shaded( __write_only image2d_t frameBuffer,

        uint frameWidth, uint frameHeight,

        uint sortFirstWidth , uint sortFirstHeight ,

        __constant  float* invViewMatrix,

        __read_only image3d_t volume,

        sampler_t   volumeSampler ,

        float density, float brightness ,

        uint maxSteps, float tStep ,

        __read_only image2d_t transferFunc,

        sampler_t transferFuncSampler )

{	


    const uint x = get_global_id( 0 );
    const uint y = get_global_id( 1 );

    const uint offsetX = get_global_offset( 0 );
    const uint offsetY = get_global_offset( 1 );

    // If out of boundaries, return.
    if( x - offsetX  >= sortFirstWidth   )
        return ;

    if( y - offsetY >= sortFirstHeight  )
        return ;

    float u = (x / (float) frameWidth)*2.0f-1.0f;
    float v = (y / (float) frameHeight)*2.0f-1.0f;


    float4 boxMin = (float4)(-1.f, -1.f, -1.f,1.0f);
    float4 boxMax = (float4)(1.0f, 1.f, 1.0f ,1.0f);

    float4 volScale = (float4)(1.f, 1.f, 0.125f, 1.f);
    float4 texScale = (float4)(0.125f, 0.125f, 1.f, 1.f);

    // float4 boxMin = (float4)(-1.0f * volScale.x, -1.0f * volScale.y, -1.0f * volScale.z, 1.0f);
    // float4 boxMax = (float4)(1.0f * volScale.x, 1.0f * volScale.y, 1.0f * volScale.z, 1.0f);

    // calculate eye ray in world space
    float4 eyeRay_o;
    float4 eyeRay_d;

    eyeRay_o = (float4)(invViewMatrix[3], invViewMatrix[7], invViewMatrix[11], 1.0f);

    float4 temp = normalize(((float4)(u, v, -2.0f,0.0f)));
    eyeRay_d.x = dot(temp, ((float4)(invViewMatrix[0],invViewMatrix[1],invViewMatrix[2],invViewMatrix[3])));
    eyeRay_d.y = dot(temp, ((float4)(invViewMatrix[4],invViewMatrix[5],invViewMatrix[6],invViewMatrix[7])));
    eyeRay_d.z = dot(temp, ((float4)(invViewMatrix[8],invViewMatrix[9],invViewMatrix[10],invViewMatrix[11])));
    eyeRay_d.w = 0.0f;

    // find intersection with box
    float tnear, tfar;
    int hit = intersectBox(eyeRay_o, eyeRay_d, boxMin, boxMax, &tnear, &tfar);

    // If it doesn't hit, then return a black value in the corresponding pixel
    if( !hit )
    {
        // Get the 1D index of the pixel to set its color, and return
        const float4 nullPixel = ( float4 )( 0.f , 0.f , 0.f , 0.f );
        const int2 location = (int2)( x , y  );
        write_imagef( frameBuffer , location , nullPixel );

        return;
    }

    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane

    // march along ray from back to front, accumulating color
    temp = (float4)(0.0f,0.0f,0.0f,0.0f);
    float t = tfar;

    for(uint i=0; i<maxSteps; i++) {
        float4 pos = eyeRay_o + eyeRay_d*t;

        pos = (pos*0.5f) +0.5f;    // map position to [0, 1] coordinates

        // read from 3D texture
#ifdef IMAGE_SUPPORT        
        float4 sample = read_imagef(volume, volumeSampler, pos);
        
        // lookup in transfer function texture
        float2 transfer_pos = (float2)(sample.x, 1.f);
        float4 col = read_imagef(transferFunc, transferFuncSampler, transfer_pos);
#else
        float4 col = (float4)(pos.x,pos.y,pos.z,.25f);
#endif


        // accumulate result
        float a = col.w*density;
        temp = mix(temp, col, (float4)(a, a, a, a));

        t -= tStep;
        if (t < tnear) break;
    }
    temp *= brightness;

    // Get a 1D index of the pixel in the _frameBuffer_
    const int2 location = (int2)( x , y  );
    write_imagef( frameBuffer , location , temp );
}

