/*
 * TcpSocketServer.hpp
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


#ifndef TCPSOCKETSERVER_HPP_
#define TCPSOCKETSERVER_HPP_

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string>

#include "AbstractSocketServer.hpp"

namespace onposix {

/**
 * \brief Abstraction of a socket server.
 * This descriptor corresponds to a socket created with socket().
 */
class TcpSocketServer: public AbstractSocketServer {

	TcpSocketServer(const TcpSocketServer&);
	TcpSocketServer& operator=(const TcpSocketServer&);

public:

	TcpSocketServer(const uint16_t port);
	TcpSocketServer(const std::string& name);

	/**
	 * \brief Destructor.
	 * It just calls close() to close the descriptor.
	 */
	virtual ~TcpSocketServer(){
		close();
	}

	/**
	 * \brief Close the descriptor
	 */
	virtual void close(){
		::close(fd_);
	}
};

} /* onposix */

#endif /* TCPSOCKETSERVER_HPP_ */