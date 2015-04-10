/*
 * Trace.h
 *
 *  Created on: Apr 2, 2015
 *      Author: bjoern
 */

#ifndef TRACE_H_
#define TRACE_H_
#include "BasicBlockSummary.h"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace bjoern {

struct Trace {
	Trace();
	Trace(const Trace&);
	bool addBasicBlock(BasicBlockSummaryPtr);
	virtual ~Trace();

	std::vector<BasicBlockSummaryPtr> summaries;
};

typedef boost::shared_ptr<Trace> TracePtr;

std::ostream& operator<<(std::ostream&, const Trace&);

} /* namespace bjoern */

#endif /* TRACE_H_ */
