/*
 * Pipe.hpp
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

#ifndef PIPE_HPP_
#define PIPE_HPP_

#include "PosixDescriptor.hpp"

#include <unistd.h>
#include <limits.h>


namespace onposix {

/**
 * \brief Posix pipe.
 *
 * This class wraps two PosixDescriptors because in some circumstances
 * (e.g., after a fork()) the code wants to explicitly close one of the
 * endpoints.
 * The getReadDescriptor() and getWriteDescriptor() methods allow to get
 * the PosixDescriptor and explicitly close it.
 */
class Pipe {

	PosixDescriptor* read_;
	PosixDescriptor* write_;

	// Disable default copy constructor
	Pipe(const Pipe&);

public:
	Pipe();

	/**
	 * \brief Destructor.
	 *
	 * The destructor of PosixDescriptor will close the descriptor.
	 */
	virtual ~Pipe() {
		if (write_ != 0)
			delete write_;
		if (read_ != 0)
			delete read_;
	}

	/**
	 * \brief Get the write endpoint
	 *
	 * This method is useful if we want to explicitly close the read
	 * endpoint.
	 * @return A pointer to the PosixDescriptor for reading
	 */
	PosixDescriptor* getReadDescriptor() {
		return read_;
	}

	/**
	 * \brief Get the read endpoint
	 *
	 * This method is useful if we want to explicitly close the write
	 * endpoint.
	 * @return A pointer to the PosixDescriptor for writing
	 */
	PosixDescriptor* getWriteDescriptor() {
		return write_;
	}

	/**
	 * \brief Method to read from the pipe and fill a buffer.
	 *
	 * Note: this method may block current thread if data is not
	 * available.
	 * The buffer is filled with the read data.
	 * @param b Pointer to the buffer to be filled
	 * @param size Number of bytes that must be read
	 * @return -1 in case of error; the number of bytes read otherwise
	 */
	inline int read (Buffer* b, size_t size) {
		return read_->read(b, size);
	}

	/**
	 * \brief Method to read from the pipe.
	 *
	 * Note: this method may block current thread if data is not
	 * available.
	 * The buffer is filled with the read data.
	 * @param p Pointer to the memory space to be filled
	 * @param size Number of bytes that must be read
	 * @return -1 in case of error; the number of bytes read otherwise
	 */
	inline int read (void* p, size_t size) {
		return read_->read(p, size);
	}

	/**
	 * \brief Method to write data in a buffer to the pipe.
	 *
	 * Note: this method may block current thread if data cannot
	 * be written.
	 * @param b Pointer to the buffer to be filled
	 * @param size Number of bytes that must be written
	 * @return -1 in case of error; the number of bytes read otherwise
	 */
	inline int write (Buffer* b, size_t size) {
		return write_->write(b, size);
	}

	/**
	 * \brief Method to write in the pipe.
	 *
	 * Note: this method may block current thread if data cannot
	 * be written.
	 * @param p Pointer to the memory space containing data
	 * @param size Number of bytes that must be written
	 * @return -1 in case of error; the number of bytes read otherwise
	 */
	inline int write (const void* p, size_t size) {
		return write_->write(p, size);
	}

	/**
	 * \brief Method to write a string in the pipe.
	 *
	 * Note: this method may block current thread if data cannot
	 * be written.
	 * @param string to be written
	 * @return -1 in case of error; the number of bytes read otherwise
	 */
	inline int write (const std::string& s) {
		return write_->write(s);
	}

	/**
	 * \brief Method to close the pipe.
	 *
	 * Note: currently there is no method to re-open the pipe.
	 */
	inline void close(){
		write_->close();
		read_->close();
	}

	/**
	 * \brief Get posix capacity of the pipe
	 *
	 * @return capacity of the pipe based on the Posix standard
	 */
	static inline long int getPosixCapacity() {
		return _POSIX_PIPE_BUF;
	}

	/**
	 * \brief Get real capacity of the pipe
	 *
	 * @return real capacity of the pipe
	 */
	inline long int getRealCapacity() const {
		return fpathconf(read_->getDescriptorNumber(),
		    _PC_PIPE_BUF);
	}
};

} /* onposix */

#endif /* PIPE_HPP_ */
