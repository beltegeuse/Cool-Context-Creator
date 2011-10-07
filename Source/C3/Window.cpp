#include <C3/Window.h>
#include <C3/WindowImpl.h>

namespace C3
{

Window::Window() :
	m_Window(0),
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

void Window::Create(WindowHandle handle, const OpenGLContextSettings& settings)
{
    // Destroy the previous window implementation
    Close();
    m_LastFrameTime = 0;
    // Recreate the window implementation
    m_Window = priv::WindowImpl::Create(handle, settings);

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

void Window::EnableKeyRepeat(bool enabled)
{
	if(m_Window)
		m_Window->EnableKeyRepeat(enabled);
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
	{
		delete m_Window;
		m_Window = 0;
	}
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
