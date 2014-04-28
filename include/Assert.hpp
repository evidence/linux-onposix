/*
 * Assert.hpp
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

#ifndef ASSERT_HPP_
#define ASSERT_HPP_

#include <iostream>
#include <cstdlib>

/**
 * \brief It prints the failed message and aborts the execution.
 *
 * This function is not meant to be used directly. Use VERIFY_ASSERTION.
 * @param expr	The expression that has been evaluated.
 * @param file	The file containing the evaluated expression.
 * @param line	The line that contains the evaluated expression.
 */
inline void AssertionFailedMsg(const char* expr, const char* file, int line)
{
	std::cerr << "Assertion \"" << expr << "\" failed [" << file << ":"
			  << line << "]" << std::endl;
	std::abort();
}

/** \brief Macro to evaluate an expression.
 *
 * If the expression evaluates to false then the AssertionFailedMsg function is
 * called.
 */
#define VERIFY_ASSERTION(expr) \
	(expr ? static_cast<void>(0) : AssertionFailedMsg(#expr, __FILE__,\
					__LINE__))

#endif /* ASSERT_HPP_ */
