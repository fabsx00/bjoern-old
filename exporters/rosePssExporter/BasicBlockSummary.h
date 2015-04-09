/*
 * BasicBlockSummary.h
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#ifndef BASICBLOCKSUMMARY_H_
#define BASICBLOCKSUMMARY_H_

#include <rose.h>
#include <BaseSemantics2.h>
#include <map>
#include <memory>
#include <boost/shared_ptr.hpp>

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;


namespace bjoern {

struct BasicBlockSummary {
	enum ATTRIBUTES {
		NONE = 0,
		ENDS_IN_CALL = 1,
		ENDS_IN_RET = 1 << 1
	};

	BaseSemantics::StatePtr finalState;
	BaseSemantics::StatePtr preCallState;
	uint32_t attributes;
	rose_addr_t address;

	BasicBlockSummary();
	BasicBlockSummary(const SgAsmBlock*);
	BasicBlockSummary(BasicBlockSummary&&);
	virtual ~BasicBlockSummary();

	BasicBlockSummary& operator=(BasicBlockSummary&&);
};

typedef boost::shared_ptr<BasicBlockSummary> BasicBlockSummaryPtr;

std::ostream& operator<<(std::ostream&, const BasicBlockSummary&);

} /* namespace bjoern */

#endif /* BASICBLOCKSUMMARY_H_ */
