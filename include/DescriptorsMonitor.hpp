/*
 * DescriptorsMonitor.hpp
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

#ifndef DESCRIPTORSMONITOR_HPP_
#define DESCRIPTORSMONITOR_HPP_

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "PosixDescriptor.hpp"

namespace onposix {

class AbstractDescriptorReader;

/**
 * \brief Class to watch a set of descriptors and notify related classes.
 *
 * This class implements the "Observer" design pattern, and allows classes
 * inherited from AbstractDescriptorReader to be notified when a descriptor
 * they monitor becomes ready for read operations.
 * The class is a wrapper for the select() POSIX system call, so the
 * descriptor may refer to both a file or a socket.
 * When the descriptor becomes ready, this class notifies the reader
 * class by calling AbstractDescriptorReader::dataAvailable(int descriptor).
 * Notes:
 * <ul>
 * <li> This monitor works only for read operations (i.e., it does
 * not monitor write operations or system exceptions) even if it can
 * be easily extended to support also write operations.
 * <li> One descriptor can be monitored by at most one receiver.
 * <li> A receiver can monitor more than one descriptor.
 * </ul>
 * It is not implemented as a Singleton because it must be possible to have
 * more than one monitor with different sets of descriptors.
 * Classes that want to make use of this class must inherit from virtual 
 * class AbstractDescriptorReader, because the readiness of the descriptor
 * is notified through a call to 
 * AbstractDescriptorReader::dataAvailable(int descriptor).
 */

class DescriptorsMonitor {
	/**
	 * \brief Current set of monitored descriptors.
	 *
	 * This set is given as argument to the select() syscall.
	 */
	fd_set descriptorSet_;

	/**
	 * \brief Highest-value descriptor in descriptorSet_.
	 *
	 * The select() syscall needs this value + 1.
	 */
	int highestDescriptor_;

	/**
	 * \brief Association between a reader and a monitored descriptor.
	 */
	struct monitoredDescriptor {
		/**
		 * \brief Pointer to the observer class.
		 *
		 * This points to the class that wants to be notified when
		 * the descriptor ise ready for read operations.
		 */
		AbstractDescriptorReader* reader_;

		/**
		 * \brief Monitored descriptor.
		 */
		PosixDescriptor* descriptor_;
	};

	/**
	 * \brief Associations between readers and monitored descriptors
	 */
	std::vector<monitoredDescriptor*> descriptors_;

public:
	DescriptorsMonitor();
	virtual ~DescriptorsMonitor();
	bool startMonitoringDescriptor(AbstractDescriptorReader& reader,
	    PosixDescriptor& descriptor);
	bool stopMonitoringDescriptor(PosixDescriptor& descriptor);
	bool wait();
};

} /* onposix */

#endif /* DESCRIPTORSMONITOR_HPP_ */
