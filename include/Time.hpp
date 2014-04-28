/*
 * Time.hpp
 *
 * Copyright (C) 2012 Evidence Srl - www.evidence.eu.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef TIME_HPP_
#define TIME_HPP_

#include <time.h>

namespace onposix {

/**
 * \brief Class to contain a time
 *
 * This class wraps a time, with a resolution of nanoseconds.
 * It is useful to get the current time and to make comparisons between times.
 */
class Time
{
	/**
	 * \brief Time in the form returned by gettimeofday()
	 */
	struct timespec time_;

	/**
	 * Type of clock.
	 */
	clockid_t clockType_;
public:
	Time(clockid_t clockType = CLOCK_MONOTONIC);
	virtual ~Time(){}
	void add(time_t sec, long nsec);
	void set(time_t sec, long nsec);
	void resetToCurrentTime();
	bool operator< (const Time& ref) const;
	bool operator> (const Time& ref) const;
	bool operator== (const Time& ref) const;
	void getResolution (time_t* sec, long* nsec);


	/**
	 * \brief Method to get the number of seconds
	 *
	 * @return Number of seconds
	 */
	inline time_t getSeconds() const {
		return time_.tv_sec;
	}

	/**
	 * \brief Method to get the number of microseconds
	 *
	 * @return Number of microseconds
	 */
	inline long getNSeconds() const {
		return time_.tv_nsec;
	}
};

} /* onposix */

#endif /* TIME_HPP_ */
