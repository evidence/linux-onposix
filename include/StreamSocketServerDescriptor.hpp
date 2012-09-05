/*
 * StreamSocketServerDescriptor.hpp
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

#ifndef STREAMSOCKETSERVERDESCRIPTOR_HPP_
#define STREAMSOCKETSERVERDESCRIPTOR_HPP_

#include "PosixDescriptor.hpp"
#include "StreamSocketServer.hpp"

namespace onposix {

/**
 * \brief Socket descriptor for connection-oriented communications.
 *
 * This is a class to accept connection-oriented connections.
 * This descriptor corresponds to a socket created with accept() over a
 * StreamSocketServer.
 *
 * Example of usage:
 * \code
 * StreamSocketServer serv ("/tmp/mysocket");
 * StreamSocketServerDescriptor des (serv);
 * Buffer b (10);
 * des.read(b, b.getSize());
 * \endcode
 */
class StreamSocketServerDescriptor: public PosixDescriptor {

public:
	explicit StreamSocketServerDescriptor(const StreamSocketServer& server);
};

} /* onposix */

#endif /* STREAMSOCKETSERVERDESCRIPTOR_HPP_ */
