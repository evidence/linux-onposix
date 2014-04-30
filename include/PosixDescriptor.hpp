/*
 * Descriptor.hpp
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

#ifndef POSIXDESCRIPTOR_HPP_
#define POSIXDESCRIPTOR_HPP_

#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <queue>

#include "Logger.hpp"
#include "Buffer.hpp"
#include "AbstractThread.hpp"
#include "PosixMutex.hpp"
#include "PosixCondition.hpp"

// Uncomment to enable Linux-specific methods:
#define ONPOSIX_LINUX_SPECIFIC

namespace onposix {

/**
 * \brief Abstraction of a POSIX descriptor.
 * 
 * This is an abstract class for the concept of Posix descriptor.
 * The descriptor can correspond to a file (class FileDescriptor)
 * or to a socket (class StreamSocketServerDescriptor).
 */
class PosixDescriptor {

	/**
	 * \brief Single asynchronous operation
	 *
	 * This data structure contains information about a single pending
	 * asynchronous operation.
	 */
	struct job {
		/**
		 * \brief Type of scheduled async operation
		 */
		enum {
			NONE		= 0, //< No operation scheduled
			READ_BUFFER	= 1, //< Read operation on a Buffer
			READ_VOID	= 2, //< Read operation on a void*
			WRITE_BUFFER	= 3, //< Write operation on a Buffer
			WRITE_VOID	= 4  //< Write operation on a void*
		} job_type_;

		/// Size of data to be read/written
		size_t size_;

		/**
		 * \brief Handler in case of read/write operation on a
		 * Buffer
		 */
		void (*buff_handler_) (Buffer* b, size_t size);

		/**
		 * \brief Buffer in case of read/write operation on a Buffer
		 */
		Buffer* buff_buffer_;

		/**
		 * \brief Handler in case of read/write operation on a void*
		 */
		void (*void_handler_) (void* b, size_t size);

		/**
		 * \brief void* pointer in case of read/write operation on a
		 * void*
		 */
		void* void_buffer_;
	};

	/**
	 * \brief Class for synchronization between the main thread and the worker thread.
	 *
	 * This data structure is in charge of keeping a queue of pending asynchronous operations
	 * (shared between the main thread and the worker thread) and synchronize the two threads.
	 */
	class shared_queue {

		/**
		 * \brief Queue of all pending operations.
		 */
		std::queue<job*> queue_;

		/**
		 * \brief Signal the worker to not block anymore.
		 *
		 * This variable signals the worker thread that must flush all
		 * pending operations and not block on the condition variable
		 * because the descriptor is going to be closed.
		 */
		bool flush_and_close_;

		/**
		 * \brief Mutex to avoid contentions.
		 *
		 * This mutex protects accesses to queue_ and flush_and_close_
		 * when asynchronous operations are scheduled.
		 */
		PosixMutex lock_;

		/**
		 * \brief Condition for not empty queue.
		 *
		 * This condition signals the worker thread that there is
		 * work to be carried out (i.e., the queue is not empty).
		 */
		PosixCondition cond_not_empty_;

		/**
		 * \brief Condition for empty queue.
		 *
		 * This condition signals the main thread that the queue is
		 * empty. Used by the worker thread to signal the main
		 * thread when the descriptor is going to be closed.
		 */
		PosixCondition cond_empty_;

	public:
		/// Constructor
		shared_queue(): flush_and_close_(false) {}

		/**
		 * \brief Add an asynchronous operation
		 *
		 * @param j asynchronous operation
		 */
		inline void push(struct job* j){
			lock_.lock();
			queue_.push(j);
			cond_not_empty_.signal();
			lock_.unlock();
		}

		/**
		 * \brief Signal the worker that there are new operations
		 *
		 * This method is used by the main thread to signal the worker
		 * that there are new operations in the queue.
		 */
		inline void signal_not_empty(){
			lock_.lock();
			cond_not_empty_.signal();
			lock_.unlock();
		}

		/**
		 * \brief Signal the main thread that the queue is empty
		 *
		 * This method is used by the worker to signal the main thread
		 * that the queue is empty and all pending operations have
		 * been carried out.
		 */
		inline void signal_empty(){
			lock_.lock();
			cond_empty_.signal();
			lock_.unlock();
		}

		/**
		 * \brief Wait until there are new operations
		 *
		 * This method is used by the worker to wait until there are
		 * new operations in the queue.
		 */
		inline void wait_not_empty(){
			lock_.lock();
			cond_not_empty_.wait(&lock_);
			lock_.unlock();
		}

		/**
		 * \brief Wait until the queue is empty
		 *
		 * This method is used by the main thread to wait until the
		 * queue is empty to close the descriptor.
		 */
		inline void wait_empty(){
			lock_.lock();
			cond_empty_.wait(&lock_);
			lock_.unlock();
		}

		/**
		 * \brief Signal that the descriptor is going to be close
		 *
		 * This method is used to let the main thread signal the worker
		 * that the descriptor is going to be closed, so it must flush
		 * all pending operations and not block on wait_not_empty()
		 * anymore.
		 */
		inline void set_flush_and_close(){
			lock_.lock();
			flush_and_close_ = true;
			lock_.unlock();
		}

		/**
		 * \brief Pop the next operation from the queue.
		 *
		 * This method is used by the worker to pop the next
		 * operation from the queue.
		 * @param close Pointer to a boolean used as return variable to
		 * tell the worker if the descriptor is going to be closed.
		 * @return pointer to a job instance allocated in the heap; 0
		 * if the queue is empty.
		 */
		inline job* pop (bool* close){
			job* ret = 0;
			lock_.lock();
			if (!queue_.empty()){
				ret = queue_.front();
				queue_.pop();
			}
			if (queue_.empty()){
				cond_empty_.signal();
			}
			*close = flush_and_close_;
			lock_.unlock();
			return ret;
		}
	};

	/**
	 * \brief Worker thread to perform asynchronous operations.
	 *
	 * This class is used to run asynchronous operations (i.e.,
	 * read and write). These operations are run on a different thread.
	 */
	class Worker: public AbstractThread {

		/// Disable the default constructor
		Worker();

		/**
		 * \brief Method automatically called by start()
		 *
		 * This method is automatically called by start() which,
		 * in turn, is called by startAsyncOperation()
		 */
		void run();

		/**
		 * \brief File descriptor
		 *
		 * This is a pointer to the same PosixDescriptor that "owns"
		 * this instance of Worker.
		 * The pointer is needed to perform the operation
		 * (i.e., read or write).
		 */
		PosixDescriptor* des_;

		/**
		 * \brief Pointer to the shared queue
		 *
		 * This variable points the shared_queue used for asynchronous
		 * operations and synchronization between the main thread
		 * and the worker thread.
		 */
		shared_queue* queue_;

	public:

		/**
		 * \brief Constructor. 
		 *
		 * It just initializes the variables.
		 * @param q Pointer to the shared_queue for synchronization and
		 * pending jobs
		 * @param des Pointer to the PosixDescriptor that "owns"
		 * this worker
		 */
		Worker(shared_queue* q, PosixDescriptor* des):
		    des_(des), queue_(q)  {}

		~Worker(){
		}

		void startAsyncOperation (bool read_operation, 
		    void (*handler) (Buffer* b, size_t size),
		    	Buffer* buff, size_t size);

		void startAsyncOperation (bool read_operation,
		    void (*handler) (void* b, size_t size),
		    	void* buff, size_t size);	
		
	};

	/**
	 * \brief Pointer to the worker that performs asynchronous operations.
	 *
	 * The worker is allocated on the heap in the constructors
	 * (2 standard + 1 copy) and deallocated in the destructor.
	 */
	Worker* worker_;

	/**
	 * \brief Pointer to the shared_queue for synchronization with the
	 * worker thread
	 *
	 * This data structure is allocated on the heap in the constructors
	 * (2 standard + 1 copy) and deallocated in the destructor.
	 */
	shared_queue* queue_;

	/**
	 * \brief If the worker thread has been already started.
	 *
	 * This variable is modified in async_read() and async_write();
	 */
	bool worker_started_;
		
	/**
	 * \brief Private constructor used by derived classes
	 *
	 * It allocates queue_ and worker_.
	 * @param fd File descriptor number returned by open(), socket(),
	 * accept(), etc.
	 */
	PosixDescriptor(int fd): queue_(0), worker_started_(false), fd_(fd) {
		queue_ = new shared_queue;
		worker_ = new Worker (queue_, this);
	}

	friend class Pipe;
	friend class AsyncThread;

protected:
	/**
	 * \brief Number of the file descriptor.
	 *
	 * This is the return value of open(), socket() or accept().
	 */
	int fd_;

	int do_read (void* p, size_t size);
	int do_write (const void* p, size_t size);

	/**
	 * \brief Constructor
	 *
	 * It allocates queue_ and worker_.
	 */
	PosixDescriptor(): queue_(0), worker_started_(false), fd_(-1) {
		queue_ = new shared_queue;
		worker_ = new Worker (queue_, this);
	}

public:
	/**
	 * \brief Destructor.
	 *
	 * It closes the file descriptor and deallocates queue_ and
	 * worker_.
	 */
	virtual ~PosixDescriptor() {
		DEBUG("Destroying descriptor...");
		DEBUG("Closing desciptor...");
		close();
		DEBUG("delete thread...");
		delete(worker_);
		delete(queue_);

		DEBUG("Descriptor succesfully destroyed. Let's move on!");
	}


	/**
	 * \brief Run asynchronous read operation
	 *
	 * This method schedules an asynchronous read operation.
	 * The operation is internally run on a different thread.
	 * @param handler Function to be run when the read operation has
	 * finished.
	 * This function will have two parameters: a pointer to the Buffer
	 * where data has been saved, and the number of bytes actually read.
	 * @param b Pointer to the Buffer to be provided to the handler
	 * function as argument
	 * @param size Number of bytes to be read
	 */
	inline void async_read(void (*handler)(Buffer* b, size_t size),
	    Buffer* b,
	    size_t size){
		DEBUG("async_read() called!");
		if (!worker_started_){
			worker_->start();
			worker_started_ = true;
		}
		worker_->startAsyncOperation(true, handler, b, size);
	}

	/**
	 * \brief Run asynchronous read operation
	 *
	 * This method schedules an asynchronous read operation.
	 * The operation is internally run on a different thread.
	 * @param handler Function to be run when the read operation has
	 * finished.
	 * This function will have two parameters: a void* where data
	 * has been saved, and the number of bytes actually read.
	 * @param b Pointer to be provided to the handler function as
	 * argument
	 * @param size Number of bytes to be read
	 */
	inline void async_read(void (*handler)(void* b, size_t size),
	    void* b,
	    size_t size){
		DEBUG("async_read() called!");
		if (!worker_started_){
			worker_->start();
			worker_started_ = true;
		}
		worker_->startAsyncOperation(true, handler, b, size);
	}
	
	/**
	 * \brief Run asynchronous write operation
	 *
	 * This method schedules an asynchronous write operation.
	 * The operation is internally run on a different thread.
	 * @param handler Function to be run when the write operation has
	 * finished.
	 * This function will have two parameters: a pointer to the Buffer
	 * where original data was stored, and the number of bytes actually
	 * written.
	 * @param b Pointer to the Buffer to be provided to the handler
	 * function as argument
	 * @param size Number of bytes to be written.
	 */
	inline void async_write(void (*handler)(Buffer* b, size_t size),
	    Buffer* b,
	    size_t size){
		if (!worker_started_){
			worker_->start();
			worker_started_ = true;
		}
		worker_->startAsyncOperation(false, handler, b, size);
	}

	/**
	 * \brief Run asynchronous write operation
	 *
	 * This method schedules an asynchronous write operation.
	 * The operation is internally run on a different thread.
	 * @param handler Function to be run when the write operation has
	 * finished.
	 * This function will have two parameters: a void* where original
	 * data was stored, and the number of bytes actually written.
	 * @param b Pointer to be provided to the handler function as
	 * argument
	 * @param size Number of bytes to be written
	 */
	inline void async_write(void (*handler)(void* b, size_t size),
	    void* b,
	    size_t size){
		if (!worker_started_){
			worker_->start();
			worker_started_ = true;
		}
		worker_->startAsyncOperation(false, handler, b, size);
	}
		
	int read (Buffer* b, size_t size);
	int read (void* p, size_t size);
	int write (Buffer* b, size_t size);
	int write (const void* p, size_t size);
	int write (const std::string& s);

	/**
	 * \brief Method to close the descriptor.
	 *
	 * Note: currently there is no method to re-open the descriptor.
	 * In case the worker thread has been started, it signals the worker
	 * that it must not block on wait anymore (through
	 * set_flush_and_close()); then it unblocks the worker (through
	 * signal_not_empty()).
	 */
	inline virtual void close(){
		if (worker_started_){
			DEBUG("Flushing pending data...")
			queue_->set_flush_and_close();
			queue_->signal_not_empty();
			queue_->wait_empty();
			worker_->waitForTermination();
		}
		::close(fd_);
	}



	/**
	 * \brief Method to get descriptor number.
	 *
	 * @return Descriptor number.
	 */
	inline int getDescriptorNumber() const {
		return fd_;
	}

	/**
	 * \brief Copy constructor.
	 *
	 * The copy constructor is called to copy an existing object to
	 * another object that is being constructed.
	 * Examples:
	 * \code
	 * PosixDescriptor p1;
	 * PosixDescriptor p2 = p1;
	 * PosixDesscriptor p3 (p1);
	 * \endcode
	 * It allocates queue_ and worker_.
	 * @exception runtime_error if the ::dup() returns an error
	 */
	PosixDescriptor(const PosixDescriptor& src): queue_(0), worker_started_(false) {
		fd_ = ::dup(src.fd_);
		if (fd_ < 0) {
			ERROR("Bad file descriptor");
			throw std::runtime_error("PosixDescriptor: error in copy constructor");
		}
		DEBUG("Creating worker (stopped)");
		queue_ = new shared_queue;
		worker_ = new Worker (queue_, this);
	}

	/**
	 * \brief Assignment operator.
	 *
	 * The assignment operator is called to copy an existing object to
	 * another object that is already existing as well.
	 * Examples:
	 * \code
	 * PosixDescriptor p1, p2;
	 * p2 = p1;
	 * \endcode
	 * @exception runtime_error if the ::dup() returns an error
	 */
	PosixDescriptor& operator= (const PosixDescriptor& src){
		if (::dup2(src.fd_, fd_) < 0) {
			ERROR("Bad file descriptor");
			throw std::runtime_error("PosixDescriptor: error in operator=");
		}
		return *this;
	}

#ifdef ONPOSIX_LINUX_SPECIFIC

	/**
	 * \brief Method to flush this specific descriptor
	 */
	inline bool flush(){
		if (syncfs(fd_) < 0)
			return false;
		else
			return true;
	}

	/**
	 * \brief Ioctl on the file descriptor
	 */
	inline int ioctl(int request){
		return ::ioctl(fd_, request);
	}

	/**
	 * \brief Ioctl on the file descriptor
	 */
	inline int ioctl(int request, void* argp){
		return ::ioctl(fd_, request, argp);
	}
#endif /* ONPOSIX_LINUX_SPECIFIC */
};


} /* onposix */

#endif /* POSIXDESCRIPTOR_HPP_ */
