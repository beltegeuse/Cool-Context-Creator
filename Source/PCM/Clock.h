/*
 * Clock.h
 *
 *  Created on: Jun 27, 2011
 *      Author: adrien
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include <PCM/Config.h>

namespace PCM
{
class Clock {
private:
	/*
	 * Attributes
	 */
	Uint64 m_Time;
public:
	/*
	 * Constructors & Destructors
	 */
	Clock();
	virtual ~Clock();

	/*
	 * Public methods
	 */
	void Reset();
	Uint32 GetElapsedTime() const;

private:
	Uint64 GetSystemClock() const;
};
}
#endif /* CLOCK_H_ */
