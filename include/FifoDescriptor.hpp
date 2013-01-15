/*
 * FifoDescriptor.hpp
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

#ifndef FIFODESCRIPTOR_HPP_
#define FIFODESCRIPTOR_HPP_

#include "PosixDescriptor.hpp"

namespace onposix {

/**
 * \brief Abstraction of a fifo (AKA "named pipe") descriptor.
 *
 * This is an abstract class for the concept of fifo.
 * The descriptor corresponds to an open fifo.
 *
 * Example of usage:
 * \code
 * FifoDescriptor fd ("/tmp/myfifo", O_RDONLY);
 * Buffer b (10);
 * fd.read (b, b.getSize());
 * \endcode
 */
class FifoDescriptor: public PosixDescriptor {

public:
	FifoDescriptor(const std::string& name, const int flags);
	FifoDescriptor(const std::string& name, const int flags,
	    const mode_t mode);
	int getCapacity();

};

} /* onposix */

#endif /* FIFODESCRIPTOR_HPP_ */
