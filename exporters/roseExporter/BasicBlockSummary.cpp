/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.hpp"
#include <utility>

namespace bjoern {

std::ostream& operator<<(std::ostream& os, const StateMap& sm) {
	for (const auto& traceEntry : sm) {
		os << "Trace: " << traceEntry.first << std::endl;
		os << *(traceEntry.second) << std::endl;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const BasicBlockSummary& bbs) {
	os << "Attributes: [";
	if (bbs.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL) {
		os << "ENDS_IN_CALL ";
	}
	if (bbs.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_RET) {
			os << "ENDS_IN_RET ";
	}
	os << "]\n";
	os << bbs.sm;
	return os;
}

BasicBlockSummary::BasicBlockSummary() : attributes(ATTRIBUTES::NONE) {
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
