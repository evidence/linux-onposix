/*
 * Buffer.cpp
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
#include <cstring>

#include "Buffer.hpp"

namespace onposix {

/**
 * \brief Constructor. It checks size and allocates memory.
 *
 * @param size size of the buffer
 * @exception invalid_argument in case of wrong size
 */
Buffer::Buffer(unsigned long int size): size_(size)
{
	if (size == 0)
		throw std::invalid_argument("Buffer with size 0");
	else
		data_ = new char[size_];
}

/**
 * Destructor.
 * It deallocates memory.
 */
Buffer::~Buffer()
{
	if (size_ != 0)
		delete[] data_;
}

/**
 * \brief Method to access a specific byte of the buffer.
 *
 * It checks the argument and throws an exception in case of out of range.
 * @param p position in the buffer
 * @return the byte at the specified position
 * @exception out_of_range in case the position is out of boundary
 */
char& Buffer::operator[](unsigned long int p)
{
	if (p > (size_ - 1))
		throw std::out_of_range("Operation on buffer out of boundary");
	else
		return data_[p];
}


/**
 * \brief Method to fill the buffer
 *
 * It takes the content from a specified address.
 * @param src source of the content used to fill the buffer
 * @param size number of bytes to be copied
 * @return number of bytes copied
 * @exception out_of_range in case the size is greater than the size of the buffer
 * @exception invalid_argument in case the source points to NULL
 */
unsigned long int Buffer::fill(const char* src, unsigned long int size)
{
	if (size > size_)
		throw std::out_of_range("Operation on buffer out of boundary");
	else if (src == 0)
		throw std::invalid_argument("Attempt to copy from NULL pointer");
	else if (size == 0)
		return 0;
	std::memcpy (data_, src, size);
	return size;
}

/**
 * \brief Method to fill the buffer with the content of another buffer
 *
 * The number of bytes copied is the minimum between the size of the buffer and
 * the size provided as argument.
 * @param b pointer to the buffer whose data must be used to fill this buffer
 * @param size size of bytes to be copied
 * @return number of bytes copied
 * @see Buffer::fill(const char* src, unsigned long int size)
 */
unsigned long int Buffer::fill(Buffer* b, unsigned long int size)
{
	unsigned long int min;
	if (size < b->getSize())
		min = size;
	else
		min = b->getSize();
	return fill (b->getBuffer(), min);
}



/**
 * \brief Method to compare two buffers' content
 *
 * It compares the content of this buffer with the content of another buffer
 * @param b the buffer against whose content it must be compared
 * @param size size of bytes to be compared
 * @return true if the contents match, false otherwise
 * @exception out_of_range in case the given size is greater than the size of one of the two
 * buffers
 */
bool Buffer::compare(Buffer* b, unsigned long int size)
{
	if (size > size_ || size > b->getSize())
		throw std::out_of_range("Operation on buffer out of boundary");
	return !std::memcmp(data_, b->getBuffer(), size);
}

/**
 * \brief Method to compare the content of the buffer against a buffer
 * in memory.
 *
 * It compares the content of this buffer with the content at a
 * specified memory address
 * @param s pointer to the memory address against whose content it must be compared
 * @param size size of bytes to be compared
 * @return true if the contents match, false otherwise
 * @exception out_of_range in case the given size is greater than the buffer
 * buffers
 */
bool Buffer::compare(const char* s, unsigned long int size)
{
	if (size > size_)
		throw std::out_of_range("Operation on buffer out of boundary");
	return !memcmp(data_, s, size);
}



} /* onposix */
