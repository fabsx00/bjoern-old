/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.hpp"
#include <utility>

namespace bjoern {

	BasicBlockSummary::BasicBlockSummary() : attributes(ATTRIBUTES::NONE) {
	}

	BasicBlockSummary::~BasicBlockSummary() {
	}

	void BasicBlockSummary :: pushState(BaseSemantics::StatePtr final,
		       BaseSemantics::StatePtr preCall)
	{
		unique_ptr<MemAndRegisterState> ptr(new MemAndRegisterState(final, preCall));
		stateList.push_back(move(ptr));
	}


	void BasicBlockSummary :: popState()
	{
		if(stateList.size() == 0){
			cout << "This should not happen either" << endl;
			return;
		}

		stateList.pop_back();
	}

	void BasicBlockSummary :: getUsedRegisters(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: getUsedMemory(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: getDefinedRegisters(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: getDefinedMemory(list<string> &out)
	{
		// TODO
	}

} /* namespace bjoern */
