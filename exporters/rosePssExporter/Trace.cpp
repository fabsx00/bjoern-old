/*
 * Trace.cpp
 *
 *  Created on: Apr 2, 2015
 *      Author: bjoern
 */

#include "Trace.h"

namespace bjoern {

Trace::Trace() {

}

Trace::~Trace() {

}

Trace::Trace(const Trace& other) {
	summaries = other.summaries;
}

bool Trace::addBasicBlock(BasicBlockSummaryPtr summary) {
	summaries.push_back(summary);
	return true;
}

std::ostream& operator<<(std::ostream& os, const Trace& t) {
	os << "*** Trace ***\n";
	for (auto summary : t.summaries) {
		os << *summary << std::endl;
	}
	return os;
}

} /* namespace bjoern */
