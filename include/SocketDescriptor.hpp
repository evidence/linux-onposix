/*
 * SocketDescriptor.hpp
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

#ifndef SOCKETDESCRIPTOR_HPP_
#define SOCKETDESCRIPTOR_HPP_

#include "PosixDescriptor.hpp"
#include "SocketServer.hpp"

namespace onposix {

/**
 * \brief Abstraction of a socket descriptor.
 *
 * This is an abstract class for the concept of socket.
 * This descriptor corresponds to a socket created with accept() over a
 * SocketServer.
 *
 * Example of usage:
 * \code
 * SocketServer serv ("/tmp/mysocket", SocketServer::stream);
 * SocketDescriptor des (serv);
 * Buffer b (10);
 * des.read(b, b.getSize());
 * \endcode
 */
class SocketDescriptor: public PosixDescriptor {

public:
	explicit SocketDescriptor(const SocketServer& server);
};

} /* onposix */

#endif /* SOCKETDESCRIPTOR_HPP_ */
