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

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;


namespace bjoern {

typedef std::map<size_t, BaseSemantics::StatePtr> StateMap;
std::ostream& operator<<(std::ostream& os, const StateMap& sm);

struct BasicBlockSummary {
	enum ATTRIBUTES {
		ENDS_IN_CALL = 1
	};

	StateMap sm;
	uint32_t attributes;

	BasicBlockSummary();
	BasicBlockSummary(BasicBlockSummary&&);
	virtual ~BasicBlockSummary();

	BasicBlockSummary& operator=(BasicBlockSummary&&);

};

} /* namespace bjoern */

#endif /* BASICBLOCKSUMMARY_H_ */
