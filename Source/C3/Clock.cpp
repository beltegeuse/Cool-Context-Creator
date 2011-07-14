#include <C3/Clock.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

namespace C3 {
void Sleep(Uint32 time)
{
#ifdef WIN32
	::Sleep(time);
#else
	usleep(time * 1000);
#endif
}

Clock::Clock()
{
	Reset();
}

Clock::~Clock()
{
}

void Clock::Reset()
{
	m_Time = GetSystemClock();
}

Uint32 Clock::GetElapsedTime() const
{
	 return static_cast<Uint32>(GetSystemClock() - m_Time);
}

Uint64 Clock::GetSystemClock() const
{
#ifdef WIN32
	static LARGE_INTEGER frequency;
	static BOOL useHighPerformanceTimer = QueryPerformanceFrequency(&frequency);

	if (useHighPerformanceTimer)
	{
		// High performance counter available : use it
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		return currentTime.QuadPart * 1000 / frequency.QuadPart;
	}
	else
	{
		// High performance counter not available: use GetTickCount (less accurate)
		return GetTickCount();
	}
#else
	 timeval time = {0, 0};
	 gettimeofday(&time, 0);

	 return time.tv_sec * 1000 + time.tv_usec / 1000;
#endif
}

}
