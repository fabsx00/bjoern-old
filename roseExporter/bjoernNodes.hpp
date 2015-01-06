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
	int childId;
public:

	BjoernNode(): childId(0) {}

	void setAddr(uint64_t anAddr)
	{
		stringstream convert;
		convert << (uint64_t) anAddr;
		addr = convert.str();
	}

};

class BjoernInstructionNode : public BjoernNode {
public:
	BjoernInstructionNode()
	{
		type = "Instruction";
	}
};

class BjoernBasicBlockNode : public BjoernNode {
private:
	list<BjoernInstructionNode *> instructions;
	list<uint64_t> successors;
public:
	BjoernBasicBlockNode()
	{
		type = "BasicBlock";
	}

	void addSuccessor(uint64_t suc)
	{
		successors.push_back(suc);
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

	void addBasicBlock(BjoernBasicBlockNode *basicBlock)
	{
		basicBlocks.push_back(basicBlock);
	}

	~BjoernFunctionNode()
	{
		freeBasicBlocks();
	}

	void freeBasicBlocks()
	{
		std::list<BjoernBasicBlockNode *>::const_iterator it;
		for (it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
			delete *it;
		}

	}

};

#endif
