/*
 * DgramSocketClientDescriptor.cpp
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Logger.hpp"
#include "DgramSocketClientDescriptor.hpp"

namespace onposix {

/**
 * \brief Constructor for local (i.e., AF_UNIX) sockets.
 *
 * It calls socket()+connect().
 * @param name Name of the local socket on the filesystem
 * @exception runtime_error in case of error in socket() or connect()
 */
DgramSocketClientDescriptor::DgramSocketClientDescriptor(const std::string& name)
{
	// socket()
	fd_ = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd_ < 0) {
		ERROR("Client socket creation");
		throw std::runtime_error ("Client socket error");
	}

	// connect()
	struct sockaddr_un serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, name.c_str(), sizeof(serv_addr.sun_path) - 1);
	if (connect(fd_, (struct sockaddr *) &serv_addr,
	                                sizeof(serv_addr)) < 0) {
		::close(fd_);
		ERROR("connect()");
		throw std::runtime_error ("Client socket error");
	}
}


/**
 * \brief Constructor for UDP (i.e., AF_INET) sockets.
 *
 * It calls socket()+connect().
 * @param port Port of the socket
 * @exception runtime_error in case of error in socket() or connect()
 */
DgramSocketClientDescriptor::DgramSocketClientDescriptor(const std::string& address,
				const uint16_t port)
{
	// socket()
	fd_ = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd_ < 0) {
		ERROR("Client socket creation");
		throw std::runtime_error ("Client socket error");
	}

	// connect()
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	struct in_addr addr;
	inet_aton(address.c_str(), &addr);
	bcopy(&addr, &serv_addr.sin_addr.s_addr, sizeof(addr));

	if (connect(fd_, (struct sockaddr *) &serv_addr,
	                                sizeof(serv_addr)) < 0) {
		::close(fd_);
		ERROR("connect()");
		throw std::runtime_error ("Client socket error");
	}

}

} /* onposix */
