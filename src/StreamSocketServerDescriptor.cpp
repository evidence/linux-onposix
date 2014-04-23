/*
 * StreamSocketServerDescriptor.cpp
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
#include "StreamSocketServerDescriptor.hpp"

namespace onposix {

/**
 * \brief Constructor to accept() connections on a socket.
 *
 * This constructor calls accept().
 * @param socket StreamSocketServer on which a new connection must be accepted.
 * @exception runtime_error in case of error in accept()
 */
StreamSocketServerDescriptor::StreamSocketServerDescriptor(const StreamSocketServer& socket)
{
	fd_ = accept(socket.getDescriptorNumber(), NULL, 0);
	if (fd_ < 0) {
		ERROR("accept()");
		throw std::runtime_error("Accept error");
	}
}

} /* onposix */
