/*
 * Buffer.hpp
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

#ifndef BUFFER_HPP_
#define BUFFER_HPP_



namespace onposix {

/**
 * \brief Very simple buffer with control on overflow.
 *
 * This is a simple buffer, internally allocated as a char buffer with new
 * and delete.
 * With respect to hand-made buffers, it adds the check on boundaries.
 */
class Buffer {
	/**
	 * \brief Current size of the buffer.
	 */
	unsigned long int size_;

	/**
	 * \brief Pointer to the allocated memory.
	 */
	char* data_;

	// Disable default copy constructor
	Buffer(const Buffer&);

public:
	explicit Buffer(unsigned long int size);
	virtual ~Buffer();
	char& operator[](unsigned long int p);
	unsigned long int fill(const char* src, unsigned long int size);
	unsigned long int fill(Buffer* b, unsigned long int size);
	bool compare(Buffer* b, unsigned long int size);
	bool compare(const char* s, unsigned long int size);

	/**
	 * \brief Method to get a pointer to the buffer.
	 *
	 * @return position of the first byte
	 */
	char* getBuffer(){
		return reinterpret_cast<char*> (data_);
	}

	/**
	 * \brief Method to get the size of the buffer
	 *
	 * @return Size of the buffer
	 */
	inline unsigned long int getSize() const {
		return size_;
	}

	inline unsigned long int fill(char* src, unsigned long int size) {
		return fill (reinterpret_cast<const char*> (src), size);
	}
};

} /* onposix */

#endif /* BUFFER_HPP_ */
