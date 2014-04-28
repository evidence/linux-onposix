/*
 * DescriptorsMonitor.cpp
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


#include "DescriptorsMonitor.hpp"
#include "AbstractDescriptorReader.hpp"
#include "Logger.hpp"

namespace onposix {

/**
 * \brief Constructor.
 * It just initializes the set of descriptors.
 */
DescriptorsMonitor::DescriptorsMonitor(): highestDescriptor_(0)
{
	FD_ZERO(&descriptorSet_);
}

/**
 * \brief Destructor.
 *
 * It just deletes the data structure containing the association between
 * readers and monitored descriptors.
 * Note: it does not deletes the descriptors and the readers, because
 * they are just pointers to classes allocated somewhere else.
 */
DescriptorsMonitor::~DescriptorsMonitor()
{
	for (std::vector<monitoredDescriptor*>::iterator i = descriptors_.begin();
	    i != descriptors_.end(); ++i)
			delete(*i);
}

/**
 * \brief Method to start monitoring a descriptor.
 *
 * It is called by each class AbstractDescriptorReader that wants to be notified
 * about a specific descriptor.
 * @param reader class that wants to be notified
 * @param descriptor descriptor
 * @return true in case of success; false if the descriptor is already monitored
 */
bool DescriptorsMonitor::startMonitoringDescriptor(AbstractDescriptorReader& reader,
		PosixDescriptor& descriptor)
{
	if (FD_ISSET(descriptor.getDescriptorNumber(), &descriptorSet_)){
		ERROR("Descriptor already monitored by some reader");
		return false;
	}
	monitoredDescriptor* n = new monitoredDescriptor;
	n->descriptor_ = &descriptor;
	n->reader_ = &reader;
	descriptors_.push_back(n);

	FD_SET(descriptor.getDescriptorNumber(), &descriptorSet_);

	if (highestDescriptor_ < descriptor.getDescriptorNumber())
		highestDescriptor_ = descriptor.getDescriptorNumber();
	return true;
}

/**
 * \brief Method to stop monitoring a descriptor.
 *
 * It is called by each class AbstractDescriptorReader that wants to stop
 * notifications about a specific descriptor.
 * The AbstractDescriptorReader class is not among arguments, because each
 * descriptor can be monitored by at most one class.
 * @param descriptor whose notifications must be stopped
 * @return true in case of success; false if the descriptor was not monitored
 */
bool DescriptorsMonitor::stopMonitoringDescriptor(PosixDescriptor& descriptor)
{
	if (!FD_ISSET(descriptor.getDescriptorNumber(), &descriptorSet_)){
		ERROR("Descriptor was not monitored");
		return false;
	}

	for (std::vector<monitoredDescriptor*>::iterator i = descriptors_.begin();
	    i != descriptors_.end(); ++i)
		if ((*i)->descriptor_->getDescriptorNumber() ==
		    descriptor.getDescriptorNumber()) {
			delete(*i);
			descriptors_.erase(i);
			break;
		}
	FD_CLR(descriptor.getDescriptorNumber(), &descriptorSet_);
	return true;
}

/**
 * \brief Method to wait until some descriptor becomes ready for read
 * operations.
 *
 * It suspends the execution of the program until a descriptor becomes
 * ready.
 * @return true in case of success; false if selects() returns error
 */
bool DescriptorsMonitor::wait()
{
	// Additional variable needed because select() will change the set
	fd_set fd = descriptorSet_;

	// We need this additional variable, because this method calls
	// AbstractDescriptorReader::dataAvailable() which in turn can call
	// DescriptorsMonitor::startMonitoringDescriptor() to start
	// monitoring a further descriptor. This adds new descriptors to
	// descriptors_ within the execution of this method, messing up things.
	std::vector<monitoredDescriptor*> checkedDescriptors = descriptors_;
	int ret = select(highestDescriptor_+1,
			&fd,
			NULL,
			NULL,
			NULL);
	DEBUG("Select returned!");
	if (ret == -1){
		// Error in select()
		ERROR("select()");
		return false;
	} else if (!ret) {
		// Timeout
		DEBUG("Timeout()");
		return false;
	} else {
		// At least one descriptor is ready for read operations
		for (std::vector<monitoredDescriptor*>::iterator i = 
		    checkedDescriptors.begin();
		    i != checkedDescriptors.end(); ++i) {
			if (FD_ISSET((*i)->descriptor_->getDescriptorNumber(),
			    &fd)) {
				// Notify the class
				DEBUG("Notifying class...");
				((*i)->reader_)->dataAvailable(*((*i)->descriptor_));
			}
		}
		return true;
	}
}

} /* onposix */
