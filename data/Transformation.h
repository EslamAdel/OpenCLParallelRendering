#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H
#include <Typedefs.hh>

struct Transformation
{
    Transformation();

    ~Transformation(){}

    Transformation( const Transformation &copy );

    Transformation &operator=( const Transformation &rhs );


//    void setRotationX( const int angle );

//    void setRotationY( const int angle );

//    void setRotationZ( const int angle );

//    void setTranslationX( const float d);

//    void setTranslationY( const float d);

//    void setTranslationZ( const float d);


//    void setScaleX( const float scale );

//    void setScaleY( const float scale );

//    void setScaleZ( const float scale );


//    void setVolumeDensity( const float volumeDensity );

//    void setBrightness( const float brightness );


//    float rotationX() const ;

//    float rotationY() const ;

//    float rotationZ() const ;

//    float translationX() const;

//    float translationY() const;

//    float translationZ() const;


//    float scaleX() const;

//    float scaleY() const;

//    float scaleZ() const;

//    float volumeDensity() const;

//    float brightness() const;


//    Coordinates3D &rotation();

//    Coordinates3D &translation();

//    Coordinates3D &scale();


public:

    /**
     * @brief scale_
     */
    Coordinates3D scale ;

    /** @brief rotation_
    */
    Coordinates3D rotation;


     /**
    * @brief translation_
    */
    Coordinates3D translation;


    /**
     * @brief volumeDensity_
     */
    float volumeDensity;


    /**
     * @brief imageBrightness_
     */
    float imageBrightness;

};


#endif // TRANSFORMATION_H
