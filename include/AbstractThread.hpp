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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <features.h>

#include <vector>

// Uncomment to enable Linux-specific methods:
#define ONPOSIX_LINUX_SPECIFIC

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
 *
 * In case a return value must be passed from the finished thread to the
 * other thread, this can be achieved as follows:
 *
 * \code
 * class MyThread: public AbstractThread {
 * 	int returnValue_;
 * public:
 *	MyThread(): returnValue_(0) {
 *		// Initialize other arguments here
 *	}
 * 	void run() {
 * 		// Put thread code here
 * 		returnValue_ = ...;
 * 	}
 * 	inline int getReturnValue() {
 *		return returnValue_;
 *	}
 * };
 *
 * int main ()
 * {
 * 	MyThread t;
 * 	t.start();
 * 	t.waitForTermination();
 *	// Attention: get return arguments only after the thread has
 *	// terminated the execution, to avoid race conditions!
 * 	int ret = t.getReturnValue();
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
	 *
	 * This function can be used in the subclasses to check if a request
	 * for termination has been made; if so, the thread is terminated.
	 */
	static void checkTermination() {
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
	bool setSchedParam(int policy, int priority);
	bool getSchedParam(int* policy, int* priority);

#if defined(ONPOSIX_LINUX_SPECIFIC) && defined(__GLIBC__) && \
    ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ > 3)))
	// Functions to get/set affinity are available only from glibc 2.4
	void setAffinity(const std::vector<bool>& s);
	void getAffinity(std::vector<bool>* v);
#endif /* ONPOSIX_LINUX_SPECIFIC && GLIBC */
};

} /* onposix */

#endif /* ABSTRACTTHREAD_HPP_ */
