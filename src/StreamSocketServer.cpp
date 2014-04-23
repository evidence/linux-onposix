/*
 * StreamSocketServer.cpp
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

#include <stdexcept>
#include <unistd.h>
#include "StreamSocketServer.hpp"
#include "Logger.hpp"


namespace onposix {

/**
 * \brief Constructor for local connection-oriented sockets.
 *
 * This constructor creates a connection-oriented AF_UNIX socket.
 * It calls socket()+bind()+listen().
 * @param name Name of the local socket on the filesystem
 * @exception runtime_error in case of error in socket(), bind() or listen()
 */
StreamSocketServer::StreamSocketServer(const std::string& name,
				int maxPendingConnections)
{
	// socket()
	fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd_ < 0) {
		ERROR("Creating client socket");
		throw std::runtime_error ("Socket error");
	}

	// bind()
	struct sockaddr_un serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, name.c_str(),
	    sizeof(serv_addr.sun_path) - 1);
	if (bind(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		::close(fd_);
		ERROR("bind()");
		throw std::runtime_error ("Bind error");
	}

	// listen()
	if (listen(fd_, maxPendingConnections) < 0) {
		::close(fd_);
		ERROR("listen()");
		throw std::runtime_error ("Listen error");
	}
}

/**
 * \brief Constructor for TCP connection-oriented sockets.
 *
 * This constructor creates a connection-oriented AF_INET socket.
 * It calls socket()+bind()+listen().
 * If the protocol is a stream, it also calls listen().
 * @param port Port of the socket
 * @exception runtime_error in case of error in socket(), bind() or listen()
 *
 */
StreamSocketServer::StreamSocketServer(const uint16_t port, int maxPendingConnections)
{
	// socket()
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_ < 0) {
		ERROR("Creating client socket");
		throw std::runtime_error ("Socket error");
	}

	// bind()
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		::close(fd_);
		ERROR("bind()");
		throw std::runtime_error ("Bind error");
	}

	// listen()
	if (listen(fd_, maxPendingConnections) < 0) {
		::close(fd_);
		ERROR("listen()");
		throw std::runtime_error ("Listen error");
	}
}

} /* onposix */
