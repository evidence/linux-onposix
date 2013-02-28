/*
 * Process.hpp
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

#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>

// Uncomment to enable Linux-specific methods:
#define ONPOSIX_LINUX_SPECIFIC

namespace onposix {

/**
 * \brief Class for launching a new process
 *
 * Class to launch a process through fork().
 *
 * Example of usage to run a function:
 * \code
 *
 * void function ()
 * {
 *	//...
 * }
 *
 * int main ()
 * {
 * 	Process p(function);
 * }
 * \endcode
 *
 * Example of usage to run a program (i.e., through for()+execvp())
 * \code
 *
 * int main ()
 * {
 *	std::vector<std::string> args;
 *	args.push_back("-l");
 *	args.push_back("*.cpp");
 *
 * 	Process p("ls", args);
 * }
 * \endcode
 */
class Process {

	/**
	 * \brief Pid of the new process
	 *
	 * This value contains the pid of the new process for both the
	 * parent process and the child process (who sees its own pid)
	 */
	pid_t pid_;

	/**
	 * \brief If the class instance is related to the current process
	 *
	 * In a parent-child relationship, this variable is useful to 
	 * distinguish between the parent and the child.
	 * This variable is euql to false for the parent (i.e., the process
	 * who created the new process) and equal to true for the child
	 * process (i.e., the one who has been created).
	 */
	bool is_child_;
	
	/**
	 * \brief If the process is running
	 */
	bool running_;

	/**
	 * \brief Exit status of the child process when terminated.
	 *
	 * The child process can be terminated by the parent through
	 * sendSignal(KILL). The parent can also waith the normal termination
	 * of the child through waitForTermination().
	 */
	int status_;

	void createProcess();

public:

	/**
	 * \brief Get PID of the process related to this instance
	 *
	 * This value is the pid of the new process for both the
	 * parent process and the child process (who gets its own pid)
	 */
	inline pid_t getPid () const {
		return pid_;
	}

	Process(void (*function)(void));
			
	Process(const std::string& program, 
	    const std::vector<std::string>& args);


	bool waitForTermination();
	inline bool checkNormalTermination();
	inline bool checkSignalTermination();
	bool sendSignal(int sig);
	static bool setSignalHandler(int sig, void (*handler) (int));
	bool setSchedParam(int policy, int priority);
	bool getSchedParam(int* policy, int* priority);

#if defined(ONPOSIX_LINUX_SPECIFIC) && defined(__GLIBC__) && \
    ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ > 3)))
	// Functions to get/set affinity are available only from glibc 2.4
	void setAffinity(const std::vector<bool>& s);
	void getAffinity(std::vector<bool>* v);
#endif /* ONPOSIX_LINUX_SPECIFIC && GLIBC */
};

} /* onposix */

#endif /* PROCESS_HPP_ */
