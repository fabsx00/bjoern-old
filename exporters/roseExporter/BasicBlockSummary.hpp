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

struct BasicBlockSummary {
	enum ATTRIBUTES {
		NONE = 0,
		ENDS_IN_CALL = 1,
		ENDS_IN_RET = 1 << 1
	};

	list<BaseSemantics::StatePtr> finalStateList;
	list<BaseSemantics::StatePtr> preCallStateList;

	uint32_t attributes;

	BasicBlockSummary();
	void pushState(BaseSemantics::StatePtr final,
		       BaseSemantics::StatePtr preCall)
	{
		// finalStateList.push_back(final);
		// BasicBlockState s;
		// s.finalState = final;
		// s.preCallState = preCall;
		// stateList.push_back(s);
	}

	void popState()
	{
	      // finalStateList.pop_back();
	     // stateList.pop_back();
	}

	virtual ~BasicBlockSummary();
};

} /* namespace bjoern */

#endif /* BASICBLOCKSUMMARY_H_ */
