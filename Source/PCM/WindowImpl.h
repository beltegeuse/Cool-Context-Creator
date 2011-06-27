/*
 * WindowImpl.h
 *
 *  Created on: Apr 17, 2011
 *      Author: Beltegeuse
 */

#ifndef WINDOWIMPL_H_
#define WINDOWIMPL_H_

// STL includes
#include <string>
#include <queue>

// PCM includes
#include <PCM/WindowMode.h>
#include <PCM/OpenGLContextSettings.h>
#include <PCM/Events.h>

namespace PCM
{
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

	/*
	 * Public pure virtual functions
	 */
	virtual void Display() = 0;
	virtual void ProcessEvents(bool block) = 0;
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
