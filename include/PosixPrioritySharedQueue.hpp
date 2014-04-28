#ifndef POSIXPRIORITYSHAREDQUEUE_HPP_
#define POSIXPRIORITYSHAREDQUEUE_HPP_

#include <pthread.h>
#include <map>
#include <queue>
#include <assert.h>
#include "PosixMutex.hpp"

namespace onposix {

/** 
 * \brief Thread-safe FIFO priority queue class.
 *
 * No aging techniques are implemented, so that the low priority elements
 * can starve. The class is noncopyable. \n
 * The template paramaters are:
 * <ul>
 *		<li> T	is the type of the element in the queue
 *		<li> _Priority	is the priority type (shall support the less operator)
 * </ul>
 */
template<typename T, typename _Priority = int>
class PosixPrioritySharedQueue {

	std::map< _Priority, std::queue<T> > queues_;
	pthread_cond_t empty_;
	mutable pthread_mutex_t mutex_;
	size_t globalSize_;

	PosixPrioritySharedQueue(const PosixPrioritySharedQueue&);
	PosixPrioritySharedQueue& operator=(const PosixPrioritySharedQueue&);

public:
	PosixPrioritySharedQueue();
	~PosixPrioritySharedQueue();

	void addQueue(const _Priority& prio);

	void push(const T& data, const _Priority& prio);

	T pop();

	void clear();


	size_t size() const;
};

/**
 * \brief Constructor. Initialize the queue.
 *
 * @exception runtime_error if the initialization fails.
 */
template<typename T, typename _Priority>
PosixPrioritySharedQueue<T, _Priority>::PosixPrioritySharedQueue():
	globalSize_(0)
{
	if (pthread_mutex_init(&mutex_, NULL) != 0)
		throw std::runtime_error(std::string("Mutex initialization: ") +
								 strerror(errno));
	if (pthread_cond_init(&empty_, NULL) != 0)
		throw std::runtime_error(
				std::string("Condition variable initialization: ") +
				strerror(errno));
}

/**
 * \brief Destructor. Clean up the resources.
 */
template<typename T, typename _Priority>
PosixPrioritySharedQueue<T, _Priority>::~PosixPrioritySharedQueue()
{
	VERIFY_ASSERTION(!pthread_mutex_destroy(&mutex_));
	VERIFY_ASSERTION(!pthread_cond_destroy(&empty_));
}

/** 
 * \brief Adds new priority. 
 *
 * If the priority already exists does nothing.
 * \param prio	The priority to be added.
 */
template<typename T, typename _Priority>
void PosixPrioritySharedQueue<T, _Priority>::addQueue(const _Priority &prio)
{
	PthreadMutexLocker lock(mutex_);
	if (queues_.find(prio) == queues_.end())
		queues_.insert(std::make_pair<_Priority, std::queue<T> >(
						   prio, std::queue<T>()));
}

/** 
 * \brief Insert an new element in the queue.
 *
 * @param data	The element to be added.
 * @param prio	The priority of the element.
 */
template<typename T, typename _Priority>
void PosixPrioritySharedQueue<T, _Priority>::push(const T& data,
				const _Priority& prio)
{
	pthread_mutex_lock(&mutex_);
	if (queues_.find(prio) != queues_.end()){
		queues_[prio].push(data);
		++globalSize_;
		pthread_mutex_unlock(&mutex_);
		pthread_cond_signal(&empty_);
		return;
	}
	pthread_mutex_unlock(&mutex_);
}

/** 
 * \brief Extract an element from the queue.
 * 
 * If the queue is empty the calling thread is blocked.
 * @return The first of the highest priority element in the queue.
 */
template<typename T, typename _Priority>
T PosixPrioritySharedQueue<T, _Priority>::pop()
{
	PthreadMutexLocker lock(mutex_);
	while (!globalSize_)
		pthread_cond_wait(&empty_, &mutex_);
	typename std::map<_Priority, std::queue<T> >::iterator it = queues_.begin();
	typename std::map<_Priority, std::queue<T> >::iterator itEnd =
			queues_.end();
	for(; it != itEnd; ++it){
		if (!(it->second).empty()){
			T data = (it->second).front();
			(it->second).pop();
			--globalSize_;
			return data;
		}
	}
	assert(false);
	//Just to silent the compiler
	T data;
	return data;
}

/**
 * \brief Empties the queue. 
 *
 * In order to efficiently accomplish its task,
 * this function exchanges its content with an empty queue using the specialized
 * version of swap() implemented for the STL container std::map.
 */
template<typename T, typename _Priority>
void PosixPrioritySharedQueue<T, _Priority>::clear()
{
	PthreadMutexLocker lock(mutex_);
	std::map<_Priority, std::queue<T> > empty;
	std::swap(queues_, empty);
	globalSize_ = 0;
}

/** 
 * \brief The current size of the queue.
 *
 * @return The queue size.
 */
template<typename T, typename _Priority>
size_t PosixPrioritySharedQueue<T, _Priority>::size() const
{
	PthreadMutexLocker lock(mutex_);
	return globalSize_;
}

} /* onposix */

#endif /* POSIXPRIORITYSHAREDQUEUE_HPP_ */
