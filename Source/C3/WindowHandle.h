#ifndef WINDOWHANDLE_H
#define WINDOWHANDLE_H

// Windows' HWND is a typedef on struct HWND__*
#ifdef WIN32
struct HWND__;
#endif

namespace C3
{
////////////////////////////////////////////////////////////
/// Define a low-level window handle type, specific to
/// each platform
////////////////////////////////////////////////////////////
#ifdef WIN32

// Window handle is HWND (HWND__*) on Windows
typedef HWND__* WindowHandle;

#else // For linux

// Window handle is Window (unsigned long) on Unix - X11
typedef unsigned long WindowHandle;

//#elif defined(SYSTEM_MACOS)

//// Window handle is NSWindow (void*) on Mac OS X - Cocoa
//typedef void* WindowHandle;

#endif

} // namespace C3

#endif // WINDOWHANDLE_H
