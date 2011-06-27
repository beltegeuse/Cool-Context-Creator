/*
 * WindowImplLinux.h
 *
 *  Created on: Jun 25, 2011
 *      Author: adrien
 */

#ifndef WINDOWIMPLLINUX_H_
#define WINDOWIMPLLINUX_H_

// PCM Includes
#include <WindowImpl.h>

// X Includes
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace PCM
{
namespace priv
{

class WindowImplLinux : public WindowImpl
{
private:
	/*
	 * Attributes
	 */
	::Display* m_Display;
	::Window m_Window;
	::Colormap m_Colormap;
	GLXContext m_Context;
public:
	/*
	 * Constructors and destructors
	 */
	WindowImplLinux(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings);
	virtual ~WindowImplLinux();

	/*
	 * Virtual methods
	 */
	virtual void Display();
	virtual void ProcessEvents(bool block);
};

// Namespaces ends
}
}

#endif /* WINDOWIMPLLINUX_H_ */
