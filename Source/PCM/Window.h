#ifndef WindowGL_H_
#define WindowGL_H_

// STL include
#include <string>

// PCM Includes
#include <PCM/Exceptions.h>
#include <PCM/WindowMode.h>
#include <PCM/OpenGLContextSettings.h>
#include <PCM/WindowImpl.h>

namespace PCM
{
class Window
{
protected:
	/*
	 * Attributes
	 */
	// To know if the windows is destroy
	priv::WindowImpl* m_Window;
public:
	/*
	 * Constructors and destructors
	 */
	Window();
	virtual ~Window();

	/*
	 * Public methods
	 */
	// Create OpenGL window
	void Create(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings = OpenGLContextSettings());
	// To know if the window is close
	bool IsOpened() const;
	void Close();

	// Events
	bool PoolEvent(Event& event);

	/*
	 * pure virtual public methods
	 */
	void Display();
};
}

#endif /* ABSTRACTWINDOW_H_ */