/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.hpp"
#include <utility>


using namespace BinaryAnalysis :: InstructionSemantics2 :: BaseSemantics;

namespace bjoern {

	BasicBlockSummary::BasicBlockSummary() : attributes(ATTRIBUTES::NONE) {
	}

	BasicBlockSummary::~BasicBlockSummary() {
	}

	void BasicBlockSummary :: pushState(BaseSemantics::StatePtr final,
		       BaseSemantics::StatePtr preCall)
	{
		stateList.push_back(new MemAndRegisterState(final, preCall));
	}


	void BasicBlockSummary :: popState()
	{
		if(stateList.size() == 0){
			cout << "This should not happen either" << endl;
			return;
		}

		auto back = stateList.back();
		delete back;
		stateList.pop_back();
	}

	void BasicBlockSummary :: getUsedForRegisters(list<string> &out)
	{

	}

	void BasicBlockSummary :: getUsedMemory(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: getDefinedRegisters(list<string> &out)
	{
		for(auto it : stateList){
			auto finalState = it->finalState;
			auto regState = finalState->get_register_state();
			auto regDict = regState->get_register_dictionary();

			auto ptr = dynamic_pointer_cast<RegisterStateGeneric>(regState);
			auto storedRegs = ptr->get_stored_registers();

			for(auto reg: storedRegs){
				auto regName = regDict->lookup(reg.desc);
				out.push_back(regName);
			}
		}
	}


	void BasicBlockSummary :: getDefinedMemory(list<string> &out)
	{

	}

} /* namespace bjoern */
