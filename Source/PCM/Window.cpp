/*
 * Window.cpp
 *
 *  Created on: Apr 17, 2011
 *      Author: Beltegeuse
 */

#include <PCM/Window.h>
#include <PCM/WindowImpl.h>

namespace PCM
{

Window::Window() :
	m_Window(NULL),
	m_LastFrameTime(0),
	m_FramerateLimit(0)
{
}

Window::~Window()
{
	Close();
}

void Window::Create(const WindowMode& mode, const std::string& name,
		const OpenGLContextSettings& settings)
{
	m_LastFrameTime = 0;
	m_Window = priv::WindowImpl::Create(mode, name, settings);
}

void Window::Display()
{
	// Frame limits
	if(m_FramerateLimit > 0)
	{
		Uint32 waitTime = 1000 / m_FramerateLimit - m_Clock.GetElapsedTime();
		if(waitTime > 0)
			Sleep(waitTime);
	}

	// Update Time between two frames
	m_LastFrameTime = m_Clock.GetElapsedTime();
	m_Clock.Reset();

	if(m_Window)
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

int Window::GetWidth() const
{
	if (m_Window)
		m_Window->GetWidth();
	return 0;
}

int Window::GetHeight() const
{
	if(m_Window)
		m_Window->GetHeight();
	return 0;
}

Uint32 Window::GetFrameTime() const
{
	return m_LastFrameTime;
}

void Window::SetTitle(const std::string& title)
{
	if(m_Window)
		m_Window->SetTitle(title);
}

void Window::Show(bool show)
{
	if(m_Window)
		m_Window->Show(show);
}


} // Namespace PCM
