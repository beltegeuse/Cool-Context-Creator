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
void Sleep(Uint32 time);

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
        /**
         * \brief Clock constructor
         */
	Clock();

        /**
         * \brief Clock destructor
         */
	virtual ~Clock();

	/*
	 * Public methods
	 */
        /**
         * Reset the clock.
         */
	void Reset();

        /**
         * Return the time elapsed since the last reset or creation of the clock.
         *
         * \return The time elapsed in seconds (micro ? TODO: be sure)
         */
	Uint32 GetElapsedTime() const;

private:
	/*
	 * Private methods
	 */
        /**
         * Returns the current time, expressed in seconds (micro ? TODO: be sure) since the Epoch.
         *
         * \return Current time in seconds
         */
	Uint64 GetSystemClock() const;
};
}
#endif /* CLOCK_H_ */
