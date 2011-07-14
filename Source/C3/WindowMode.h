#ifndef WINDOWMODE_H_
#define WINDOWMODE_H_

namespace C3
{

struct WindowMode
{
        /**
         * \brief WindowMode constructor
         *
         * \param VWidth Width resolution of the window
         * \param VHeight Height resolution of the window
         * \param fullScreen Specify if the diplay must be displayed in full screen
         * \param bits Number of bits per pixel
         */
	WindowMode(int VWidth, int VHeight, bool fullScreen = false, int bits = 32) :
		Width(VWidth), Height(VHeight), Fullscreen(fullScreen),
				BitsPerPixels(bits)
	{
	}
	int Width;
	int Height;
	bool Fullscreen;
	int BitsPerPixels;
};

}

#endif /* WINDOWMODE_H_ */
