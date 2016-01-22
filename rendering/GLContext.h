#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include <QGLWidget>
#include <iostream>
#include <string>

#include "oclUtils.h"
#include "Utilities.h"
#include <Volume.h>
#include <CLGLContext.h>

template < class T >
class GLContext : public QGLWidget
{
public:
    GLContext( Volume< T >* volume );
    ~GLContext();

public:

    /**
     * @brief renderFrameToPixmap
     * @return
     */
    QPixmap renderFrameToPixmap( );

protected:

    /**
     * @brief initializeGL
     */
    void initializeGL( );

    /**
     * @brief paintGL
     */
    void paintGL( );

    /**
     * @brief resizeGL
     * @param x
     * @param y
     */
    void resizeGL( int x, int y );

public slots:

    /**
     * @brief updateRotationX_SLOT
     * @param angle
     */
    void updateRotationX_SLOT( int angle );

    /**
     * @brief updateRotationY_SLOT
     * @param angle
     */
    void updateRotationY_SLOT( int angle );

    /**
     * @brief updateRotationZ_SLOT
     * @param angle
     */
    void updateRotationZ_SLOT( int angle );

private:

    /**
     * @brief initializeCL_
     */
    void initializeCL_( );

    /**
     * @brief initializePixelBuffer_
     */
    void initializePixelBuffer_( );

    /**
     * @brief updatePixelBuffer_
     */
    void updatePixelBuffer_( );

private:

    /**
     * @brief volume_
     */
    const Volume< T >* volume_;

    /**
     * @brief clContext_
     */
    CLGLContext< T >* clContext_;

    /**
     * @brief frameWidth_
     */
    uint frameWidth_;

    /**
     * @brief frameHeight_
     */
    uint frameHeight_;

    /**
     * @brief glPixelBuffer_
     */
    GLuint glPixelBuffer_;

    /**
     * @brief rotation_
     */
    Coordinates3D rotation_;

    /**
     * @brief translation_
     */
    Coordinates3D translation_;

    /**
     * @brief inverseMatrix_
     */
    float inverseMatrix_[ 12 ];
};

#endif // GLCONTEXT_H
