/*
 * Logger.cpp
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

#include <iostream>
#include <new>
#include <cstdlib>

#include "Logger.hpp"

namespace onposix {

// Definition (and initialization) of static attributes
Logger* Logger::m_ = 0;

#ifdef LOGGER_MULTITHREAD

PosixMutex Logger::lock_ ;

inline void Logger::lock()
{
	lock_.lock();
}

inline void Logger::unlock()
{
    lock_.unlock();
}
#else
inline void Logger::lock(){}
inline void Logger::unlock(){}
#endif





/**
 * \brief Constructor.
 * It is a private constructor, called only by getInstance() and only the
 * first time. It is called inside a lock, so lock inside this method
 * is not required.
 * It only initializes the initial time. All configuration is done inside the
 * configure() method.
 */
Logger::Logger():
		fileSeverityLevel_(OFF),
		screenSeverityLevel_(ERROR),
		latestMsgPrintedOnFile_(false),
		latestMsgPrintedOnScreen_(false)
{
	gettimeofday(&initialTime_, NULL);
}

/**
 * \brief Method to configure the logger. Called by the DEBUG_CONF() macro.
 * To make implementation easier, the old stream is always closed.
 * Then, in case, it is open again in append mode.
 * @param outputFile Name of the file used for logging
 * @param configuration Configuration (i.e., log on file and on screen on or off)
 * @param fileSeverityLevel Severity threshold for file
 * @param screenSeverityLevel Severity threshold for screen
 */
void Logger::configure (const std::string&	outputFile,
			const severity_level_t	fileSeverityLevel,
			const severity_level_t	screenSeverityLevel)
{
		Logger::lock();
		latestMsgPrintedOnFile_ = false;
		latestMsgPrintedOnScreen_ = false;

		// Close the old stream, if needed
		if ((fileSeverityLevel_ != OFF) && (fileSeverityLevel == OFF))
			out_.close();

		// Compute a new file name, if needed
		if (outputFile != logFile_){
			std::ostringstream oss;
			time_t currTime;
			time(&currTime);
			struct tm *currTm = localtime(&currTime);
			oss << outputFile << "_" <<
					currTm->tm_mday << "_" <<
					currTm->tm_mon << "_" <<
					(1900 + currTm->tm_year) << "_" <<
					currTm->tm_hour << "-" <<
					currTm->tm_min << "-" <<
					currTm->tm_sec << ".log";
			logFile_ = oss.str().c_str();
		}

		// Open a new stream, if needed
		if ((fileSeverityLevel_ == OFF) && (fileSeverityLevel != OFF))
			out_.open(logFile_.c_str(), std::ios::app);

		fileSeverityLevel_ = fileSeverityLevel;
		screenSeverityLevel_ = screenSeverityLevel;

		Logger::unlock();
}

/**
 * \brief Destructor.
 * It only closes the file, if open, and cleans memory.
 */

Logger::~Logger()
{
	Logger::lock();
	if (fileSeverityLevel_ != OFF)
		out_.close();
	delete m_;
	Logger::unlock();

}

/**
 * \brief Method to get a reference to the object (i.e., Singleton)
 * It is a static method.
 * @return Reference to the object.
 */
Logger& Logger::getInstance()
{
	if (m_ == 0){
	    Logger::lock();
	    if (m_ == 0)
		m_ = new Logger;
	    Logger::unlock();
	}
	return *m_;
}


/**
 * \brief Method used to print messages.
 * Called by the DEBUG() macro.
 * @param severitylevel Severity of the debug message
 * @param file Source file where the method has been called (set equal to __FILE__
 * 	      by the DEBUG macro)
 * @param line Number of line in the source code where the method has been
 * called (automatically set equal to __LINE__ by the DEBUG macro)
 * @param message Message to be logged
 */
void Logger::print(const severity_level_t severityLevel,
					const std::string& file,
					const int line,
					const std::string& message)
{
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);

	Logger::lock();

	latestMsgPrintedOnFile_ = false;
	latestMsgPrintedOnScreen_ = false;

	if (fileSeverityLevel_ >= severityLevel){
		out_ << "DEBUG [" << file << ":" << line << "] @ " <<
		    (currentTime.tv_sec - initialTime_.tv_sec) <<
		    ":" << message << std::endl;
		latestMsgPrintedOnFile_ = true;
	}

	if (screenSeverityLevel_ >= severityLevel){
		std::cerr << "DEBUG [" << file << ":" << line << "] @ "
		    << (currentTime.tv_sec - initialTime_.tv_sec) <<
		    ":" << message << std::endl;
		latestMsgPrintedOnScreen_ = true;
	}
	Logger::unlock();
}

} /* onposix */
