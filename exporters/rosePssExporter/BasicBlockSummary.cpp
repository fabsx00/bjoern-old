/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.h"
#include <utility>

namespace bjoern {

std::ostream& operator<<(std::ostream& os, const StateMap& sm) {
	for (const auto& traceEntry : sm) {
		os << "Trace: " << traceEntry.first << std::endl;
		os << *(traceEntry.second) << std::endl;
	}
	return os;
}

BasicBlockSummary::BasicBlockSummary() : attributes(0) {
}

BasicBlockSummary::~BasicBlockSummary() {
}

BasicBlockSummary::BasicBlockSummary(BasicBlockSummary&& other) {
	attributes = other.attributes;
	sm = std::move(other.sm);
}

BasicBlockSummary& BasicBlockSummary::operator=(BasicBlockSummary&& other) {
	attributes = other.attributes;
	sm = std::move(other.sm);
	return *this;
}
} /* namespace bjoern */
