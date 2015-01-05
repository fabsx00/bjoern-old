#ifndef _BJOERN_FUNCTION_HPP
#define _BJOERN_FUNCTION_HPP

#include "bjoernNodes.hpp"
#include <string>

using namespace std;

class BjoernFunction
{
private:
	BjoernFunctionNode *funcNode;

public:
	BjoernFunction()
	{
		funcNode = new BjoernFunctionNode();
	}
	
	void setName(string &aName)
	{
		funcNode->setName(aName);
	}

	void setAddr(uint64_t anAddr)
	{
		funcNode->setAddr(anAddr);
	}
};

#endif
