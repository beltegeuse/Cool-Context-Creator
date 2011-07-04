/*
 * WindowImplLinux.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: adrien
 */

// PCM Includes
#include <PCM/Linux/WindowImplLinux.h>
#include <PCM/Exceptions.h>
#include <PCM/Trace.h>

// OpenGL Includes
#include <GL/gl.h>
#include <GL/glx.h>

// Other Includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sstream>

namespace PCM {
namespace priv {

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(::Display*, GLXFBConfig,
		GLXContext, Bool, const int*);

unsigned long eventMask = FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
                                                PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
                                                EnterWindowMask | LeaveWindowMask;

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

Bool CheckEvent(::Display*, XEvent* event, XPointer userData)
{
	// Just check if the event matches the window
	return event->xany.window == reinterpret_cast< ::Window >(userData);
}

WindowImplLinux::WindowImplLinux(const WindowMode& mode,
		const std::string& name, const OpenGLContextSettings& settings) :
	m_Display(NULL),m_InputMethod (NULL),
	m_InputContext(NULL),m_AtomClose (0),
	m_KeyRepeat(false)
{
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

	/*
	 * GLX version checking
	 */
	int glx_major, glx_minor;
	if ( !glXQueryVersion( m_Display, &glx_major, &glx_minor ) ||
	   ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
			throw new CException( "Invalid GLX version" );

	/*
	 * Find best configuration
	 * TODO: Voir comment la SFML fait pour ca ...
	 */
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

	/*
	 * Create window
	 */
	TRACE( "Creating colormap" );
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap( m_Display,
										 RootWindow( m_Display, vi->screen ),
										 vi->visual, AllocNone );
	swa.background_pixmap = None ;
	swa.border_pixel      = 0;
	swa.event_mask        = eventMask; //StructureNotifyMask
	//swa = fullscreen; //TODO: For fullscreen

	TRACE( "Creating window" );
	m_Window = XCreateWindow( m_Display, RootWindow( m_Display, vi->screen ),
							  0, 0, mode.Width, mode.Height, 0, vi->depth, InputOutput,
							  vi->visual,
							  CWBorderPixel|CWColormap|CWEventMask, &swa );
	if ( !m_Window )
		throw new CException( "Failed to create window." );

	// Done with the visual info data
	XFree( vi );

	// Put the window name
	SetTitle(name);

	/*
	 * Change window style
	 */
	if (!mode.Fullscreen) // If not fullscreen
	{
		Atom WMHintsAtom = XInternAtom(m_Display, "_MOTIF_WM_HINTS", false);
		if (WMHintsAtom)
		{
			static const unsigned long MWM_HINTS_FUNCTIONS = 1 << 0;
			static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

			//static const unsigned long MWM_DECOR_ALL = 1 << 0;
			static const unsigned long MWM_DECOR_BORDER = 1 << 1;
			static const unsigned long MWM_DECOR_RESIZEH = 1 << 2;
			static const unsigned long MWM_DECOR_TITLE = 1 << 3;
			static const unsigned long MWM_DECOR_MENU = 1 << 4;
			static const unsigned long MWM_DECOR_MINIMIZE = 1 << 5;
			static const unsigned long MWM_DECOR_MAXIMIZE = 1 << 6;

			//static const unsigned long MWM_FUNC_ALL = 1 << 0;
			static const unsigned long MWM_FUNC_RESIZE = 1 << 1;
			static const unsigned long MWM_FUNC_MOVE = 1 << 2;
			static const unsigned long MWM_FUNC_MINIMIZE = 1 << 3;
			static const unsigned long MWM_FUNC_MAXIMIZE = 1 << 4;
			static const unsigned long MWM_FUNC_CLOSE = 1 << 5;

			struct WMHints
			{
				unsigned long Flags;
				unsigned long Functions;
				unsigned long Decorations;
				long InputMode;
				unsigned long State;
			};

			WMHints hints;
			hints.Flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
			hints.Decorations = 0;
			hints.Functions = 0;

			//TODO: Les tests de la SFML pour activer que certains evenements
//			if (style & Style::Titlebar)
//			{
				hints.Decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
				hints.Functions |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE;
//			}
//			if (style & Style::Resize)
//			{
				hints.Decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
				hints.Functions |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
//			}
//			if (style & Style::Close)
//			{
				hints.Decorations |= 0;
				hints.Functions |= MWM_FUNC_CLOSE;
//			}

			const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&hints);
			XChangeProperty(m_Display, m_Window, WMHintsAtom, WMHintsAtom, 32, PropModeReplace, ptr, 5);
		}

		//TODO: A voir si intergration
		// This is a hack to force some windows managers to disable resizing
//		if (!(style & Style::Resize))
//		{
//			XSizeHints sizeHints;
//			sizeHints.flags = PMinSize | PMaxSize;
//			sizeHints.min_width = sizeHints.max_width = width;
//			sizeHints.min_height = sizeHints.max_height = height;
//			XSetWMNormalHints(myDisplay, myWindow, &sizeHints);
//		}
	}

	/*
	 * Input section
	 */

	// Make sure the "last key release" is initialized with invalid values
	m_LastKeyReleaseEvent.type = -1;

	// Get the atom defining the close event
	m_AtomClose = XInternAtom(m_Display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(m_Display, m_Window, &m_AtomClose, 1);

	// Create the input context
	m_InputMethod = XOpenIM(m_Display, NULL, NULL, NULL);
	if (m_InputMethod)
	{
		m_InputContext = XCreateIC(m_InputMethod,
								   XNClientWindow, m_Window,
								   XNFocusWindow, m_Window,
								   XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
								   NULL);
	}
	else
	{
		m_InputContext = NULL;
	}
	if (!m_InputContext)
		throw new CException("Failed to create input context for window -- TextEntered event won't be able to return unicode");



	// Display the window
	TRACE( "Mapping window" );
	Show(true);

	/*
	 * OpenGL section
	 */
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
	// TODO: Faire la gestion d'erreur (Voir SFML)
	glXMakeCurrent(m_Display, 0, 0);
	glXDestroyContext(m_Display, m_Context);

	XDestroyWindow(m_Display, m_Window);
	XFreeColormap(m_Display, m_Colormap);
	XCloseDisplay( m_Display);
}

void WindowImplLinux::Display() {
	glXSwapBuffers(m_Display, m_Window);
}

void WindowImplLinux::SetTitle(const std::string& title)
{
	XStoreName( m_Display, m_Window, title.c_str() );
}

void WindowImplLinux::Show(bool show)
{
	if (show)
		XMapWindow(m_Display, m_Window);
	else
		XUnmapWindow(m_Display, m_Window);

	XFlush(m_Display);
}

void WindowImplLinux::ProcessEvents(bool block)
{
	XEvent event;
	if (block)
	{
		// Blocking -- wait and process events in the event queue until a valid event is found
		do
		{
			XIfEvent(m_Display, &event, &CheckEvent, reinterpret_cast<XPointer>(m_Window));
		}
		while (!ProcessEvent(event));
	}
	else
	{
		// Non-blocking -- process all events in the event queue
		while (XCheckIfEvent(m_Display, &event, &CheckEvent, reinterpret_cast<XPointer>(m_Window)))
		{
			ProcessEvent(event);
		}
	}
}

bool WindowImplLinux::ProcessEvent(XEvent windowEvent)
{
	// This function implements a workaround to properly discard
	// repeated key events when necessary. The problem is that the
	// system's key events policy doesn't match SFML's one: X server will generate
	// both repeated KeyPress and KeyRelease events when maintaining a key down, while
	// SFML only wants repeated KeyPress events. Thus, we have to:
	// - Discard duplicated KeyRelease events when EnableKeyRepeat is true
	// - Discard both duplicated KeyPress and KeyRelease events when EnableKeyRepeat is false
	// Detect repeated key events
	if ((windowEvent.type == KeyPress) || (windowEvent.type == KeyRelease))
	{
		if (windowEvent.xkey.keycode < 256)
		{
			// To detect if it is a repeated key event, we check the current state of the key.
			// - If the state is "down", KeyReleased events must obviously be discarded.
			// - KeyPress events are a little bit harder to handle: they depend on the EnableKeyRepeat state,
			// and we need to properly forward the first one.
			char keys[32];
			XQueryKeymap(m_Display, keys);
			if (keys[windowEvent.xkey.keycode >> 3] & (1 << (windowEvent.xkey.keycode % 8)))
			{
				// KeyRelease event + key down = repeated event --> discard
				if (windowEvent.type == KeyRelease)
				{
					m_LastKeyReleaseEvent = windowEvent;
					return false;
				}

				// KeyPress event + key repeat disabled + matching KeyRelease event = repeated event --> discard
				if ((windowEvent.type == KeyPress) && !m_KeyRepeat &&
					(m_LastKeyReleaseEvent.xkey.keycode == windowEvent.xkey.keycode) &&
					(m_LastKeyReleaseEvent.xkey.time == windowEvent.xkey.time))
				{
					return false;
				}
			}
		}
	}

	// Convert the X11 event to a sf::Event
	switch (windowEvent.type)
	{

		// Destroy event
		case DestroyNotify :
		{
                        TRACE("The window is destroyed.");
			// The window is about to be destroyed : we must cleanup resources
			//CleanUp(); //FIXME
			break;
		}

		// Gain focus event
		case FocusIn :
		{
                        TRACE("The window gains focus.");
			// Update the input context
			if (m_InputContext)
				XSetICFocus(m_InputContext);

			Event event;
			event.Type = Event::GainedFocus;
			PushEvent(event);
			break;
		}

		// Lost focus event
		case FocusOut :
		{
                        TRACE("The window loses focus.");
			// Update the input context
			if (m_InputContext)
				XUnsetICFocus(m_InputContext);

			Event event;
			event.Type = Event::LostFocus;
			PushEvent(event);
			break;
		}

		// Resize event
		case ConfigureNotify :
		{
			if ((windowEvent.xconfigure.width != static_cast<int>(m_Width)) || (windowEvent.xconfigure.height != static_cast<int>(m_Height)))
			{
				m_Width = windowEvent.xconfigure.width;
				m_Height = windowEvent.xconfigure.height;

                                // For trace
                                std::stringstream sstrace;
                                sstrace << "The window is resized to (" << m_Width << " , " << m_Height << ")";
                                TRACE(sstrace.str());

                                // End of trace

				Event event;
				event.Type = Event::Resized;
				event.Size.Width = m_Width;
				event.Size.Height = m_Height;
				PushEvent(event);
			}
			break;
		}

		// Close event
		case ClientMessage :
		{
			if ((windowEvent.xclient.format == 32) && (windowEvent.xclient.data.l[0]) == static_cast<long>(m_AtomClose))
			{
                                TRACE("The window is closed");
				Event event;
				event.Type = Event::Closed;
				PushEvent(event);
			}
			break;
		}

		// Key down event
		case KeyPress :
		{
			// Get the keysym of the key that has been pressed
			static XComposeStatus keyboard;
			char buffer[32];
			KeySym symbol;
			XLookupString(&windowEvent.xkey, buffer, sizeof(buffer), &symbol, &keyboard);

                        // For the trace
                        // TODO : handle the special keys
                        std::string strace(buffer);
                        std::stringstream sstrace;
                        sstrace << "A key has been pressed : " << strace;
                        TRACE(sstrace.str());
                        // End of trace


			// Fill the event parameters
			// TODO: if modifiers are wrong, use XGetModifierMapping to retrieve the actual modifiers mapping
			Event event;
			event.Type = Event::KeyPressed;
			event.Key.Code = KeysymToSF(symbol);
			event.Key.Alt = windowEvent.xkey.state & Mod1Mask;
			event.Key.Control = windowEvent.xkey.state & ControlMask;
			event.Key.Shift = windowEvent.xkey.state & ShiftMask;
			//event.Key.System = windowEvent.xkey.state & Mod4Mask; //FIXME
			PushEvent(event);

			// Generate a TextEntered event
			if (!XFilterEvent(&windowEvent, None))
			{
//#ifdef X_HAVE_UTF8_STRING
//				if (m_InputContext)
//				{
//					Status status;
//					Uint8 keyBuffer[16];
//					int length = Xutf8LookupString(m_InputContext, &windowEvent.xkey, reinterpret_cast<char*>(keyBuffer), sizeof(keyBuffer), NULL, &status);
//					if (length > 0)
//					{
//						Uint32 unicode = 0;
//						Utf8::Decode(keyBuffer, keyBuffer + length, unicode, 0);
//						if (unicode != 0)
//						{
//							Event textEvent;
//							textEvent.Type = Event::TextEntered;
//							textEvent.Text.Unicode = unicode;
//							PushEvent(textEvent);
//						}
//					}
//				}
//				else
//#endif
				{
					static XComposeStatus status;
					char keyBuffer[16];
					if (XLookupString(&windowEvent.xkey, keyBuffer, sizeof(keyBuffer), NULL, &status))
					{
						Event textEvent;
						textEvent.Type = Event::TextEntered;
						//textEvent.Text.Unicode = static_cast<Uint32>(keyBuffer[0]); //FIXME
						PushEvent(textEvent);
					}
				}
			}

			break;
		}

		// Key up event
		case KeyRelease :
		{

			// Get the keysym of the key that has been pressed
			char buffer[32];
			KeySym symbol;
			XLookupString(&windowEvent.xkey, buffer, 32, &symbol, NULL);

                        // For the trace
                        // TODO : handle the special keys
                        std::string strace(buffer);
                        std::stringstream sstrace;
                        sstrace << "A key has been released : " << strace;
                        TRACE(sstrace.str());
                        // End of trace

			// Fill the event parameters
			Event event;
			event.Type = Event::KeyReleased;
			event.Key.Code = KeysymToSF(symbol);
			event.Key.Alt = windowEvent.xkey.state & Mod1Mask;
			event.Key.Control = windowEvent.xkey.state & ControlMask;
			event.Key.Shift = windowEvent.xkey.state & ShiftMask;
			//event.Key.System = windowEvent.xkey.state & Mod4Mask; // FIXME
			PushEvent(event);

			break;
		}

		// Mouse button pressed
		case ButtonPress :
		{
			unsigned int button = windowEvent.xbutton.button;
			if ((button == Button1) || (button == Button2) || (button == Button3) || (button == 8) || (button == 9))
			{
				Event event;
				event.Type = Event::MouseButtonPressed;
				event.MouseButton.X = windowEvent.xbutton.x;
				event.MouseButton.Y = windowEvent.xbutton.y;
				switch (button)
				{
                                        case Button1 : event.MouseButton.Button = Mouse::Left;
                                                       TRACE("Mouse left button pressed"); break;
                                        case Button2 : event.MouseButton.Button = Mouse::Middle;
                                                       TRACE("Mouse middle button pressed"); break;
                                        case Button3 : event.MouseButton.Button = Mouse::Right;
                                                       TRACE("Mouse right button pressed"); break;
                                        case 8 : event.MouseButton.Button = Mouse::XButton1;
                                                       TRACE("Mouse button 1 pressed"); break;
                                        case 9 : event.MouseButton.Button = Mouse::XButton2;
                                                       TRACE("Mouse button 2 pressed"); break;
				}
				PushEvent(event);
			}
			break;
		}

		// Mouse button released
		case ButtonRelease :
		{
			unsigned int button = windowEvent.xbutton.button;
			if ((button == Button1) || (button == Button2) || (button == Button3) || (button == 8) || (button == 9))
			{
				Event event;
				event.Type = Event::MouseButtonReleased;
				event.MouseButton.X = windowEvent.xbutton.x;
				event.MouseButton.Y = windowEvent.xbutton.y;
				switch (button)
				{
                                        case Button1 : event.MouseButton.Button = Mouse::Left;
                                                       TRACE("Mouse left button released"); break;
                                        case Button2 : event.MouseButton.Button = Mouse::Middle;
                                                       TRACE("Mouse right button released"); break;
                                        case Button3 : event.MouseButton.Button = Mouse::Right;
                                                       TRACE("Mouse middle button released"); break;
                                        case 8 : event.MouseButton.Button = Mouse::XButton1;
                                                       TRACE("Mouse button 1 released"); break;
                                        case 9 : event.MouseButton.Button = Mouse::XButton2;
                                                       TRACE("Mouse button 2 released"); break;
				}
				PushEvent(event);
			}
			else if ((button == Button4) || (button == Button5))
			{
				Event event;
				event.Type = Event::MouseWheelMoved;
				event.MouseWheel.Delta = windowEvent.xbutton.button == Button4 ? 1 : -1;
                                if (event.MouseWheel.Delta)
                                    TRACE("Mouse wheel moved."); //TODO : how can we know if it went up or not?
				event.MouseWheel.X = windowEvent.xbutton.x;
				event.MouseWheel.Y = windowEvent.xbutton.y;
				PushEvent(event);
			}
			break;
		}

		// Mouse moved
		case MotionNotify :
		{
			Event event;
			event.Type = Event::MouseMoved;
			event.MouseMove.X = windowEvent.xmotion.x;
			event.MouseMove.Y = windowEvent.xmotion.y;
			PushEvent(event);

                        // For the trace
                        std::stringstream otrace;
                        otrace << "Mouse new position : (";
                        otrace << event.MouseMove.X;
                        otrace << " , ";
                        otrace << event.MouseMove.Y;
                        otrace << ")";
                        TRACE(otrace.str());
                        // End of trace


			break;
		}

		// Mouse entered
		case EnterNotify :
		{
                        TRACE("The mouse entered the window area");
			Event event;
			event.Type = Event::MouseEntered;
			PushEvent(event);
			break;
		}

		// Mouse left
		case LeaveNotify :
		{
                        TRACE("The mouse left the window area");
			Event event;
			event.Type = Event::MouseLeft;
			PushEvent(event);
			break;
		}
	}

	return true;
}

Key::Code WindowImplLinux::KeysymToSF(KeySym symbol)
{
	// First convert to uppercase (to avoid dealing with two different keysyms for the same key)
	KeySym lower, key;
	XConvertCase(symbol, &lower, &key);

	switch (key)
	{
		case XK_Shift_L : return Key::LShift;
		case XK_Shift_R : return Key::RShift;
		case XK_Control_L : return Key::LControl;
		case XK_Control_R : return Key::RControl;
		case XK_Alt_L : return Key::LAlt;
		case XK_Alt_R : return Key::RAlt;
		case XK_Super_L : return Key::LSystem;
		case XK_Super_R : return Key::RSystem;
		case XK_Menu : return Key::Menu;
		case XK_Escape : return Key::Escape;
		case XK_semicolon : return Key::SemiColon;
		case XK_slash : return Key::Slash;
		case XK_equal : return Key::Equal;
		case XK_minus : return Key::Dash;
		case XK_bracketleft : return Key::LBracket;
		case XK_bracketright : return Key::RBracket;
		case XK_comma : return Key::Comma;
		case XK_period : return Key::Period;
		case XK_dead_acute : return Key::Quote;
		case XK_backslash : return Key::BackSlash;
		case XK_dead_grave : return Key::Tilde;
		case XK_space : return Key::Space;
		case XK_Return : return Key::Return;
		case XK_KP_Enter : return Key::Return;
		case XK_BackSpace : return Key::Back;
		case XK_Tab : return Key::Tab;
		case XK_Prior : return Key::PageUp;
		case XK_Next : return Key::PageDown;
		case XK_End : return Key::End;
		case XK_Home : return Key::Home;
		case XK_Insert : return Key::Insert;
		case XK_Delete : return Key::Delete;
		case XK_KP_Add : return Key::Add;
		case XK_KP_Subtract : return Key::Subtract;
		case XK_KP_Multiply : return Key::Multiply;
		case XK_KP_Divide : return Key::Divide;
		case XK_Pause : return Key::Pause;
		case XK_F1 : return Key::F1;
		case XK_F2 : return Key::F2;
		case XK_F3 : return Key::F3;
		case XK_F4 : return Key::F4;
		case XK_F5 : return Key::F5;
		case XK_F6 : return Key::F6;
		case XK_F7 : return Key::F7;
		case XK_F8 : return Key::F8;
		case XK_F9 : return Key::F9;
		case XK_F10 : return Key::F10;
		case XK_F11 : return Key::F11;
		case XK_F12 : return Key::F12;
		case XK_F13 : return Key::F13;
		case XK_F14 : return Key::F14;
		case XK_F15 : return Key::F15;
		case XK_Left : return Key::Left;
		case XK_Right : return Key::Right;
		case XK_Up : return Key::Up;
		case XK_Down : return Key::Down;
		case XK_KP_0 : return Key::Numpad0;
		case XK_KP_1 : return Key::Numpad1;
		case XK_KP_2 : return Key::Numpad2;
		case XK_KP_3 : return Key::Numpad3;
		case XK_KP_4 : return Key::Numpad4;
		case XK_KP_5 : return Key::Numpad5;
		case XK_KP_6 : return Key::Numpad6;
		case XK_KP_7 : return Key::Numpad7;
		case XK_KP_8 : return Key::Numpad8;
		case XK_KP_9 : return Key::Numpad9;
		case XK_A : return Key::A;
		case XK_Z : return Key::Z;
		case XK_E : return Key::E;
		case XK_R : return Key::R;
		case XK_T : return Key::T;
		case XK_Y : return Key::Y;
		case XK_U : return Key::U;
		case XK_I : return Key::I;
		case XK_O : return Key::O;
		case XK_P : return Key::P;
		case XK_Q : return Key::Q;
		case XK_S : return Key::S;
		case XK_D : return Key::D;
		case XK_F : return Key::F;
		case XK_G : return Key::G;
		case XK_H : return Key::H;
		case XK_J : return Key::J;
		case XK_K : return Key::K;
		case XK_L : return Key::L;
		case XK_M : return Key::M;
		case XK_W : return Key::W;
		case XK_X : return Key::X;
		case XK_C : return Key::C;
		case XK_V : return Key::V;
		case XK_B : return Key::B;
		case XK_N : return Key::N;
		case XK_0 : return Key::Num0;
		case XK_1 : return Key::Num1;
		case XK_2 : return Key::Num2;
		case XK_3 : return Key::Num3;
		case XK_4 : return Key::Num4;
		case XK_5 : return Key::Num5;
		case XK_6 : return Key::Num6;
		case XK_7 : return Key::Num7;
		case XK_8 : return Key::Num8;
		case XK_9 : return Key::Num9;
	}

	return Key::Code(0);
}

// End namespaces
}
}
