/*
 * Pipe.cpp
 *
 *  Created on: 05/set/2012
 *      Author: cloud
 */

#include <stdexcept>
#include <unistd.h>

#include "Pipe.hpp"
#include "Logger.hpp"

namespace onposix {

/**
 * \brief Constructor for the pipe
 *
 * @exception runtime_error in case of error
 */
Pipe::Pipe(): read_(0), write_(0)
{
	int fd[2];
	if (pipe(fd) != 0) {
		ERROR("Opening pipe");
		throw std::runtime_error ("Open pipe error");
	}
	read_ = new PosixDescriptor(fd[0]);
	write_ = new PosixDescriptor(fd[1]);
}


} /* onposix */
