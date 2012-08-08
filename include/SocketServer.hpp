/*
 * SocketServer.hpp
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


#ifndef SOCKETSERVER_HPP_
#define SOCKETSERVER_HPP_

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>

namespace onposix {

/**
 * \brief Abstraction of a socket server.
 * This descriptor corresponds to a socket created with socket().
 */
class SocketServer {

	/**
	 * \brief Number of the file descriptor.
	 * This is the return value of socket().
	 */
	int fd_;

	/**
	 * \brief Type of socket (i.e., stream vs datagram)
	 * This variable is used to discriminate between stream (e.g., TCP)
	 * and datagram (e.g., UDP) protocols.
	 */
	enum S_mode_ {
		M_stream	= 1 << 0,
		M_dgram		= 1 << 1} socketMode_;

public:

	typedef S_mode_ S_mode;

	static const S_mode stream = M_stream;
	static const S_mode dgram = M_dgram;

	SocketServer(const uint16_t port, S_mode);

	SocketServer(const std::string& name, const S_mode mode);

	/**
	 * \brief Destructor.
	 * It just calls close() to close the descriptor.
	 */
	virtual ~SocketServer(){
		close();
	}

	/**
	 * \brief Close the descriptor
	 */
	inline void close(){
		::close(fd_);
	}

	/**
	 * \brief Method to get descriptor number.
	 * @return Descriptor number.
	 */
	inline int getDescriptorNumber() const {
		return fd_;
	}
};

} /* onposix */

#endif /* SOCKETSERVER_HPP_ */
