/*
 * WindowImplLinux.h
 *
 *  Created on: Jun 25, 2011
 *      Author: adrien
 */

#ifndef WINDOWIMPLLINUX_H_
#define WINDOWIMPLLINUX_H_

// PCM Includes
#include <WindowImpl.h>

namespace PCM
{
namespace priv
{

class WindowImplLinux : public WindowImpl
{
public:
	/*
	 * Constructors and destructors
	 */
	WindowImplLinux(const WindowMode& mode, const std::string& name,
			const OpenGLContextSettings& settings);
	virtual ~WindowImplLinux();

	/*
	 * Virtual methods
	 */
	virtual void Display();
	virtual void ProcessEvents(bool block);
};

// Namespaces ends
}
}

#endif /* WINDOWIMPLLINUX_H_ */
