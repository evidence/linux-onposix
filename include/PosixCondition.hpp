/*
 * PosixCondition.hpp
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

#ifndef POSIXCONDITION_HPP_
#define POSIXCONDITION_HPP_

#include "PosixMutex.hpp"
#include "Time.hpp"


#include <pthread.h>
#include <time.h>

namespace onposix {

/**
 * \brief Implementation of a condition variable.
 *
 * The class is non copyable and makes use of the pthread library.
 */
class PosixCondition {

	PosixCondition(const PosixMutex&);
	PosixCondition& operator=(const PosixMutex&);

	pthread_cond_t cond_;

public:
	PosixCondition();
	~PosixCondition();

	/**
	 * \brief Blocks the calling thread on the condition variable.
	 *
	 * These functions atomically release mutex and cause the calling thread
	 * to block on the condition variable.
	 * @param m Mutex released when blocking and acquired when unblocking.
	 * @return 0 in case of success
	 */
	int wait(PosixMutex* m) {
		return pthread_cond_wait(&cond_, &(m->mutex_));
	}

	/**
	 * \brief Blocks the calling thread on the condition variable.
	 *
	 * These functions atomically release mutex and cause the calling thread
	 * to block on the condition variable.
	 * @param m Mutex released when blocking and acquired when unblocking.
	 * @param abstime Absolute time for timeout
	 * @return 0 in case of success, ETIMEDOUT in case of timeout
	 */
	int timedWait(PosixMutex* m, const Time& abstime) {
		timespec ts;
		ts.tv_sec = abstime.getSeconds();
		ts.tv_nsec = abstime.getNSeconds();
		return pthread_cond_timedwait(&cond_, &(m->mutex_), &ts);
	}

	/**
	 * \brief Unblocks at least one of the blocked threads.
	 *
	 * @return 0 in case of success
	 */
	int signal() {
		return pthread_cond_signal(&cond_);
	}

	/**
	 * \brief Unblocks all blocked threads.
	 *
	 * @return 0 in case of success
	 */
	int signalAll() {
		return pthread_cond_broadcast(&cond_);
	}
};



} /* onposix */

#endif /* POSIXCONDITION_HPP_ */
