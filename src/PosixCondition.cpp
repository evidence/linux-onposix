/*
 * PosixCondition.cpp
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

#include "PosixCondition.hpp"
#include "Assert.hpp"
#include <errno.h>
#include <string.h>
#include <stdexcept>

namespace onposix {

/**
 * \brief Constructor. Initialize the condition variable.
 *
 * @exception runtime_error if the initialization of the condition variable
 * fails.
 */
PosixCondition::PosixCondition()
{
	if (pthread_cond_init(&cond_, NULL) != 0)
		throw std::runtime_error(std::string("Error: ") + strerror(errno));
}

/**
 * \brief Destructor. Destroys the condition variable.
 */
PosixCondition::~PosixCondition()
{
	VERIFY_ASSERTION(!pthread_cond_destroy(&cond_));
}

} /* onposix */








