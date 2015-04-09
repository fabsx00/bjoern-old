/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.h"
#include <utility>

namespace bjoern {

std::ostream& operator<<(std::ostream& os, const BasicBlockSummary& bbs) {
	os << "+++ Basic Block +++\n";
	os << "Attributes: [";
	if (bbs.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL) {
		os << "ENDS_IN_CALL ";
	}
	if (bbs.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_RET) {
			os << "ENDS_IN_RET ";
	}
	os << "]\n";

	if (bbs.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL) {
		os << "Pre-call state:\n";
		os << *bbs.preCallState;
	}
	os << "Final state:\n";
	os << *bbs.finalState;
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
	address = other.address;
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
