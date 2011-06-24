#include "WindowImpl.h"

#ifdef WIN32
#include <Win32/WindowImplWin32.h>
typedef PCM::priv::WindowImplWin32 WindowImplType;
#else
#endif

namespace PCM
{
namespace priv
{

WindowImpl* WindowImpl::Create(const WindowMode& mode, const std::string& name,
		const OpenGLContextSettings& settings)
{
	return new WindowImplType(mode, name, settings);
}

}
}
