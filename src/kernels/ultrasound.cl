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


#define M_PI_2DEC 3.14

float d2r(float angle)
{
    return(angle*M_PI_2DEC/180.0);
}

// Intersects a ray with a triangle defined by three vertices.
// If intersecting, t is the point of intersection along the ray
bool rayTriangleIntersection(float3 o, float3 dir, float3 v0, float3 v1, float3 v2, float* t) {
    float3 e1 = v1-v0;
    float3 e2 = v2-v0;
    float3 p = cross(dir, e2);
    float a = dot(e1, p);
    if (a > -0.00001f && a < 0.00001f) {
        return false;
    }
    float f = 1.f/a;
    // Distance from v0 to ray origin
    float3 s = o-v0;
    // u parameter
    float u = f * dot(s, p);
    if (u < 0.f || u > 1.f) {
        return false;
    }
    
    float3 q = cross(s, e1);
    float v = f * dot(dir, q);
    
    if (v < 0.f || u+v > 1.f) {
        return false;
    }
    // Compute the intersection point t
    
    *t = f * dot(e2, q);
    
    if (*t > 0.00001f) {
        return true;
    } else {
        return false;
    }
}

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



// intersection with the pyramidal grid
int intersectPyramidalGrid(float4 r_o, float4 r_d, float apex,
                           float *tnear, float *tfar,
                           float xScale, float yScale, float zScale)
{
    /*
    float theta = d2r(apex);
    float tanTheta = tan(theta);
    float yApex = (xScale / tanTheta) - yScale;

    float3 pApex = (float3) (0.0, yApex, 0.0);
    float3 p1 = (float3) (-xScale, -yScale, -zScale);
    float3 p2 = (float3) (-xScale, -yScale, zScale);
    float3 p3 = (float3) (xScale, -yScale, zScale);
    float3 p4 = (float3) (xScale, -yScale, -zScale);
*/

    float theta = d2r(apex);
    float tanTheta = tan(theta);
    float yApex = -((1 / tanTheta) - 1);

    float3 pApex = (float3) (0.0, yApex, 0.0);
    float3 p1 = (float3) (-1, 1.0, -1);
    float3 p2 = (float3) (-1, 1.0, 1);
    float3 p3 = (float3) (1, 1.0, 1);
    float3 p4 = (float3) (1, 1.0, -1);

    float3 origin = (float3) (r_o.x, r_o.y, r_o.z);
    float3 direction = (float3) (r_d.x, r_d.y, r_d.z);

    float tMax = 0;
    float tMin = 0;
    float t = 0.0;

    bool firstHit = false;
    bool secondHit = false;

    if(rayTriangleIntersection(origin, direction, pApex, p1, p2, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    if(rayTriangleIntersection(origin, direction, pApex, p2, p3, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    if(rayTriangleIntersection(origin, direction, pApex, p3, p4, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    if(rayTriangleIntersection(origin, direction, pApex, p4, p1, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    if(rayTriangleIntersection(origin, direction, p1, p4, p3, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    if(rayTriangleIntersection(origin, direction, p1, p2, p3, &t))
    {
        if(!firstHit)
        {
            firstHit = true;
            tMax = t;
            tMin = t;
        }
        else
        {
            secondHit = true;
            if(t > tMax) tMax = t;
            if(t < tMin) tMin = t;
        }
    }

    *tnear = min(tMin, tMax);
    *tfar = max(tMin, tMax);

    return secondHit;

}


float4 mapPosition(float4 position, float apex,
                   float xxxScale, float yyyScale, float zzzScale) {
    float theta = d2r(apex);
    float tanTheta = tan(theta);
    float yApex = -((1 / tanTheta) - 1);

    float Xc = 1.0;
    float Zc = 1.0;
    float r = fabs(yApex - position.y);
    float Xp = r * tanTheta;
    float Zp = r * tanTheta;
    float xScale = Xc / Xp;
    float zScale = Zc / Zp;

    float4 pos = (float4)(0.0, 0.0, 0.0, 0.0);
    pos.x = position.x * xScale;
    pos.y = position.y;
    pos.z = position.z * zScale;
    pos.w = position.w;
    return pos;
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
ultrasound( __write_only image2d_t frameBuffer,

            uint frameWidth, uint frameHeight,

            uint sortFirstWidth , uint sortFirstHeight ,

            __constant  float* invViewMatrix,

            __read_only image3d_t volume,

            sampler_t   volumeSampler ,

            float density, float brightness ,

            uint maxSteps, float tStep ,

            __read_only image2d_t transferFunc,

            sampler_t transferFuncSampler, float apex,

            float xScale, float yScale, float zScale)

{
    const uint x = get_global_id( 0 );
    const uint y = get_global_id( 1 );

    const uint offsetX = get_global_offset( 0 );
    const uint offsetY = get_global_offset( 1 );

    // If out of boundaries, return.
    if( x - offsetX  >= sortFirstWidth   )
        return ;

    if( y - offsetY  >= sortFirstHeight  )
        return ;
    
    float u = (x / (float) frameWidth)*2.0f-1.0f;
    float v = (y / (float) frameHeight)*2.0f-1.0f;
    
    //float tstep = 0.01f;
    float4 boxMin = (float4)(-1.0f, -1.0f, -1.0f,1.0f);
    float4 boxMax = (float4)(1.0f, 1.0f, 1.0f,1.0f);

    // float4 boxMin = (float4)(-1.0f * xScale, -1.0f * yScale, -1.0f * zScale,1.0f);
    // float4 boxMax = (float4)(1.0f * xScale, 1.0f * yScale, 1.0f * zScale,1.0f);
    
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
    // int hit = intersectBox(eyeRay_o, eyeRay_d, boxMin, boxMax, &tnear, &tfar);
    int hit = intersectPyramidalGrid(eyeRay_o, eyeRay_d, apex, &tnear, &tfar, xScale, yScale, zScale);
    // If it doesn't hit, then return a black value in the corresponding pixel
    if( !hit )
    {
        // Get the 1D index of the pixel to set its color, and return
        const float4 nullPixel = ( float4 )( 0.f , 0.f , 0.f , 0.f );
        const int2 location = (int2)( x - offsetX , y - offsetY );
        write_imagef( frameBuffer , location , nullPixel );

        return;
    }

    if (tnear < 0.0f) tnear = 0.0f;     // clamp to near plane
    
    
    // march along ray from back to front, accumulating color
    temp = (float4)(0.0f,0.0f,0.0f,0.0f);
    float t = tfar;
    
    for(uint i=0; i<maxSteps; i++) {
        float4 pos = eyeRay_o + eyeRay_d*t;
        float4 mappedPos = mapPosition(pos, apex, xScale, yScale, zScale);

        // float4 inverseScale = (float4)(1.f/xScale, 1.f/yScale, 1.f/zScale, 0.f);
        // float4 scale = (float4)(xScale, yScale, zScale, 0.f);
        // pos = (pos* 0.5f * scale) +0.5f;    // map position to [0, 1] coordinates
        mappedPos = (mappedPos* 0.5f) +0.5f;


        
        // read from 3D texture
#ifdef IMAGE_SUPPORT
        float4 sample;
        if(pos.y > -1.0)
        {
            sample = read_imagef(volume, volumeSampler, mappedPos);
        }
        else
            sample = (float4)(0.0, 0.0, 0.0, 0.0);
        
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
    const int2 location = (int2)( x - offsetX , y - offsetY );
    write_imagef( frameBuffer , location , temp );
}



/*
float4 mapPosition(float4 position, float apex,
                   float xScale, float yScale, float zScale) {
    float theta = d2r(apex);
    float tanTheta = tan(theta);
    float yApex = (xScale / tanTheta) - yScale;

    float Xc = 1.f;
    float Zc = 1.f;

    float r = fabs(yApex - position.y);
    float Xp = r * tanTheta;
    float Zp = r * tanTheta;
    float xxScale = Xc / Xp;
    float zzScale = Zc / Zp;

    float4 pos = (float4)(0.0, 0.0, 0.0, 0.0);
    pos.x = position.x * xxScale;
    pos.y = position.y;
    pos.z = position.z * zzScale;
    pos.w = position.w;
    return pos;
}
*/
