/*
 * Logger.hpp
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

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <sys/time.h>

/// Comment this line if you don't need multithread support
#define LOGGER_MULTITHREAD

#ifdef LOGGER_MULTITHREAD
#include "PosixMutex.hpp"
#endif


namespace onposix {

enum severity_level_t {
	OFF 	= 0,
	ERROR	= 1,
	WARN	= 2,
	DEBUG	= 3};

/**
 * \brief Macro to configure the logger.
 *
 * @param Base name of the file used for logging (e.g. "/tmp/myproject")
 * @param File severity level (i.e., OFF, ERROR, WARN, DEBUG)
 * @param Console severity level (i.e., OFF, ERROR, WARN, DEBUG)
 *
 * Example of configuration of the Logger: *
 * \code
 * 	DEBUG_CONF("outputfile", DEBUG, ERROR);
 * \endcode
 */
#define DEBUG_CONF(outputFile, \
		fileSeverityLevel, \
		screenSeverityLevel) { \
			Logger::getInstance().configure(outputFile, \
						fileSeverityLevel, \
						screenSeverityLevel); \
		}

/**
 * \brief Macro to print log messages.
 *
 * Example of usage of the Logger:
 * \code
 * 	DEBUG(DEBUG, "hello " << "world");
 * \endcode
 */
#define DEBUG(priority, msg) { \
	std::ostringstream __debug_stream__; \
	__debug_stream__ << msg; \
	Logger::getInstance().print(priority, __FILE__, __LINE__, \
			__debug_stream__.str()); \
	}





/**
 * \brief Simple logger to log messages on file and console.
 *
 * This is the implementation of a simple logger in C++. It is implemented 
 * as a Singleton, so it can be easily called through two DEBUG macros.
 * It is Pthread-safe.
 * It allows to log on both file and screen, and to specify a severity level
 * for both.
 *
 * Example of configuration of the Logger: *
 * \code
 * 	DEBUG_CONF("outputfile", DEBUG, ERROR);
 * \endcode
 *
 * Example of usage of the Logger:
 * \code
 * 	DEBUG(DEBUG, "hello " << "world");
 * \endcode
 */
class Logger
{
	/* \brief Current severity level for logging on file.
	 *
	 * It can be OFF, DEBUG, WARN or ERROR.
	 */
	severity_level_t fileSeverityLevel_;

	/* \brief Current severity level for logging on screen.
	 *
	 * It can be OFF, DEBUG, WARN or ERROR.
	 */
	severity_level_t screenSeverityLevel_;

#ifdef LOGGER_MULTITHREAD
	/**
	 * \brief Lock for mutual exclusion between different threads
	 */
	static PosixMutex lock_;
#endif
	
	/**
	 * \brief Pointer to the unique Logger (i.e., Singleton)
	 */
	static Logger* m_;

	/**
	 * \brief Initial part of the name of the file used for Logging.
	 * Date and time are automatically appended.
	 */
	std::string logFile_;

	/**
	 * \brief Stream used when logging on a file
	 */
	std::ofstream out_;

	/**
	 * \brief Initial time (used to print relative times)
	 */
	struct timeval initialTime_;

	/**
	 * \brief Debug: to know if the latest message has been printed
	 * on file.
	 */
	bool latestMsgPrintedOnFile_;

	/**
	 * \brief Debug: to know if the latest message has been printed 
	 * on screen.
	 */
	bool latestMsgPrintedOnScreen_;

	Logger();
	~Logger();

	/**
	 * \brief Method to lock in case of multithreading
	 */
	inline static void lock();

	/**
	 * \brief Method to unlock in case of multithreading
	 */
	inline static void unlock();

public:

	static Logger& getInstance();

	void print(const severity_level_t	severityLevel,
			const std::string&	sourceFile,
			const int 		codeLine,
			const std::string& 	message);

	void configure (const std::string&	outputFile,
			const severity_level_t	fileSeverityLevel,
			const severity_level_t	screenSeverityLevel);

	/**
	 * \brief Method to know if the latest message has been printed on file
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnFile() const {
		return latestMsgPrintedOnFile_;
	}

	/**
	 * \brief Method to know if the latest message has been printed on file
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnScreen() const {
		return latestMsgPrintedOnScreen_;
	}

};

} /* onposix */

#endif /* LOGGER_HPP_ */
