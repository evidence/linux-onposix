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
	 * \brief Class to run asynchronous operations.
	 *
	 * This class is used to run asynchronous operations (i.e.,
	 * read and write). operations are run on a different thread.
	 */
	class Worker: public AbstractThread {

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
		 * \brief Queue of pending operations.
		 */
		std::queue<job*> jobs_;  
		
		/**
		 * \brief Mutex to avoid contentions in accessing jobs_
		 *
		 * This mutex allows to avoid contentions on the queue of jobs
		 * when asynchronous operations are scheduled.
		 */
		PosixMutex jobs_lock_;

		PosixCondition wait_completion_;
		PosixCondition wait_new_operation_;

		// Disable default constructor
		Worker();

		/**
		 * \brief Thread method automatically called by start()
		 *
		 * This method is automatically called by start() which,
		 * in turn, is called by startAsyncOperation()
		 */
		void run();


		/**
		 * \brief File descriptor
		 *
		 * This is a pointer to the same file descriptor that "owns"
		 * the instance of AsyncThread.
		 * The pointer is needed to perform the operation
		 * (i.e., read or write).
		 */
		PosixDescriptor* des_;

	public:

		/**
		 * \brief Constructor. 
		 *
		 * It just initializes variables.
		 * @param des Pointer to the PosixDescriptor that "owns"
		 * this instance of AsyncThread
		 */
		explicit Worker(PosixDescriptor* des):
		    des_(des) {}

		~Worker(){
			jobs_lock_.unlock();
		}

		void startAsyncOperation (bool read_operation, 
		    void (*handler) (Buffer* b, size_t size),
		    	Buffer* buff, size_t size);
		void startAsyncOperation (bool read_operation,
		    void (*handler) (void* b, size_t size),
		    	void* buff, size_t size);
		
		void waitCompletion(){
			jobs_lock_.lock();
			if (!jobs_.empty())
				wait_completion_.wait(&jobs_lock_);
		}
		
		
	};

	Worker* worker_;

	bool worker_started_;
		
	PosixDescriptor(int fd): worker_(0), worker_started_(false), fd_(fd) {
		DEBUG("Descriptor created");
		DEBUG("Creating worker (stopped)");
		worker_ = new Worker (this);
	}

	friend class Pipe;
	friend class AsyncThread;

protected:
	/**
	 * \brief Number of the file descriptor.
	 * This is the return value of open(), socket() or accept().
	 */
	int fd_;

	int __read (void* p, size_t size);
	int __write (const void* p, size_t size);

	PosixDescriptor(): worker_(0), worker_started_(false), fd_(-1) {
		DEBUG("Creating worker (stopped)");
		worker_ = new Worker (this);
	}

public:

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
	 * Note: currently there is no method to re-open the descriptor.
	 */
	inline virtual void close(){
		::close(fd_);
	}

	/**
	 * \brief Desctructor. It just calls close()
	 */
	virtual ~PosixDescriptor() {
		DEBUG("Destroying descriptor...");
		if (worker_started_){
			DEBUG("waitCompletion()...");
			worker_->waitCompletion();
			DEBUG("stop()...");
			worker_->stop();
			DEBUG("waitForTermination()...");
			//worker_->waitForTermination();
		}
		DEBUG("delete thread...");
		delete(worker_);
		close();
	}

	/**
	 * \brief Method to get descriptor number.
	 * @return Descriptor number.
	 */
	inline int getDescriptorNumber() const {
		return fd_;
	}

	/**
	 * \brief Copy constructor.
	 * The copy constructor is called to copy an existing object to
	 * another object that is being constructed.
	 * Examples:
	 * \code
	 * PosixDescriptor p1;
	 * PosixDescriptor p2 = p1;
	 * PosixDesscriptor p3 (p1);
	 * \endcode
	 * @exception runtime_error if the ::dup() returns an error
	 */
	PosixDescriptor(const PosixDescriptor& src): worker_(0){
		fd_ = ::dup(src.fd_);
		if (fd_ < 0) {
			ERROR("Bad file descriptor");
			throw std::runtime_error("PosixDescriptor: error in copy constructor");
		}
		DEBUG("Creating worker (stopped)");
		worker_ = new Worker (this);
	}

	/**
	 * \brief Assignment operator.
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
