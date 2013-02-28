/*
 * SimpleThread.hpp
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

#ifndef SIMPLETHREAD_HPP_
#define SIMPLETHREAD_HPP_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <features.h>
#include <vector>

#include "AbstractThread.hpp"

namespace onposix {

/**
 * \brief Class for simple thread invocation.
 *
 * This class is useful for creating threads that do not return any value.
 * For more complex cases (e.g., return value), create your own thread by
 * inheriting from class AbstractThread.
 *
 * Example of usage:
 * \code
 * void myfunction (void* arg);
 *
 * int main ()
 * {
 *	int b;
 * 	SimpleThread t (myfunction, (void*) b);
 * 	t.start();
 * 	t.waitForTermination();
 * }
 * \endcode
 */
class SimpleThread: public AbstractThread {

	void (*handler_)(void* p);
	void* arg_;

public:

	SimpleThread(void (*handler)(void* p), void* arg):
	    handler_(handler),
	    arg_(arg) {}
	virtual ~SimpleThread(){}

	/**
	 * \brief Method run on the new thread
	 *
	 * This method is run on the new thread once start()
	 * is invoked.
	 */
	inline void run(){
		handler_(arg_);
	}
};

} /* onposix */

#endif /* SIMPLETHREAD_HPP_ */
