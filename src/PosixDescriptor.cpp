/*
 * Descriptor.cpp
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

#include "PosixDescriptor.hpp"

namespace onposix {

/**
 * \brief Function to start an asynchronous operation
 *
 * This method allows to start an asynchronous operation, either read or write.
 * @param read_operation Specifies if it is a read (true) or write (false) operation
 * @param handler Function to be run at the end of the operation. 
 * This function will have as arguments a Buffer* where data is stored and the
 * number of bytes actually transferred.
 * @param buff Buffer where data must be copied to (for a read operation) or where
 * data is contained (for a write operation)
 * @param size Amount of bytes to be transferred
 */
void PosixDescriptor::Worker::startAsyncOperation (bool read_operation, 
    void (*handler) (Buffer* b, size_t size),
	Buffer* buff, size_t size)
{

	DEBUG("Async operation started with buffer*");
	struct job* j = new job;
	j->size_ = size;
	j->buff_handler_ = handler;
	j->buff_buffer_ = buff;
	if (read_operation)
		j->job_type_ = job::READ_BUFFER;
	else
		j->job_type_ = job::WRITE_BUFFER;

	queue_->push(j);
}



/**
 * \brief Function so start an asynchronous operation
 *
 * This method allows to start an asynchronous operation, either read or write.
 * @param read_operation Specifies if it is a read (true) or write (false) operation
 * @param handler Function to be run at the end of the operation. 
 * This function will have as arguments a void* where data is stored and the
 * number of bytes actually transferred.
 * @param buff void* where data must be copied to (for a read operation) or where
 * data is contained (for a write operation)
 * @param size Amount of bytes to be transferred
 */
void PosixDescriptor::Worker::startAsyncOperation (bool read_operation,
    void (*handler) (void* b, size_t size),
	void* buff, size_t size)
{

	DEBUG("Async operation started with void*");
	struct job* j = new job;
	j->size_ = size;
	j->void_handler_ = handler;
	j->void_buffer_ = buff;
	if (read_operation)
		j->job_type_ = job::READ_VOID;
	else
		j->job_type_ = job::WRITE_VOID;

	queue_->push(j);
}

/**
 * \brief Function run on the separate thread.
 *
 * This is the function automatically called by start() which, in turn, is called by
 * startAsyncOperation().
 * The function is run by the worker thread. It performs the read/write operation
 * and invokes the handler.
 * @exception It throws runtime_error in case no operation has been scheduled
 */
void PosixDescriptor::Worker::run()
{
	DEBUG("Worker running");
	for (;;) {
		DEBUG("===================");
		DEBUG("New cycle");
		bool close;
		job* j = queue_->pop(&close);
		if (j != 0){
			int n;
			DEBUG("Found one item in queue");
			DEBUG("Need to read " << j->size_ << " bytes");
			DEBUG("File descriptor = " << des_->getDescriptorNumber());

			if (j->job_type_ == job::READ_BUFFER)
				n = des_->do_read(j->buff_buffer_->getBuffer(), j->size_);
			else if (j->job_type_ == job::READ_VOID)
				n = des_->do_read(j->void_buffer_, j->size_);
			else if (j->job_type_ == job::WRITE_BUFFER)
				n = des_->do_write(j->buff_buffer_->getBuffer(), j->size_);
			else if (j->job_type_ == job::WRITE_VOID)
				n = des_->do_write(j->void_buffer_, j->size_);
			else {
				ERROR("Handler called without operation!");
				throw std::runtime_error ("Async error");
			}
			DEBUG("Read " << n << " bytes");

			DEBUG("Calling handler");
			if ((j->job_type_ == job::READ_BUFFER) || (j->job_type_ == job::WRITE_BUFFER))
				j->buff_handler_(j->buff_buffer_, n);
			else
				j->void_handler_(j->void_buffer_, n);
			delete j;
		} else {
			queue_->signal_empty();
			if (!close) {
				DEBUG("No data in queue");
				queue_->wait_not_empty();
			} else {
				DEBUG("Exiting!!");
				pthread_exit(0);
			}
		}
	}
}


/**
 * \brief Low-level read
 *
 * This method is private because it is meant to be used through the other read()
 * methods.
 * Note: it can block the caller, because it continues reading until the given
 * number of bytes have been read.
 * @param buffer Pointer to the buffer where read bytes must be stored
 * @param size Number of bytes to be read
 * @exception runtime_error if the ::read() returns an error
 * @return The number of actually read bytes or -1 in case of error
 */
int PosixDescriptor::do_read (void* buffer, size_t size)
{
	size_t remaining = size;
	while (remaining > 0) {
		ssize_t ret = ::read (fd_, ((char*)buffer)+(size-remaining),
		    remaining);
		if (ret == 0)
			// End of file reached
			break;
		else if (ret < 0) {
			throw std::runtime_error ("Read error");
			return -1;
		}
		remaining -= ret;
	}
	return (size-remaining);
}


/**
 * \brief Method to read from the descriptor and fill a buffer.
 *
 * Note: this method may block current thread if data is not available.
 * The buffer is filled with the read data.
 * @param b Pointer to the buffer to be filled
 * @param size Number of bytes that must be read
 * @return -1 in case of error; the number of bytes read otherwise
 */
int PosixDescriptor::read (Buffer* b, size_t size)
{
	if (b->getSize() == 0 || size > b->getSize()) {
		ERROR("Buffer size not enough!");
		return -1;
	}
	int ret = do_read(b->getBuffer(), size);
	return ret;
}

/**
 * \brief Method to read from the descriptor.
 *
 * Note: this method may block current thread if data is not available.
 * The buffer is filled with the read data.
 * @param p Pointer to the memory space to be filled
 * @param size Number of bytes that must be read
 * @return -1 in case of error; the number of bytes read otherwise
 */
int PosixDescriptor::read (void* p, size_t size)
{
	int ret = do_read(p, size);
	return ret;
}




/**
 * \brief Low-level write
 *
 * This method is private because it is meant to be used through the other
 * write() methods.
 * Note: it can block the caller, because it continues writing until the
 * given number of bytes have been written.
 * @param buffer Pointer to the buffer containing bytes to be written
 * @param size Number of bytes to be written
 * @exception runtime_error if the ::write() returns 0 or an error
 * @return The number of actually written bytes or -1 in case of error
 */
int PosixDescriptor::do_write (const void* buffer, size_t size)
{
	size_t remaining = size;
	while (remaining > 0) {
		ssize_t ret = ::write (fd_,
		    ((char*)buffer)+(size-remaining), remaining);
		if (ret == 0)
			// Cannot write more
			break;
		else if (ret < 0) {
			throw std::runtime_error ("Write error");
			return -1;
		}
		remaining -= ret;
	}
	return (size-remaining);
}


/**
 * \brief Method to write data in a buffer to the descriptor.
 *
 * Note: this method may block current thread if data cannot be written.
 * @param b Pointer to the buffer to be filled
 * @param size Number of bytes that must be written
 * @return -1 in case of error; the number of bytes read otherwise
 */
int PosixDescriptor::write (Buffer* b, size_t size)
{
	if (b->getSize() == 0 || size > b->getSize()) {
		ERROR("Buffer size not enough!");
		return -1;
	}
	return do_write(reinterpret_cast<const void*> (b->getBuffer()),
	    size);
}

/**
 * \brief Method to write to the descriptor.
 *
 * Note: this method may block current thread if data cannot be written.
 * @param p Pointer to the memory space containing data
 * @param size Number of bytes that must be written
 * @return -1 in case of error; the number of bytes read otherwise
 */
int PosixDescriptor::write (const void* p, size_t size)
{
	return do_write(p, size);
}


/**
 * \brief Method to write a string to the descriptor.
 *
 * Note: this method may block current thread if data cannot be written.
 * @param string to be written
 * @return -1 in case of error; the number of bytes read otherwise
 */
int PosixDescriptor::write (const std::string& s)
{
	return do_write(reinterpret_cast<const void*> (s.c_str()), s.size());
}

} /* onposix */
