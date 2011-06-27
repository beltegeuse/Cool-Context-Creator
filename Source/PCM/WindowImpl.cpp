#include "WindowImpl.h"

#ifdef WIN32
#include <PCM/Win32/WindowImplWin32.h>
typedef PCM::priv::WindowImplWin32 WindowImplType;
#else
#include <PCM/Linux/WindowImplLinux.h>
typedef PCM::priv::WindowImplLinux WindowImplType;
#endif

namespace PCM
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

int WindowImpl::GetWidth() const
{
	return m_Width;
}

int WindowImpl::GetHeight() const
{
	return m_Height;
}

bool WindowImpl::GetEvent(PCM::Event& event)
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
