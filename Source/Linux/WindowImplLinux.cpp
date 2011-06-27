/*
 * WindowImplLinux.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: adrien
 */

// PCM Includes
#include "WindowImplLinux.h"
#include "Exceptions.h"
#include "Trace.h"

// OpenGL Includes
#include <GL/gl.h>
#include <GL/glx.h>

// Other Includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace PCM {
namespace priv {

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(::Display*, GLXFBConfig,
		GLXContext, Bool, const int*);

#include <string.h>
// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)

{

	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
	 OpenGL extensions string. Don't be fooled by sub-strings,
	 etc. */
	for (start = extList;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;

		start = terminator;
	}

	return false;
}
static bool m_ContextErrorOccurred = false;
static int m_ContextErrorHandler(::Display *dpy, XErrorEvent *ev) {
	m_ContextErrorOccurred = true;
	return 0;
}

WindowImplLinux::WindowImplLinux(const WindowMode& mode,
		const std::string& name, const OpenGLContextSettings& settings) :
	m_Display(NULL) {
	m_Display = XOpenDisplay(NULL);

	if ( !m_Display )
		throw new CException( "Failed to open X m_Display" );

	// Get a matching FB config
	static int visual_attribs[] =
	{
	  GLX_X_RENDERABLE    , True,
	  GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
	  GLX_RENDER_TYPE     , GLX_RGBA_BIT,
	  GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
	  GLX_RED_SIZE        , 8,
	  GLX_GREEN_SIZE      , 8,
	  GLX_BLUE_SIZE       , 8,
	  GLX_ALPHA_SIZE      , 8,
	  GLX_DEPTH_SIZE      , settings.DepthBits,
	  GLX_STENCIL_SIZE    , settings.StentilBits,
	  GLX_DOUBLEBUFFER    , True,
	  //GLX_SAMPLE_BUFFERS  , 1,
	  //GLX_SAMPLES         , 4,
	  None
	};

	int glx_major, glx_minor;

	// FBConfigs were added in GLX version 1.3.
	if ( !glXQueryVersion( m_Display, &glx_major, &glx_minor ) ||
	   ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
			throw new CException( "Invalid GLX version" );

	TRACE( "Getting matching framebuffer configs" );
	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig( m_Display, DefaultScreen( m_Display ),
										visual_attribs, &fbcount );
	if ( !fbc )
		throw new CException( "Failed to retrieve a framebuffer config" );

	TRACE( "Found "<< fbcount <<" matching FB configs." );

	// Pick the FB config/visual with the most samples per pixel
	TRACE( "Getting XVisualInfos" );
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for ( i = 0; i < fbcount; i++ )
	{
	XVisualInfo *vi = glXGetVisualFromFBConfig( m_Display, fbc[i] );
	if ( vi )
	{
	  int samp_buf, samples;
	  glXGetFBConfigAttrib( m_Display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
	  glXGetFBConfigAttrib( m_Display, fbc[i], GLX_SAMPLES       , &samples  );

	  TRACE( "  Matching fbconfig " << i << ", visual ID " << vi -> visualid << ": SAMPLE_BUFFERS = " << samp_buf << ", SAMPLES = " << samples );

	  if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
		best_fbc = i, best_num_samp = samples;
	  if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
		worst_fbc = i, worst_num_samp = samples;
	}
	XFree( vi );
	}

	GLXFBConfig bestFbc = fbc[ best_fbc ];

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree( fbc );

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig( m_Display, bestFbc );
	TRACE( "Chosen visual ID = " << vi->visualid );

	TRACE( "Creating colormap" );
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap( m_Display,
										 RootWindow( m_Display, vi->screen ),
										 vi->visual, AllocNone );
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = StructureNotifyMask;

	TRACE( "Creating window" );
	m_Window = XCreateWindow( m_Display, RootWindow( m_Display, vi->screen ),
							  0, 0, mode.Width, mode.Height, 0, vi->depth, InputOutput,
							  vi->visual,
							  CWBorderPixel|CWColormap|CWEventMask, &swa );
	if ( !m_Window )
		throw new CException( "Failed to create window." );

	// Done with the visual info data
	XFree( vi );

	XStoreName( m_Display, m_Window, name.c_str() );

	TRACE( "Mapping window" );
	XMapWindow( m_Display, m_Window );

	// Get the default screen's GLX extension list
	const char *glxExts = glXQueryExtensionsString( m_Display,
												  DefaultScreen( m_Display ) );

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		   glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB" );

	GLXContext m_Context = 0;

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	//
	// Note this error handler is global.  All m_Display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	m_ContextErrorOccurred = false;
	int (*oldHandler)(::Display*, XErrorEvent*) =
	  XSetErrorHandler(&m_ContextErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if ( !isExtensionSupported( glxExts, "GLX_ARB_create_context" ) ||
	   !glXCreateContextAttribsARB )
	{
	TRACE( "glXCreateContextAttribsARB() not found"
			" ... using old-style GLX context" );
	m_Context = glXCreateNewContext( m_Display, bestFbc, GLX_RGBA_TYPE, 0, True );
	}

	// If it does, try to get a GL 3.0 context!
	else
	{
	int context_attribs[] =
	  {
		GLX_CONTEXT_MAJOR_VERSION_ARB, settings.MajorVersion,
		GLX_CONTEXT_MINOR_VERSION_ARB, settings.MinorVersion,
		//GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	  };

	TRACE( "Creating context" );
	m_Context = glXCreateContextAttribsARB( m_Display, bestFbc, 0,
									  True, context_attribs );

	// Sync to ensure any errors generated are processed.
	XSync( m_Display, False );
	if ( !m_ContextErrorOccurred && m_Context )
	  TRACE( "Created GL " << settings.MajorVersion << "." << settings.MinorVersion << " context" );
	else
	{
	  // Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
	  // When a context version below 3.0 is requested, implementations will
	  // return the newest context version compatible with OpenGL versions less
	  // than version 3.0.
	  // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
	  context_attribs[1] = 1;
	  // GLX_CONTEXT_MINOR_VERSION_ARB = 0
	  context_attribs[3] = 0;

	  m_ContextErrorOccurred = false;

	  TRACE( "Failed to create GL " << settings.MajorVersion << "." << settings.MinorVersion << " context"
			  " ... using old-style GLX context" );
	  m_Context = glXCreateContextAttribsARB( m_Display, bestFbc, 0,
										True, context_attribs );
	}
	}

	// Sync to ensure any errors generated are processed.
	XSync( m_Display, False );

	// Restore the original error handler
	XSetErrorHandler( oldHandler );

	if ( m_ContextErrorOccurred || !m_Context )
		throw new CException( "Failed to create an OpenGL context" );

	// Verifying that context is a direct context
	if ( ! glXIsDirect ( m_Display, m_Context ) )
		TRACE( "Indirect GLX rendering context obtained" );
	else
		TRACE( "Direct GLX rendering context obtained" );

	TRACE( "Making context current" );
	glXMakeCurrent( m_Display, m_Window, m_Context );
}

WindowImplLinux::~WindowImplLinux() {
	glXMakeCurrent(m_Display, 0, 0);
	glXDestroyContext(m_Display, m_Context);

	XDestroyWindow(m_Display, m_Window);
	XFreeColormap(m_Display, m_Colormap);
	XCloseDisplay( m_Display);
}

void WindowImplLinux::Display() {
	glXSwapBuffers(m_Display, m_Window);
}

void WindowImplLinux::ProcessEvents(bool block)
{
}

// End namespaces
}
}
