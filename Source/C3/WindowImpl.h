#ifndef WINDOWIMPL_H_
#define WINDOWIMPL_H_

// STL includes
#include <string>
#include <queue>

// PCM includes
#include <C3/WindowMode.h>
#include <C3/OpenGLContextSettings.h>
#include <C3/Events.h>
#include <C3/WindowHandle.h>

namespace C3
{

namespace Style //TODO: put the namespace Style in another file if here is not the right place
{
    ////////////////////////////////////////////////////////////
    /// \ingroup window
    /// \brief Enumeration of the window styles
    ///
    ////////////////////////////////////////////////////////////
    enum
    {
        None       = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
        Titlebar   = 1 << 0, ///< Title bar + fixed border
        Resize     = 1 << 1, ///< Titlebar + resizable border + maximize button
        Close      = 1 << 2, ///< Titlebar + close button
        Fullscreen = 1 << 3, ///< Fullscreen mode (this flag and all others are mutually exclusive)

        Default = Titlebar | Resize | Close ///< Default window style
    };
}

namespace priv
{
class WindowImpl
{
private:
	std::queue<Event> m_Events;

public:
	// Destructors
	virtual ~WindowImpl();

	/*
	 * Public static methods
	 */
	static WindowImpl* Create(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings);

        static WindowImpl* Create(WindowHandle handle, const OpenGLContextSettings& settings);


	/*
	 * Public pure virtual functions
	 */
	virtual void Display() = 0;
	virtual void ProcessEvents(bool block) = 0;
	virtual void SetTitle(const std::string& title) = 0;
	virtual void Show(bool show) = 0;
	virtual void EnableKeyRepeat(bool enabled) = 0;

	/*
	 * Public methods
	 */
	// Get information for the size of the rendering
	int GetWidth() const;
	int GetHeight() const;

	// *** events methods ***
	bool GetEvent(Event& event);

protected:
	WindowImpl();

	void PushEvent(Event& event);

	/*
	 * Attributes
	 */
	// Window caracteristic
	int m_Width;
	int m_Height;
};
}
}

#endif /* WINDOWIMPL_H_ */
