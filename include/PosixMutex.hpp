/*
 * PosixMutex.hpp
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

#ifndef POSIXMUTEX_HPP_
#define POSIXMUTEX_HPP_

#include <pthread.h>

namespace onposix {

/**
 * \brief Implementation of a mutex class.
 *
 * The class is non copyable and makes use of the pthread library.
 */
class PosixMutex {

	PosixMutex(const PosixMutex&);
	PosixMutex& operator=(const PosixMutex&);

	pthread_mutex_t mutex_;

	friend class PosixCondition;

public:
	PosixMutex();
	~PosixMutex();

	/**
	 * \brief Acquires the lock.
	 *
	 * If the mutex is busy the calling thread is blocked.
	 */
	void lock() {
		pthread_mutex_lock(&mutex_);
	}

	/**
	 * \brief Releases the lock.
	 */
	void unlock() {
		pthread_mutex_unlock(&mutex_);
	}

	bool tryLock();
};

/**
 * \brief Class to simplify locking and unlocking of PosixMutex
 *
 * This is a convenience class that simplifies locking and unlocking
 * PosixMutex(es) making use of the RAII idiom: the lock is acquired in the
 * constructor and released in the destructor.
 */
class MutexLocker {

	PosixMutex& mutex_;

public:

	MutexLocker(PosixMutex& mutex): mutex_(mutex) {
		mutex_.lock();
	}

	~MutexLocker() {
		mutex_.unlock();
	}

};

/**
 * \brief Class to simplify locking and unlocking of pthread mutex.
 *
 * This class has the same purpose of the MutexLocker class but works
 * with pthread mutex type instead of PosixMutex class.
 */
class PthreadMutexLocker {

	pthread_mutex_t& mutex_;

public:

	PthreadMutexLocker(pthread_mutex_t& mutex): mutex_(mutex) {
		pthread_mutex_lock(&mutex_);
	}

	~PthreadMutexLocker() {
		pthread_mutex_unlock(&mutex_);
	}

};

} /* onposix */

#endif /* POSIXMUTEX_HPP_ */
