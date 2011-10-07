#include <C3/WindowImpl.h>

#ifdef WIN32
#include <C3/Win32/WindowImplWin32.h>
typedef C3::priv::WindowImplWin32 WindowImplType;
#else
#include <C3/Linux/WindowImplLinux.h>
typedef C3::priv::WindowImplLinux WindowImplType;
#endif

namespace C3
{
namespace priv
{

WindowImpl::WindowImpl()
{
}

WindowImpl::~WindowImpl()
{
}

WindowImpl* WindowImpl::Create(const WindowMode& mode, const std::string& name,
		const OpenGLContextSettings& settings)
{
	return new WindowImplType(mode, name, settings);
}

WindowImpl* WindowImpl::Create(WindowHandle handle, const OpenGLContextSettings& settings)
{
        return new WindowImplType(handle,settings );
}


int WindowImpl::GetWidth() const
{
	return m_Width;
}

int WindowImpl::GetHeight() const
{
	return m_Height;
}

bool WindowImpl::GetEvent(C3::Event& event)
{
	if (m_Events.empty())
	{
		ProcessEvents(false);
		return false;
	}
	event = m_Events.front();
	m_Events.pop();
	return true;
}

void WindowImpl::PushEvent(Event& event)
{
	m_Events.push(event);
}

}
}
