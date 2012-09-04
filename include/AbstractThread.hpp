/*
 * AbstractThread.hpp
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

#ifndef ABSTRACTTHREAD_HPP_
#define ABSTRACTTHREAD_HPP_

#include <pthread.h>

namespace onposix {

/**
 * \brief Abstract for thread implementation.
 *
 * Base abstract non copyable class for thread implementation. Derived classes
 * must implement the pure virtual function run() to define their specific
 * behavior.
 * This class makes use of POSIX threads (pthreads).
 * The thread starts stopped. To start it, the start() method must be
 * explicitly called.
 *
 * Example of usage:
 * \code
 * class MyThread: public AbstractThread {
 * 	// Put thread arguments here
 * public:
 *	MyThread() {
 *		// Initialize arguments here
 *	}
 * 	void run() {
 * 		// Put thread code here
 * 		// Access arguments as normal attributes
 * 	}
 * };
 *
 * int main ()
 * {
 * 	MyThread t;
 * 	t.start();
 * 	t.waitForTermination();
 * }
 * \endcode
 */
class AbstractThread {

	static void* Execute(void* param);

	/**
	 * \brief If the thread is running
	 */
	bool isStarted_;

	AbstractThread(const AbstractThread&);
	AbstractThread& operator=(const AbstractThread&);

protected:

	/**
	 * \brief Function to know if the thread has finished computation.
	 * This function can be used in the subclasses to check if a request
	 * for termination has been made; if so, the thread is terminated.
	 */
	static void checkForTermination() {
		pthread_testcancel();
	}

	/**
	 * \brief This function must be reimplemented in a derived class
	 * to define the specific function run by the thread.
	 */
	virtual void run() = 0;

	pthread_t handle_;

public:

	AbstractThread();
	virtual ~AbstractThread();
	bool start();
	bool stop();
	bool waitForTermination();
	static bool blockSignal (int sig);
	static bool unblockSignal (int sig);
	bool sendSignal(int sig);
	static bool setSignalHandler(int sig, void (*handler) (int));

};

} /* onposix */

#endif /* ABSTRACTTHREAD_HPP_ */