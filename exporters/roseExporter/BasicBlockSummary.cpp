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

	void BasicBlockSummary :: getUsedRegisters(list<string> &out)
	{

	}

	void BasicBlockSummary :: getUsedMemory(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: getDefinedRegisters(list<string> &out)
	{
		walkRegisterStateList(out, &BasicBlockSummary::getDefinedRegisterFromLine);
	}


	void BasicBlockSummary :: getDefinedMemory(list<string> &out)
	{
		// TODO
	}

	void BasicBlockSummary :: walkRegisterStateList(list<string> &out,
			   string (BasicBlockSummary::*lineToString)(string &))
	{
		for(auto it : stateList){
			auto finalState = it->finalState;
			stringstream sstr;
			finalState->get_register_state()->print(sstr);
			string line;
			while(getline(sstr, line) ){
				out.push_back((this->*lineToString)(line));
			}
		}
	}

	string BasicBlockSummary :: getDefinedRegisterFromLine(string &line)
	{
		return line.substr(0, line.find(" "));
	}

} /* namespace bjoern */
