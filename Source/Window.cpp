/*
 * Window.cpp
 *
 *  Created on: Apr 17, 2011
 *      Author: Beltegeuse
 */

#include <Window.h>
#include <WindowImpl.h>

namespace PCM
{

Window::Window() :
	m_Window(NULL)
{
}

Window::~Window()
{
	Close();
}

void Window::Create(const WindowMode& mode, const std::string& name,
		const OpenGLContextSettings& settings)
{
	m_Window = priv::WindowImpl::Create(mode, name, settings);
}

void Window::Display()
{
	m_Window->Display();
}

bool Window::PoolEvent(Event& event)
{
	if (!m_Window)
		return false;
	return m_Window->GetEvent(event);
}

bool Window::IsOpened() const
{
	return m_Window != NULL;
}

void Window::Close()
{
	if (m_Window)
		delete m_Window;
}

} // Namespace PCM
