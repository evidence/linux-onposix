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

const int DBG_ERROR	= 0;
const int DBG_WARN	= 1;
const int DBG_DEBUG	= 2;

/**
 * \brief Macro to configure the logger.
 *
 * Example of configuration of the Logger: *
 * \code
 * 	DEBUG_CONF("outputfile", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
 * \endcode
 */
#define DEBUG_CONF(outputFile, \
		configuration, \
		fileVerbosityLevel, \
		screenVerbosityLevel) { \
			Logger::getInstance().configure(outputFile, \
						configuration, \
						fileVerbosityLevel, \
						screenVerbosityLevel); \
		}

/**
 * \brief Macro to print log messages.
 *
 * Example of usage of the Logger:
 * \code
 * 	DEBUG(DBG_DEBUG, "hello " << "world");
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
 * It allows to log on both file and screen, and to specify a verbosity
 * threshold for both of them.
 *
 * Example of configuration of the Logger: *
 * \code
 * 	DEBUG_CONF("outputfile", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
 * \endcode
 *
 * Example of usage of the Logger:
 * \code
 * 	DEBUG(DBG_DEBUG, "hello " << "world");
 * \endcode
 */
class Logger
{
	/**
	 * \brief Type used for the configuration
	 */
	enum loggerConf_	{L_nofile_	= 	1 << 0,
				L_file_		=	1 << 1,
				L_noscreen_	=	1 << 2,
				L_screen_	=	1 << 3};

#ifdef LOGGER_MULTITHREAD
	/**
	 * \brief Lock for mutual exclusion between different threads
	 */
	static PosixMutex lock_;
#endif
	
	bool configured_;

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
	 * \brief Current configuration of the logger.
	 * Variable to know if logging on file and on screen are enabled.
	 * Note that if the log on file is enabled, it means that the
	 * logger has been already configured, therefore the stream is
	 * already open.
	 */
	loggerConf_ configuration_;

	/**
	 * \brief Stream used when logging on a file
	 */
	std::ofstream out_;

	/**
	 * \brief Initial time (used to print relative times)
	 */
	struct timeval initialTime_;

	/**
	 * \brief Verbosity threshold for files
	 */
	unsigned int fileVerbosityLevel_;

	/**
	 * \brief Verbosity threshold for screen
	 */
	unsigned int screenVerbosityLevel_;

	/**
	 * \brief Debug: to know if the latest message has been printed on file.
	 */
	bool latestMsgPrintedOnFile_;

	/**
	 * \brief Debug: to know if the latest message has been printed on screen.
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

	typedef loggerConf_ loggerConf;
	static const loggerConf file_on= 	L_nofile_;
	static const loggerConf file_off= 	L_file_;
	static const loggerConf screen_on= 	L_noscreen_;
	static const loggerConf screen_off=	L_screen_;

	static Logger& getInstance();

	void print(const unsigned int		verbosityLevel,
			const std::string&	sourceFile,
			const int 		codeLine,
			const std::string& 	message);

	void configure (const std::string&	outputFile,
			const loggerConf	configuration,
			const int		fileVerbosityLevel,
			const int		screenVerbosityLevel);

	/**
	 * \brief Method to know if the latest message has been printed on file
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnFile() const {return latestMsgPrintedOnFile_;}

	/**
	 * \brief Method to know if the latest message has been printed on file
	 * @return true if it has been printed; false otherwise
	 */
	inline bool latestMsgPrintedOnScreen() const {return latestMsgPrintedOnScreen_;}

};

inline Logger::loggerConf operator|
	(Logger::loggerConf __a, Logger::loggerConf __b)
{
	return Logger::loggerConf(static_cast<int>(__a) |
		static_cast<int>(__b));
}

inline Logger::loggerConf operator&
	(Logger::loggerConf __a, Logger::loggerConf __b)
{
	return Logger::loggerConf(static_cast<int>(__a) &
		static_cast<int>(__b)); }

} /* onposix */

#endif /* LOGGER_HPP_ */
