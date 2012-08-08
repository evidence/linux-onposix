/*
 * Time.cpp
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


#include "Time.hpp"

#include <stdexcept>

namespace onposix {

/**
 * \brief Default constructor.
 * It initializes the class with the current time (calling gettimeofday())
 * @exception std::runtime_error in case of error
 */
Time::Time()
{
	if (gettimeofday(&time_, NULL) != 0)
		throw std::runtime_error("Can't get current time");
}

/**
 * \brief Method to add seconds and useconds to the current value
 * @param sec Number of seconds to be added
 * @param usec Number of useconds to be added
 */
void Time::add(time_t sec, suseconds_t usec)
{
	time_.tv_usec += (usec%1000000);
	time_.tv_sec += sec + (usec - (usec%1000000))%1000000;
}

/**
 * \brief Method to set the time to a specific value
 * @param sec Number of seconds to be set
 * @param usec Number of useconds to be set
 */
void Time::set(time_t sec, suseconds_t usec)
{
	time_.tv_usec = (usec%1000000);
	time_.tv_sec = sec + (usec - (usec%1000000))%1000000;
}

/**
 * \brief Method to reset the class to the current time.
 * This method sets the time equal to the value returned by gettimeofday().
 * @exception std::runtime_error in case of error
 */
void Time::resetToCurrentTime()
{
	if (gettimeofday(&time_, NULL) != 0)
		throw std::runtime_error("Can't get current time");
}

/**
 * \brief Operator to compare two values
 */
bool Time::operator< (const Time& ref) const
{
	if (time_.tv_sec < ref.time_.tv_sec)
		return true;
	else if ((time_.tv_sec == ref.time_.tv_sec) &&
		 (time_.tv_usec < ref.time_.tv_usec))
			return true;
	return false;
}

/**
 * \brief Operator to compare two values
 */
bool Time::operator> (const Time& ref) const
{
	if (time_.tv_sec > ref.time_.tv_sec)
		return true;
	else if ((time_.tv_sec == ref.time_.tv_sec) &&
		 (time_.tv_usec > ref.time_.tv_usec))
			return true;
	return false;
}

/**
 * \brief Operator to compare two values
 * @return true if the times expressed by the classes are equal; false otherwise
 */
bool Time::operator== (const Time& ref) const
{
	if ((time_.tv_sec == ref.time_.tv_sec) &&
		(time_.tv_usec == ref.time_.tv_usec))
			return true;
	return false;
}

} /* onposix */
