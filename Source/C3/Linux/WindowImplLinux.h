#ifndef WINDOWIMPLLINUX_H_
#define WINDOWIMPLLINUX_H_

// PCM Includes
#include <C3/WindowImpl.h>

// X Includes
#include <X11/Xlib.h>
#include <GL/glx.h>

namespace C3
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
	XEvent     m_LastKeyReleaseEvent; ///< Last key release event we received (needed for discarding repeated key events)
	Atom       m_AtomClose;           ///< Atom used to identify the close event
	XIC        m_InputContext;        ///< Input context used to get unicode input in our window
	XIM        m_InputMethod;         ///< Input method linked to the X display
        int        m_OldVideoMode;        ///< In order to reset the screen when we leave the fullscreen
	bool m_KeyRepeat;
public:
	/*
	 * Constructors and destructors
	 */
	WindowImplLinux(const WindowMode& mode, const std::string& name,
                        const OpenGLContextSettings& settings, long style = Style::Default);
	virtual ~WindowImplLinux();

	/*
	 * Virtual methods
	 */
	virtual void Display();
	virtual void ProcessEvents(bool block);
	virtual void SetTitle(const std::string& title);
	virtual void Show(bool show);
	virtual void EnableKeyRepeat(bool enabled);

private:
	/*
	 * Private methods
	 */
	bool ProcessEvent(XEvent windowEvent);
	static Key::Code KeysymToSF(KeySym symbol);
        void SwitchToFullscreen(int width, int height);
        void CleanUp();
};

// Namespaces ends
}
}

#endif /* WINDOWIMPLLINUX_H_ */
