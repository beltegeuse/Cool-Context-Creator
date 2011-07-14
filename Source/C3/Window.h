#ifndef WindowGL_H_
#define WindowGL_H_

// STL include
#include <string>

// C3 Includes
#include <C3/Exceptions.h>
#include <C3/WindowMode.h>
#include <C3/OpenGLContextSettings.h>
#include <C3/WindowImpl.h>
#include <C3/Clock.h>

namespace C3
{
class Window
{
protected:
	/*
	 * Attributes
	 */
	// To know if the windows is destroy
	priv::WindowImpl* m_Window;
	Uint32 m_LastFrameTime;
	Clock m_Clock;
	Uint32 m_FramerateLimit;

public:
	/*
	 * Constructors and destructors
	 */
        /**
         * \brief Window constructor
         */
	Window();
        /**
         * \brief Window destructor
         */
	virtual ~Window();

	/*
	 * Public methods
	 */
	// Getters
        /**
         * Returns the width of the window
         *
         * \return Width of the window
         */
	int GetWidth() const;
        /**
         * Returns the height of the window
         *
         * \return Height of the window
         */
	int GetHeight() const;

        /**
         * Returns the time elapsed since the creation of the window and the last frame displayed.
         *
         * \return Time elapsed in milliseconds
         */
	Uint32 GetFrameTime() const;

	// Setters
        /**
         * Changes the window title to the one specified
         *
         * \param title New title
         */
	void SetTitle(const std::string& title);

        /**
         * Specifies if the window is visible or not
         *
         * \param show If true, the window will be visible
         */
	void Show(bool show);

        /**
         * Force a constant number of frame displayed per second
         *
         * \param fps Number of frame per second
         */
	void SetFrameLimit(Uint32 fps);


       /**
        * Create an OpenGL window
        *
        * \param mode Specifies the dimensions of the window, as well as the bits per pixel
        * \param name Title of the window
        * \param settings Specifies the desired OpenGL version, the depth bits and the stentil bits
        */
	void Create(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings = OpenGLContextSettings());
	// To know if the window is close
        /**
         * In order to know if the window is opened
         *
         * \return True if the window is opened
         */
	bool IsOpened() const;

        /**
         * Close the window
         */
	void Close();

	// Events
	bool PoolEvent(Event& event);

	/*
	 * pure virtual public methods
	 */
        /**
         * Update the frame displayed in the window
         */
	void Display();
};
}

#endif /* ABSTRACTWINDOW_H_ */
