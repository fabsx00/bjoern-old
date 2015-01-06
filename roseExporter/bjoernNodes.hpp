#ifndef _BJOERN_NODES_
#define _BJOERN_NODES_

#include <string>
#include <sstream>
#include <list>

#include <stdint.h>

using namespace std;

class BjoernNode{
protected:
	string type;
	string addr;
public:

	void setAddr(uint64_t anAddr)
	{
		stringstream convert;
		convert << (uint64_t) anAddr;
		addr = convert.str();
	}

};

class BjoernBasicBlockNode : public BjoernNode {
public:
	BjoernBasicBlockNode()
	{
		type = "BasicBlock";
	}
};

class BjoernFunctionNode : public BjoernNode {
private:
	string name;
	list<BjoernBasicBlockNode *> basicBlocks;
public:

	BjoernFunctionNode()
	{
		type = "FunctionDef";
	}

	void setName(string &aName)
	{
		name = aName;
	}

};

class BjoernInstructionNode : public BjoernNode {
public:
	BjoernInstructionNode()
	{
		type = "Instruction";
	}
};

#endif
