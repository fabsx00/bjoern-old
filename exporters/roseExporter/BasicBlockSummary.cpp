/*
 * BasicBlockSummary.cpp
 *
 *  Created on: Mar 30, 2015
 *      Author: bjoern
 */

#include "BasicBlockSummary.hpp"
#include <utility>
#include <boost/algorithm/string/predicate.hpp>

using namespace BinaryAnalysis :: InstructionSemantics2 :: BaseSemantics;
using namespace boost::algorithm;

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
		for(auto it : stateList){
			auto finalState = it->finalState;
			auto regState = finalState->get_register_state();
			auto regDict = regState->get_register_dictionary();

			auto ptr = dynamic_pointer_cast<RegisterStateGeneric>(regState);
			auto storedRegs = ptr->get_stored_registers();

			for(auto reg: storedRegs){
				auto regName = regDict->lookup(reg.desc);
				if(isUntrackedRegister(regName))
					continue;

				stringstream sstr;
				sstr << *(reg.value);
				out.push_back(sstr.str());
			}
		}

	}

	/** This is actually 'memory used to change other memory' */

	void BasicBlockSummary :: getUsedMemory(list<string> &out)
	{
		for(auto it : stateList){
			auto finalState = it->finalState;
			auto memState = finalState->get_memory_state();
			auto ptr = dynamic_pointer_cast<MemoryCellList>(memState);
			auto cells = ptr->get_cells();
			for(auto cell : cells){
				stringstream sstr;
				sstr << *(cell->get_value());
				out.push_back(sstr.str());
			}
		}
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
				if(isUntrackedRegister(regName))
					continue;

				out.push_back(regName);
			}
		}
	}


	void BasicBlockSummary :: getDefinedMemory(list<string> &out)
	{
		for(auto it : stateList){
			auto finalState = it->finalState;
			auto memState = finalState->get_memory_state();
			auto ptr = dynamic_pointer_cast<MemoryCellList>(memState);
			auto cells = ptr->get_cells();
			for(auto cell : cells){
				stringstream sstr;
				sstr << *(cell->get_address());
				out.push_back(sstr.str());
			}
		}
	}

	// Don't track eip/flags

	bool BasicBlockSummary :: isUntrackedRegister(string &regName)
	{
		return (ends_with(regName, "f") || regName == "eip");
	}


} /* namespace bjoern */
