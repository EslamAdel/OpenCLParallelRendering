#include <GL/glew.h>
#include "GLContext.h"
#include <Volume.h>
#include <Logger.h>

#if defined (__APPLE__) || defined(MACOSX)
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

#define LOCAL_SIZE_X    16
#define LOCAL_SIZE_Y    16

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenGL/OpenGL.h>
#else
#ifdef unix
#include <GL/glx.h>
#endif
#endif

#define INITIAL_FRAME_WIDTH 512
#define INITIAL_FRAME_HEIGHT 512
#define INITIAL_VOLUME_CENTER_X 0.0
#define INITIAL_VOLUME_CENTER_Y 0.0
#define INITIAL_VOLUME_CENTER_Z -4.0
#define INITIAL_VOLUME_ROTATION_X 0.0
#define INITIAL_VOLUME_ROTATION_Y 0.0
#define INITIAL_VOLUME_ROTATION_Z 0.0

#include <oclHWDL/oclHWDL.h>

template < class T >
GLContext< T >::GLContext( Volume< T >* volume )
    : volume_( volume )
{
    LOG_DEBUG( "Creating an OpenGL context for handling the transformation "
               "and displaying the final image" );

    // First initialize OpenGL context, so we can properly setup the
    // interoperability between OpenGL and OpenCL later.
    initializeGL();

    // Initialize the OpenCL context safely after the initialization of the
    // OpenGL context.
    initializeCL_();

    LOG_DEBUG( "[DONE] Creating an OpenGL context for handling the "
               "transformation and displaying the final image" );
}

template < class T >
void GLContext< T >::initializeGL( )
{
    LOG_DEBUG( "Initializing OpenGL context" );

    // This is necessary to avoid the hacks of Qt with OpenGL.
    makeCurrent();

    LOG_DEBUG( "Initializing GLEW ..." );
    glewInit();

    LOG_DEBUG( "Looking for the necessary extensions to run the application" );
    const bool glewInitialized = glewIsSupported
            ( "GL_VERSION_2_0 GL_ARB_pixel_buffer_object" );

    // If the system, can't load all the required extensions to run
    // the application, terminate.
    if( !glewInitialized )
    {
        LOG_ERROR( "GLEW missing required extensions" );
    }

    // Frame resolution
    frameWidth_ = INITIAL_FRAME_WIDTH;
    frameHeight_ = INITIAL_FRAME_HEIGHT;

    // Translation
    translation_.x = INITIAL_VOLUME_CENTER_X;
    translation_.y = INITIAL_VOLUME_CENTER_X;
    translation_.z = INITIAL_VOLUME_CENTER_Z;

    // Rotation
    rotation_.x = INITIAL_VOLUME_ROTATION_X;
    rotation_.x = INITIAL_VOLUME_ROTATION_Y;
    rotation_.x = INITIAL_VOLUME_ROTATION_Z;

    // Set the pixel buffer object to zero, to avoid errors due to
    // reinitialization when the size of the buffer changes.
    glPixelBuffer_ = 0;

    LOG_DEBUG( "[DONE] Initializing OpenGL context" );
}

template < class T >
void GLContext< T >::initializeCL_( )
{
    LOG_DEBUG( "Initializing OpenCL context with GPU" );

    clContext_ = new CLGLContext< T >( volume_, 0 , CL_GL_INTEROPERABILITY);

    clContext_->updateGridSize(frameWidth_, frameHeight_);

    clContext_->handleKernel();
    initializePixelBuffer_();

    LOG_DEBUG( "[DONE] Initializing OpenCL context with GPU" );
}

template < class T >
void GLContext< T >::initializePixelBuffer_( )
{
    LOG_DEBUG( "Creating, and initializing a pixel buffer object that will "
               "share the image between the CL/GL contexts." );

    const size_t bufferSize =
            frameWidth_ * frameHeight_ * sizeof( GLubyte ) * 4;
    glGenBuffersARB( 1, &glPixelBuffer_ );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, glPixelBuffer_ );
    glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, 0,
                     GL_STREAM_DRAW_ARB );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

    LOG_DEBUG( "Register the pixel buffer in the OpenCL context" );
    clContext_->registerGLPixelBuffer( glPixelBuffer_,
                                       frameWidth_, frameHeight_ );

    LOG_DEBUG( "[DONE] Creating, and initializing a pixel buffer object that "
               "will share the image between the CL/GL contexts." );
}

template < class T >
void GLContext< T >::updatePixelBuffer_( )
{
    LOG_DEBUG_GL_LOOP( "Re-initializing the pixel buffers" );

    // Delete the old buffers in both OpenCL and OpenGL contexts, and create
    // new buffers according to the new resolution of the frame.
    LOG_DEBUG_GL_LOOP( "Deleting the old buffers and creating new ones" );
    clContext_->releasePixelBuffer( );
    glDeleteBuffersARB( 1, &glPixelBuffer_ );

    // Create pixel buffer object for displaying the image in OpenGL
    const size_t bufferSize =
            frameWidth_ * frameHeight_ * sizeof(GLubyte) * 4;
    glGenBuffersARB( 1, &glPixelBuffer_ );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, glPixelBuffer_ );
    glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, bufferSize, 0,
                     GL_STREAM_DRAW_ARB );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

    LOG_DEBUG_GL_LOOP( "Register the pixel buffer in the OpenCL context" );
    clContext_->registerGLPixelBuffer( glPixelBuffer_,
                                       frameWidth_, frameHeight_ );

    LOG_DEBUG_GL_LOOP( "[DONE] Re-initializing the pixel buffers" );
}

template < class T >
void GLContext< T >::paintGL( )
{
    LOG_DEBUG_GL_LOOP( "Drawing a frame" );

    // Use OpenGL to build model view matrix and compute its inverse.
    GLfloat modelViewMatrix[ 16 ];
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix( );
    glLoadIdentity( );

    // Rotation
    glRotatef( -rotation_.x, 1.0, 0.0, 0.0 );
    glRotatef( -rotation_.y, 0.0, 1.0, 0.0 );
    glRotatef( -rotation_.z, 0.0, 0.0, 1.0 );

    // Translation
    glTranslatef( -translation_.x, -translation_.y, -translation_.z );

    // Get the matrix from the OpenGL state.
    glGetFloatv( GL_MODELVIEW_MATRIX, modelViewMatrix );
    glPopMatrix( );

    // Compute the inverse matrix from the original one.
    inverseMatrix_[0] = modelViewMatrix[0];
    inverseMatrix_[1] = modelViewMatrix[4];
    inverseMatrix_[2] = modelViewMatrix[8];
    inverseMatrix_[3] = modelViewMatrix[12];
    inverseMatrix_[4] = modelViewMatrix[1];
    inverseMatrix_[5] = modelViewMatrix[5];
    inverseMatrix_[6] = modelViewMatrix[9];
    inverseMatrix_[7] = modelViewMatrix[13];
    inverseMatrix_[8] = modelViewMatrix[2];
    inverseMatrix_[9] = modelViewMatrix[6];
    inverseMatrix_[10] = modelViewMatrix[10];
    inverseMatrix_[11] = modelViewMatrix[14];

    glFlush();

    LOG_DEBUG_GL_LOOP( "Render a frame using the OpenCL kernel" );
    clContext_->renderFrame( inverseMatrix_ );

    // Draw image from the PBO
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_DEPTH_TEST );
    glRasterPos2i( 0, 0 );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, glPixelBuffer_ );
    glDrawPixels( frameWidth_, frameHeight_, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

    LOG_DEBUG_GL_LOOP( "[DONE] Drawing a frame" );
}

template < class T >
void GLContext< T >::resizeGL(int x, int y)
{
    LOG_DEBUG_GL_LOOP( "Updating the frame resolution" );

    frameWidth_ = x;
    frameHeight_ = y;

    // Reinitialize the buffer if the dimensions of the image are different
    updatePixelBuffer_();

    // Update the frustum
    glViewport( 0, 0, x, y );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 );

    LOG_DEBUG_GL_LOOP( "[DONE] Updating the frame resolution" );
}

template < class T >
QPixmap GLContext< T >::renderFrameToPixmap( )
{
    LOG_DEBUG_GL_LOOP( "Render the frame to a Pixmap" );

    return renderPixmap();
}

template < class T >
void GLContext< T >::updateRotationX_SLOT( int angle )
{
    LOG_DEBUG_GL_LOOP( "Updating X-angle" );

    rotation_.x = float( angle );
    updateGL();
}

template < class T >
void GLContext< T >::updateRotationY_SLOT( int angle )
{
    LOG_DEBUG_GL_LOOP( "Updating Y-angle" );

    rotation_.y = angle;
    updateGL();
}

template < class T >
void GLContext< T >::updateRotationZ_SLOT( int angle )
{
    LOG_DEBUG_GL_LOOP( "Updating Z-angle" );

    rotation_.z = angle;
    updateGL();
}

template < class T >
GLContext< T >::~GLContext()
{
    clContext_->~CLGLContext();
}

#include <GLContext.ipp>
