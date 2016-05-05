#define MAX_STEPS 500
#define T_STEP 0.01

/**
 * @brief intersectBox
 * A function that checks the intersectin between a ray and a box representing
 * the bounding box of a three-dimensional volume texture .
 * @param rayOrigin The origin of the ray.
 * @param rayDirection The direction of the ray.
 * @param pMin The minimum point of the boundnig box that reflects the left and
 * lower corner.
 * @param pMax The maximum point of the bounding box that reflects the right and
 * upper corner.
 * @param tNear The parametric value of the the near intersection of the ray
 * with the bounding box.
 * @param tFar The parametric value of the the far intersection of the ray
 * with the bounding box.
 * @return True if the ray intersects with the bounding box and false otherwise.
 * @note To understand how does this method work, it is recommended to have a
 * look at the following link
 * http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
 */
int intersectBox( const float4 rayOrigin, const float4 rayDirection,
                  const float4 pMin, const float4 pMax,
                  float* tNear, float* tFar)
{
    // Compute the intersection of the ray with all the six planes of the
    // bounding box.
    const float4 invR = ( float4 )( 1.f, 1.f, 1.f, 1.f) / rayDirection;
    const float4 tPMin = invR * ( pMin - rayOrigin );
    const float4 tPMax = invR * ( pMax - rayOrigin );

    // Re-order the intersections to find smallest and largest on each axis.
    const float4 tMin = min( tPMax, tPMin );
    const float4 tMax = max( tPMax, tPMin );

    // Find the largest < tMin > and the smallest < tMax >.
    const float tMinLargest = max( max( tMin.x, tMin.y ), max( tMin.x, tMin.z ));
    const float tMaxSmallest = min( min( tMax.x, tMax.y ), min( tMax.x, tMax.z ));

    *tNear = tMinLargest;
    *tFar = tMaxSmallest;

    return tMaxSmallest > tMinLargest;
}

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

/**
 * @brief minIntensityProjection
 * @param frameBuffer
 * @param width
 * @param height
 * @param density
 * @param brightness
 * @param invViewMatrix
 * @param volume
 * @param volumeSampler
 */
__kernel void minIntensityProjection( __write_only image2d_t frameBuffer,
                    uint width,
                    uint height,
                    float density,
                    float brightness,
                    __constant  float* invViewMatrix,
                    __read_only image3d_t volume,
                    sampler_t   volumeSampler /**,

                    __constant float transferOffset,
                    float transferScale,
                    __read_only image2d_t transferFunc,
                    sampler_t transferFuncSampler,
                    int enableTranferFunction **/
                    )
{
    const uint x = get_global_id( 0 );
    const uint y = get_global_id( 1 );

    const float u = ( x / ( float ) width ) * 2.f - 1.f;
    const float v = ( y / ( float ) height ) * 2.f - 1.f;

    // float T_STEP = 0.f1f;
    const float4 boxMin = ( float4 )( -1.f, -1.f, -1.f, 1.f );
    const float4 boxMax = ( float4 )( 1.f, 1.f, 1.f, 1.f );

    // Calculate eye ray in world space
    float4 eyeRayDirection;

    const float4 eyeRayOrigin =
                   ( float4 )( invViewMatrix[ 3  ],
                               invViewMatrix[ 7  ],
                               invViewMatrix[ 11 ],
                               1.f );

    const float4 direction = normalize((( float4 )( u, v, -2.f, 0.f )));
    eyeRayDirection.x = dot( direction, (( float4 )( invViewMatrix[ 0  ],
                                                     invViewMatrix[ 1  ],
                                                     invViewMatrix[ 2  ],
                                                     invViewMatrix[ 3  ] )));
    eyeRayDirection.y = dot( direction, (( float4 )( invViewMatrix[ 4  ],
                                                     invViewMatrix[ 5  ],
                                                     invViewMatrix[ 6  ],
                                                     invViewMatrix[ 7  ] )));
    eyeRayDirection.z = dot( direction, (( float4 )( invViewMatrix[ 8  ],
                                                     invViewMatrix[ 9  ],
                                                     invViewMatrix[ 10 ],
                                                     invViewMatrix[ 11 ] )));
    eyeRayDirection.w = 1.f;

    // Find the intersection of the ray with the box
    float tNear, tFar;
    int hit = intersectBox( eyeRayOrigin, eyeRayDirection,
                            boxMin, boxMax, &tNear, &tFar );

    // If it doesn't hit, then return a black value in the corresponding pixel
    if( !hit )
    {
        if(( x < width ) && ( y < height ))
        {
            // Get the 1D index of the pixel to set its color, and return
            const float4 nullPixel = ( float4 )( 0.f , 0.f , 0.f , 0.f );
            const int2 location = (int2)( x , y );
            write_imagef( frameBuffer , location , nullPixel );
        }
        return;
    }

    // Clamp to near plane if the tNear was negative
    if( tNear < 0.f )
        tNear = 0.f;

    // March along the ray accumulating the densities
    float4 intensityBuffer = ( float4 )( 1.f, 1.f, 1.f, 1.f );
    float t = tFar;

    for( uint i = 0 ; i < MAX_STEPS ; i++ )
    {
        // Current position along the ray
        float4 position = eyeRayOrigin + eyeRayDirection * t;

        // Center the texture at the origin, and mapping the positions
        // between 0.f and 1.f
        position = position * 0.5f + 0.5f;    // map position to [0, 1] coordinates

        // Sample the 3D volume data using the _volumeSampler_ at the specified
        // positions along the ray.
        const float4 intensity = read_imagef( volume, volumeSampler, position );

        // update intensity buuffer to  minimum value
        if(intensityBuffer.x > intensity.x)
            intensityBuffer = intensity;

/**
        float4 col;
        if(enableTranferFunction != 0)
        {
        // lookup in transfer function texture
            float2 transfer_pos = (float2)(( intensity.x - transferOffset ) *
                                         transferScale , 0.5f );
            col = read_imagef(transferFunc, transferFuncSampler, transfer_pos);
         }
         else
            col = intensity;
**/
        // Accumulate the result by mixing what is currently in the
        // _intensityBuffer_ with the new intensity value that was sampled from
        // the volume, with the corrsponding alpha components



        // Get the parametric value of the next sample along the ray
        t -= T_STEP;
        if( t < tNear )
            break;
    }

    // Adjust the brightness of the pixel
    intensityBuffer *= brightness;

    // Write the pixel color if it fits only within the image space.
    if(( x < width ) && ( y < height ))
    {
        // Get a 1D index of the pixel in the _frameBuffer_
        const int2 location = (int2)( x , y );
        write_imagef( frameBuffer , location , intensityBuffer );
    }
}
