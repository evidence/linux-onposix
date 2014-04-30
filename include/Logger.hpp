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
#define LOG_MULTITHREAD

/// Log levels:
#define LOG_NOLOG	0 ///< No logging
#define LOG_ERRORS	1 ///< Log only error messages
#define LOG_WARNINGS	2 ///< Log warnings and error messages
#define LOG_ALL		3 ///< Log all

/// Log level for console messages:
#ifndef LOG_LEVEL_CONSOLE
#define LOG_LEVEL_CONSOLE	LOG_WARNINGS
#endif

/// Log level for file:
#ifndef LOG_LEVEL_FILE
#define LOG_LEVEL_FILE		LOG_ALL
#endif



#ifdef LOG_MULTITHREAD
#include "PosixMutex.hpp"
#endif

/**
 * \brief Macro to set the file used for logging.
 *
 * @param Base name of the file used for logging (e.g. "/tmp/myproject")
 *
 * Example of configuration of the Logger: *
 * \code
 * 	LOG_FILE("/tmp/myproject);
 * \endcode
 */
#define LOG_FILE(outputFile) { \
	onposix::Logger::getInstance().setFile(outputFile); \
	}



/**
 * \brief Macro to print error messages.
 *
 * Example of usage:
 * \code
 * 	ERROR("hello " << "world");
 * \endcode
 */
#if (defined NDEBUG) || (LOG_LEVEL_CONSOLE < LOG_ERRORS && LOG_LEVEL_FILE < LOG_ERRORS)
	#define ERROR(...)
#elif (LOG_LEVEL_CONSOLE < LOG_ERRORS)
	#define ERROR(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[ERROR]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#elif (LOG_LEVEL_FILE < LOG_ERRORS)
	#define ERROR(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[ERROR]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#else
	#define ERROR(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[ERROR]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#endif
	

/**
 * \brief Macro to print warning messages.
 *
 * Example of usage:
 * \code
 * 	WARNING("hello " << "world");
 * \endcode
 */
#if (defined NDEBUG) || (LOG_LEVEL_CONSOLE < LOG_WARNINGS && LOG_LEVEL_FILE < LOG_WARNINGS)
	#define WARNING(...)
#elif (LOG_LEVEL_CONSOLE < LOG_WARNINGS)
	#define WARNING(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[WARNING]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#elif (LOG_LEVEL_FILE < LOG_WARNINGS)
	#define WARNING(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[WARNING]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#else
	#define WARNING(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[WARNING]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#endif



/**
 * \brief Macro to print debug messages.
 *
 * Example of usage:
 * \code
 * 	DEBUG("hello " << "world");
 * \endcode
 */
#if (defined NDEBUG) || (LOG_LEVEL_CONSOLE < LOG_ALL && LOG_LEVEL_FILE < LOG_ALL)
	#define DEBUG(...)
#elif (LOG_LEVEL_CONSOLE < LOG_ALL)
	#define DEBUG(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[DEBUG]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#elif (LOG_LEVEL_FILE < LOG_ALL)
	#define DEBUG(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[DEBUG]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#else
	#define DEBUG(msg) { \
		std::ostringstream logger_dbg_stream__; \
		logger_dbg_stream__ << "[DEBUG]\t"; \
		logger_dbg_stream__ << msg; \
		onposix::Logger::getInstance().printOnConsole(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		onposix::Logger::getInstance().printOnFile(__FILE__, __LINE__, \
				logger_dbg_stream__.str()); \
		}
#endif


namespace onposix {

/**
 * \brief Simple logger to log messages on file and console.
 *
 * This is the implementation of a simple logger in C++. It is implemented 
 * as a Singleton, so it can be easily called through the DEBUG, WARNING
 * and ERROR macros.
 * It is Pthread-safe.
 * It allows to log on both file and console.
 *
 * Example of configuration of the Logger: *
 * \code
 * 	LOG_FILE("/tmp/myproject);
 * \endcode
 *
 * Example of usage of the Logger:
 * \code
 * 	DEBUG("hello " << "world");
 * \endcode
 */
class Logger
{
public:
	static Logger& getInstance();

	void printOnFile(	const std::string&	sourceFile,
				const int 		codeLine,
				const std::string& 	message);
	void printOnConsole(	const std::string&	sourceFile,
				const int 		codeLine,
				const std::string& 	message);

	void setFile (const std::string&	outputFile);

	/**
	 * \brief Method to know if the latest message has been printed on file
	 *
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnFile() const {
		return latestMsgPrintedOnFile_;
	}

	/**
	 * \brief Method to know if the latest message has been printed on file
	 *
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnConsole() const {
		return latestMsgPrintedOnConsole_;
	}

private:
	Logger();
	~Logger();

#ifdef LOG_MULTITHREAD
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
	 *
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
	 * on console.
	 */
	bool latestMsgPrintedOnConsole_;

	/**
	 * \brief Method to lock in case of multithreading
	 */
	inline static void lock();

	/**
	 * \brief Method to unlock in case of multithreading
	 */
	inline static void unlock();
};

} /* onposix */

#endif /* LOGGER_HPP_ */
