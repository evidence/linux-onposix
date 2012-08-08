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

#include <sys/time.h>

namespace onposix {

/**
 * \brief Class to contain a time
 * This class wraps a time, with a resolution of microseconds.
 * It is useful to get the current time and to make comparisons between times.
 */
class Time
{
	/**
	 * \brief Time in the form returned by gettimeofday()
	 */
	struct timeval time_;
public:
	Time();
	virtual ~Time(){}
	void add(time_t sec, suseconds_t usec);
	void set(time_t sec, suseconds_t usec);
	void resetToCurrentTime();
	bool operator< (const Time& ref) const;
	bool operator> (const Time& ref) const;
	bool operator== (const Time& ref) const;

	/**
	 * \brief Method to get the number of seconds
	 * @return Number of seconds
	 */
	inline unsigned int getSeconds() const {
		return time_.tv_sec;
	}

	/**
	 * \brief Method to get the number of microseconds
	 * @return Number of microseconds
	 */
	inline unsigned int getUSeconds() const {
		return time_.tv_usec;
	}
};

} /* onposix */

#endif /* TIME_HPP_ */
