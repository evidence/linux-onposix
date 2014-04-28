/*
 * PosixSharedQueue.hpp
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

#ifndef POSIXSHAREDQUEUE_HPP_
#define POSIXSHAREDQUEUE_HPP_

#include <pthread.h>
#include <queue>
#include <stdexcept>
#include <errno.h>
#include <string.h>
#include "PosixMutex.hpp"
#include "Assert.hpp"

namespace onposix {

/**
 * \brief Implementation of a thread safe FIFO queue class.
 *
 * The template parameter is the type of the elements contained in the queue.
 * The class is non copyable and makes use of POSIX threads (pthreads).
 */
template<typename T>
class PosixSharedQueue {

	std::queue<T> queue_;
	pthread_cond_t empty_;
	mutable pthread_mutex_t mutex_;

	PosixSharedQueue(const PosixSharedQueue&);
	PosixSharedQueue& operator=(const PosixSharedQueue&);

public:
	PosixSharedQueue();
	~PosixSharedQueue();

	void push(const T& data);

	T pop();

	void clear();

	size_t size() const;
};

/**
 * \brief Constructor. Initialize the queue.
 *
 * @exception runtime_error if the initialization fails.
 */
template<typename T>
PosixSharedQueue<T>::PosixSharedQueue()
{
	if (pthread_mutex_init(&mutex_, NULL) != 0)
		throw std::runtime_error(std::string("Mutex initialization: ") +
								 strerror(errno));
	if (pthread_cond_init(&empty_, NULL) != 0)
		throw std::runtime_error(std::string("Condition variable initialization: ") +
								 strerror(errno));
}

/**
 * \brief Destructor. Clean up the resources.
 */
template<typename T>
PosixSharedQueue<T>::~PosixSharedQueue()
{
	VERIFY_ASSERTION(!pthread_mutex_destroy(&mutex_));
	VERIFY_ASSERTION(!pthread_cond_destroy(&empty_));
}

/**
 * \brief Inserts an element in the queue
 *
 * @param data	The element to be added in the queue.
 */
template<typename T>
void PosixSharedQueue<T>::push(const T &data)
{
	{
		PthreadMutexLocker lock(mutex_);
		queue_.push(data);
	}
	pthread_cond_signal(&empty_);
}

/**
 * \brief Extracts an element from the queue.
 *
 * Blocks the calling thread if the queue is empty.
 * @return The first element in the queue.
 */
template<typename T>
T PosixSharedQueue<T>::pop()
{
	PthreadMutexLocker lock(mutex_);
	while (queue_.empty())
		if (pthread_cond_wait(&empty_, &mutex_) != 0)
			throw std::runtime_error(std::string("Condition variable wait: ") +
									 strerror(errno));
	T data = queue_.front();
	queue_.pop();
	return data;
}

/**
 * \brief Empties the queue. 
 *
 * In order to efficiently accomplish its task,
 * this function exchanges its content with an empty queue using the specialized
 * version of swap() implemented for the STL container std::queue.
 */
template<typename T>
void PosixSharedQueue<T>::clear()
{
	PthreadMutexLocker lock(mutex_);
	std::queue<T> empty;
	std::swap(queue_, empty);
}

/** \brief The current size of the queue.
 *
  * @return The queue size.
  */
template<typename T>
size_t PosixSharedQueue<T>::size() const
{
	PthreadMutexLocker lock(mutex_);
	return queue_.size();
}

} /* onposix */

#endif /* POSIXSHAREDQUEUE_HPP_ */
