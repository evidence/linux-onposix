/*
 * StreamSocketServer.hpp
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


#ifndef STREAMSOCKETSERVER_HPP_
#define STREAMSOCKETSERVER_HPP_

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

///Default maximum number of pending connections
#define STREAM_MAX_PENDING_CONNECTIONS 100

/**
 * \brief Socket server for connection-oriented communications.
 *
 * This class corresponds to a socket created with socket(), that must be
 * given to the constructor of StreamSocketServerDescriptor to accept incoming
 * connections.
 */
class StreamSocketServer {

	StreamSocketServer(const StreamSocketServer&);
	StreamSocketServer& operator=(const StreamSocketServer&);

	/**
	 * \brief Number of the file descriptor.
	 *
	 * This is the return value of socket().
	 */
	int fd_;

public:

	StreamSocketServer(const uint16_t port,
	    int maxPendingConnections = STREAM_MAX_PENDING_CONNECTIONS);
	StreamSocketServer(const std::string& name,
	    int maxPendingConnections = STREAM_MAX_PENDING_CONNECTIONS);


	/**
	 * \brief Destructor.
	 *
	 * It just calls close() to close the descriptor.
	 */
	virtual ~StreamSocketServer(){
		close();
	}

	/**
	 * \brief Close the descriptor
	 */
	void close(){
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
};

} /* onposix */

#endif /* STREAMSOCKETSERVER_HPP_ */
