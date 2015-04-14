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
#include <memory>

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;
using namespace std;


namespace bjoern {

	class MemAndRegisterState{
	private:
		BaseSemantics::StatePtr finalState;
		BaseSemantics::StatePtr preCallState;
	public:
		MemAndRegisterState(BaseSemantics::StatePtr final,
				    BaseSemantics::StatePtr pre):
			finalState(final), preCallState(pre){}

	};


	struct BasicBlockSummary {
		enum ATTRIBUTES {
			NONE = 0,
			ENDS_IN_CALL = 1,
			ENDS_IN_RET = 1 << 1
		};

		list<unique_ptr<MemAndRegisterState>> stateList;

		uint32_t attributes;

		BasicBlockSummary();
		void pushState(BaseSemantics::StatePtr final,
			       BaseSemantics::StatePtr preCall);

		void popState();

		virtual ~BasicBlockSummary();
	};

} /* namespace bjoern */

#endif /* BASICBLOCKSUMMARY_H_ */
