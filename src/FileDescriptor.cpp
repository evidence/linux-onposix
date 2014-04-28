/*
 * FileDescriptor.cpp
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

#include "FileDescriptor.hpp"

namespace onposix {

/**
 * \brief Constructor for descriptors of files.
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
 * @exception runtime_error if the ::open() returns an error
 */
FileDescriptor::FileDescriptor(const std::string& name,const  int flags)
{
		fd_ = open(name.c_str(), flags);
		if (fd_ <= 0) {
			ERROR("Opening file " << name);
			throw std::runtime_error ("Open file error");
		}
}


/**
 * \brief Constructor for descriptors of files.
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
FileDescriptor::FileDescriptor(const std::string& name, const int flags,
    const mode_t mode)
{
		fd_ = open(name.c_str(), flags, mode);
		if (fd_ <= 0) {
			ERROR("Opening file " << name);
			throw std::runtime_error ("Open file error");
		}
}



/**
 * \brief Method to get the length of current available data.
 *
 * @return length of data available on the file
 */
int FileDescriptor::getLength()
{
		int currentPosition = ::lseek(fd_, 0, SEEK_CUR);
		int end = ::lseek(fd_, 0, SEEK_END);
		::lseek(fd_, currentPosition, SEEK_SET);
		return end;
}

} /* onposix */
