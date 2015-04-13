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
#include <list>

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;
using namespace std;


namespace bjoern {

struct BasicBlockState{
	BaseSemantics::StatePtr finalState;
	BaseSemantics::StatePtr preCallState;
};


struct BasicBlockSummary {
	enum ATTRIBUTES {
		NONE = 0,
		ENDS_IN_CALL = 1,
		ENDS_IN_RET = 1 << 1
	};

	list<BasicBlockState> stateList;

	uint32_t attributes;

	BasicBlockSummary();
	virtual ~BasicBlockSummary();

};

} /* namespace bjoern */

#endif /* BASICBLOCKSUMMARY_H_ */
