/*
 * FifoDescriptor.cpp
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
#include <cerrno>

#include "FifoDescriptor.hpp"

namespace onposix {

/**
 * \brief Constructor for descriptors of fifos.
 *
 * It calls open() with the given flags.
 * @param name of the fifo
 * @param flags that can be
 * <ul>
 * <li> O_RDONLY
 * <li> O_WRONLY
 * <li> or O_RDWR
 * </ul>
 * or-ed with zero or more flags:
 * <ul>
 * <li> O_APPEND
 * <li> O_ASYNC
 * <li> O_CREAT
 * <li> O_NONBLOCK
 * <li> O_SYNC
 * </ul>
 * @exception runtime_error if the ::open() returns an error
 */
FifoDescriptor::FifoDescriptor(const std::string& name, const int flags)
{
	DEBUG("Opening fifo...");
	fd_ = open(name.c_str(), flags);
	if (fd_ <= 0) {
		ERROR("Opening fifo " << name);
		throw std::runtime_error ("Open fifo error");
	}
}


/**
 * \brief Constructor for descriptors of fifos.
 *
 * It calls open() with the given flags.
 * @param name of the file
 * @param flags that can be
 * <ul>
 * <li> O_RDONLY
 * <li> O_WRONLY
 * <li> or O_RDWR
 * </ul>
 * or-ed with zero or more flags:
 * <ul>
 * <li> O_APPEND
 * <li> O_ASYNC
 * <li> O_CREAT
 * <li> O_NONBLOCK
 * <li> O_SYNC
 * </ul>
 * @param mode can be
 * <ul>
 * <li> S_IRWXU  00700 user (file owner) has read, write and execute permission
 * <li> S_IRUSR  00400 user has read permission
 * <li> S_IWUSR  00200 user has write permission
 * <li> S_IXUSR  00100 user has execute permission
 * <li> S_IRWXG  00070 group has read, write and execute permission
 * <li> S_IRGRP  00040 group has read permission
 * <li> S_IWGRP  00020 group has write permission
 * <li> S_IXGRP  00010 group has execute permission
 * <li> S_IRWXO  00007 others have read, write and execute permission
 * <li> S_IROTH  00004 others have read permission
 * <li> S_IWOTH  00002 others have write permission
 * <li> S_IXOTH  00001 others have execute permission
 * </ul>
 * @exception runtime_error if the ::open() returns an error
 */
FifoDescriptor::FifoDescriptor(const std::string& name, const int flags,
    const mode_t mode)
{
		fd_ = open(name.c_str(), flags, mode);
		if (fd_ <= 0) {
			ERROR("Opening fifo " << name);
			throw std::runtime_error ("Open fifo error");
		}
}



/**
 * \brief Method to get the capacity of the fifo
 *
 * @return length of data available on the file;
 * 		-1 in case of error or unlimited capacity
 * @exception runtime_error in case it's not possible to get the capacity
 */
int FifoDescriptor::getCapacity()
{
	long int v;
	errno = 0;
	if ((v = fpathconf(fd_,_PC_PIPE_BUF)) == -1)
		if (errno != 0) {
			ERROR("Can't get fifo capacity");
			throw std::runtime_error ("Fifo capacity error");
			return -1;
		}
	return v;
}

} /* onposix */
