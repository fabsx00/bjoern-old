/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.h"

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

} /* namespace bjoern */
