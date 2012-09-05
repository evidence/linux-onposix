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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>
#include <stdexcept>
#include <iostream>

#include "Logger.hpp"
#include "Buffer.hpp"

namespace onposix {

/**
 * \brief Abstraction of a POSIX descriptor.
 * This is an abstract class for the concept of Posix descriptor.
 * The descriptor can correspond to a file (class FileDescriptor)
 * or to a socket (class TcpSocketServerDescriptor).
 */
class PosixDescriptor {

protected:
	/**
	 * \brief Number of the file descriptor.
	 * This is the return value of open(), socket() or accept().
	 */
	int fd_;

	int __read (void* p, size_t size);
	int __write (const void* p, size_t size);

	PosixDescriptor(): fd_(-1) {}


public:

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
	 * \brief Sync the descriptor
	 * Method to call fsync on the descriptor. Usually after a write
	 * operation.
	 */
	inline void sync(){
		::fsync(fd_);
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
	PosixDescriptor(const PosixDescriptor& src){
		fd_ = ::dup(src.fd_);
		if (fd_ < 0) {
			DEBUG(DBG_ERROR, "Bad file descriptor");
			throw std::runtime_error("PosixDescriptor: error in copy constructor");
		}
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
			DEBUG(DBG_ERROR, "Bad file descriptor");
			throw std::runtime_error("PosixDescriptor: error in operator=");
		}
		return *this;
	}

};

} /* onposix */

#endif /* POSIXDESCRIPTOR_HPP_ */
