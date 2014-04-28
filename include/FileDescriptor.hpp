/*
 * FileDescriptor.hpp
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

#ifndef FILEDESCRIPTOR_HPP_
#define FILEDESCRIPTOR_HPP_

#include "PosixDescriptor.hpp"

namespace onposix {

/**
 * \brief Abstraction of a file descriptor.
 *
 * This is an abstract class for the concept of file descriptor.
 * The descriptor corresponds to an open file.
 *
 * Example of usage:
 * \code
 * FileDescriptor fd ("/tmp/myfile", O_RDONLY);
 * Buffer b (10);
 * fd.read (b, b.getSize());
 * \endcode
 */
class FileDescriptor: public PosixDescriptor {

public:
	FileDescriptor(const std::string& name, const int flags);
	FileDescriptor(const std::string& name, const int flags,
	    const mode_t mode);
	int getLength();

	/**
	 * \brief Method to reposition at a certain offset
	 *
	 * @param offset Offset from the beginning of file
	 * @return the actual offset location in case of success; -1 in case of
	 * error
	 */
	inline int lseek(int offset) {
		return ::lseek(fd_, offset, SEEK_SET);
	}


	/**
	 * \brief Sync the descriptor
	 *
	 * Method to call fsync on the descriptor. Usually after a write
	 * operation.
	 */
	inline void sync(){
		::fsync(fd_);
	}
};

} /* onposix */

#endif /* FILEDESCRIPTOR_HPP_ */
