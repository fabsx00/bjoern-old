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

BasicBlockSummary::BasicBlockSummary(const SgAsmBlock* bb) : attributes(ATTRIBUTES::NONE) {
	address = bb->get_address();
}

BasicBlockSummary::BasicBlockSummary() : attributes(ATTRIBUTES::NONE), address(0) {
}

BasicBlockSummary::~BasicBlockSummary() {
}

BasicBlockSummary::BasicBlockSummary(BasicBlockSummary&& other) {
	attributes = other.attributes;
	finalState = std::move(other.finalState);
	preCallState = std::move(other.preCallState);
}

BasicBlockSummary& BasicBlockSummary::operator=(BasicBlockSummary&& other) {
	attributes = other.attributes;
	finalState = std::move(other.finalState);
	preCallState = std::move(other.preCallState);
	return *this;
}
} /* namespace bjoern */
