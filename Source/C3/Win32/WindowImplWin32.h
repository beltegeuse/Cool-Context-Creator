#ifndef WINDOWIMPLWIN32_H_
#define WINDOWIMPLWIN32_H_

#include <C3/WindowImpl.h>
#include <windows.h>

namespace C3
{
namespace priv
{

class WindowImplWin32: public WindowImpl
{
private:
	/*
	 * Attributes
	 */
	HWND m_Handle; ///< Win32 handle of the window
	HINSTANCE m_hInstance;
	HDC m_DeviceContext;
	HGLRC m_OpenGLContext;
	bool m_IsCursorIn;
	bool m_KeyRepeatEnabled;
public:
	/*
	 * Constructors and Destructors
	 */
	WindowImplWin32(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings);
	WindowImplWin32(WindowHandle handle, const OpenGLContextSettings& settings);

	virtual ~WindowImplWin32();

	/*
	 * Virtual functions
	 */
	virtual void ProcessEvents(bool block);
	virtual void Display();
	virtual void Show(bool show);
	virtual void SetTitle(const std::string& title);
	virtual void EnableKeyRepeat(bool enabled);

private:
	/*
	 * Private methods
	 */
	// Process Events
	static LRESULT CALLBACK GlobalOnEvent(HWND handle, UINT message,
			WPARAM wParam, LPARAM lParam);
	void ProcessEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void DestroyOpenGLWindow();
	void InitializeOpenGL(const OpenGLContextSettings& settings, int bpp = -1);
	Key::Code VirtualKeyCodeToSF(WPARAM key, LPARAM flags);
};

}
}

#endif /* WINDOWIMPLWIN32_H_ */
