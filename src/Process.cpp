/*
 * Process.cpp
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

#include <stdexcept>
#include <sched.h>
#include <sys/wait.h>
#include <strings.h>
#include <signal.h>

#include <iostream>

#include "Process.hpp"
#include "Logger.hpp"

namespace onposix {

/**
 * \brief Create a process
 *
 * This function creates a new process through fork().
 * This function is not meant to be called explicitly, 
 * but it is automatically called by the constructors.
 * @exception std::runtime_error in case the process cannot
 * be created.
 */
void Process::createProcess()
{
	pid_ = fork();
	if (pid_ == -1) {
		running_ = false;
		ERROR("Cannot start process");
		throw std::runtime_error ("Async error");
	}
	
	if (pid_ == 0) {
		// Child
		pid_ = getpid();
		is_child_ = true;
	} else {
		// Parent
		is_child_ = false;
	}
	running_ = true;
}

/**
 * \brief Constructor to run a specific function
 *
 * This constructor creates a new process that will run the
 * function given as argument.
 * @param function: pointer to the function that must be run
 */
Process::Process(void (*function)(void)):
    is_child_(false),
    running_(false),
    status_(0)
{
	createProcess();
	if (is_child_ && running_){
		(*function)();
	}
}
		
/**
 * \brief Constructor to run a specific program
 *
 * This constructor creates a new process that will run the
 * program given as argument. This contructor invokes fork()+execvp().
 * @param program: name of the program to be run
 * @args args: list of arguments
 */
Process::Process(const std::string& program, 
    const std::vector<std::string>& args):
    is_child_(false),
    running_(false),
    status_(0)
{
	createProcess();
	if (is_child_ && running_){
		char* c_args [20];
		c_args[0] = const_cast<char*> (program.c_str());
		for (unsigned int i = 0; i < args.size(); ++i){
			c_args[i+1] = const_cast<char*> (args[i].c_str());
		}
		c_args[args.size()+1] = (char*) NULL;

		execvp(program.c_str(), c_args);
	}
}

/**
 * \brief Function to wait the termination of the process
 *
 * To avoid deadlocks, this function can be called only by the parent
 * and not by the child itself.
 * @return false in case the function is called by the child or in
 * case of abnormal termination; true in case of normal termination
 */
bool Process::waitForTermination()
{
	if (is_child_)
		return false;
	waitpid(pid_, &status_, 1);
	running_ = false;
	if (WIFEXITED(status_))
		return true;
	else
		return false;
}

/**
 * \brief Function to check if the child has terminated correctly
 *
 * This function must be invoked after waitForTermination() and
 * allows to inspect the termination status of the child.
 * @return true in case of normal termination; false otherwise
 */
inline bool Process::checkNormalTermination()
{
	if (is_child_)
		return false;
	if (WIFEXITED(status_))
		return true;
	else
		return false;
}

/**
 * \brief Function to check if the child has terminated for a signal
 *
 * This function must be invoked after waitForTermination() and
 * allows to inspect the termination status of the child.
 * @return true in case the child has terminated due to a signal;
 * false otherwise
 */
inline bool Process::checkSignalTermination()
{
	if (is_child_)
		return false;
	if (WIFSIGNALED(status_))
		return true;
	else
		return false;
}



/**
 * \brief Function to send a signal to the process
 *
 * This method allows to send a signal to the process related to
 * this instance of Process. This function wraps the classical kill()
 * function.
 * The list of signals is available on /usr/include/bits/signum.h
 * @param sig the signal to be sent
 * @return true on success; false if some error occurred
 */
bool Process::sendSignal(int sig)
{
	if (kill(pid_, sig) != 0){
		ERROR("Can't send signal " << sig);
		return false;
	}
	return true;
}

/**
 * \brief Set a handler for a specific signal
 *
 * This method allows to manually set a handler for handling a specific signal.
 * The list of signals is available on /usr/include/bits/signum.h
 * Use signals less as possible, mainly for standard situations.
 * During the execution of the handler other signals may arrive. This can lead
 * to inconsistent states. The handler must be short.
 * It must just update the internal state and/or kill the application.
 * Not all library functions can be called inside the handler without having
 * strange behaviors (see man signal). In particular, it's not safe calling
 * functions of the standard library, like printf() or exit(), or other
 * functions defined inside the application itself. The access to global
 * variables is not safe either, unless they have been defined as volatile.
 * @param sig the signal to be sent
 * @return true on success; false if some error occurred
 */
bool Process::setSignalHandler(int sig, void (*handler) (int))
{
	bool ret = true;
	sigset_t oldset, set;
	struct sigaction sa;
	/* mask all signals until the handlers are installed */
	sigfillset(&set);
	sigprocmask(SIG_SETMASK, &set, &oldset);
	bzero( &sa, sizeof(sa) );
	sa.sa_handler = handler;
	if (sigaction(sig, &sa, NULL) < 0) {
		ERROR("Can't set signal " << sig);
		ret = false;
	}
	/* remove the mask */
	sigprocmask(SIG_SETMASK, &oldset,NULL);
	return ret;
}



/**
 * \brief Set scheduling policy and priority
 *
 * @param policy: policy (SCHED_FIFO, SCHED_RR or SCHED_OTHER)
 * @param priority: scheduling priority
 * @return true in case of success; false in case of error
 */
bool Process::setSchedParam(int policy, int priority)
{
	struct sched_param p;
	p.sched_priority = priority;
	if (sched_setscheduler(pid_, policy, &p) == 0)
		return true;
	else
		return false;
}



/**
 * \brief Get current scheduling policy and priority
 *
 * @param policy: policy (SCHED_FIFO, SCHED_RR or SCHED_OTHER)
 * @param priority: scheduling priority; it has a meaning only for SCHED_FIFO
 * and SCHED_RR
 * @return true in case of success; false in case of error
 */
bool Process::getSchedParam(int* policy, int* priority)
{
	struct sched_param p;
	int ret = sched_getparam(pid_, &p);
	*priority = p.sched_priority;
	*policy = sched_getscheduler(pid_);
	if ((*policy < 0) || (ret < 0))
		return false;
	else
		return true;
}


#if defined(ONPOSIX_LINUX_SPECIFIC) && defined(__GLIBC__) && \
    ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ > 3)))
/**
 * \brief Set CPU affinity
 *
 * Example of usage:
 * \code
 * std::vector<bool> v (2);
 * v[0] = true;
 * v[1] = false;
 * setAffinity(v);
 * \endcode
 * @param v: vector of booleans containing the affinity (true/false)
 * @exception std::runtime_error in case affinity cannot be set
 */
void Process::setAffinity(const std::vector<bool>& v)
{
	cpu_set_t s;
	CPU_ZERO(&s);
	for (unsigned int i = 0; (i < v.size()) && (i < CPU_SETSIZE); ++i)
		if (v[i])
			CPU_SET(i, &s);

	if ((sched_setaffinity(pid_, sizeof(s), &s) != 0))
		throw std::runtime_error ("Set affinity error");
}

/**
 * \brief Get CPU affinity
 *
 * Example of usage:
 * \code
 * std::vector<bool> v (2);
 * getAffinity(&v);
 * std::cout << "Affinity: " << v[0] << " " << v[1] << std::endl;
 * \endcode
 * @param v: vector of booleans containing the current affinity (true/false)
 * @exception std::runtime_error in case affinity cannot be get
 */
void Process::getAffinity(std::vector<bool>* v)
{
	cpu_set_t s;
	CPU_ZERO(&s);

	if ((sched_getaffinity(pid_, sizeof(s), &s) != 0))
		throw std::runtime_error ("Get affinity error");

        for (unsigned int j = 0; (j < CPU_SETSIZE) && (j < v->size()); ++j) {
		if (CPU_ISSET(j, &s))
			(*v)[j] = true;
		else
		(*v)[j] = false;
	}
}
#endif /* ONPOSIX_LINUX_SPECIFIC && GLIBC */


} /* onposix */
