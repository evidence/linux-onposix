/*
 * Pipe.cpp
 *
 *  Created on: 05/set/2012
 *      Author: cloud
 */

#include <stdexcept>

//#define _GNU_SOURCE // needed to have pipe2()
#include <unistd.h>


#include "Pipe.hpp"
#include "Logger.hpp"

namespace onposix {

/**
 * \brief Constructor for the pipe
 *
 * @param flags can be 0, O_NONBLOCK or O_CLOEXEC
 * @exception runtime_error in case of error
 */
Pipe::Pipe(int flags): read_(0), write_(0)
{
	int fd[2];
	if (pipe2(fd, flags) != 0) {
		DEBUG(DBG_ERROR, "Error opening pipe");
		throw std::runtime_error ("Open pipe error");
	}
	read_ = new PosixDescriptor(fd[0]);
	write_ = new PosixDescriptor(fd[1]);
}


} /* onposix */
