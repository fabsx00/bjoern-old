#ifndef _BJOERN_NODES_
#define _BJOERN_NODES_

#include <string>
#include <sstream>
#include <list>

#include <stdint.h>

#include "bjoernNode.hpp"

using namespace std;


class BjoernInstructionNode : public BjoernNode {
private:
	string code;

public:
	BjoernInstructionNode()
	{
		type = "Instruction";
	}

	void setCode(string str)
	{
		code = str;
	}

	const string & getCode()
	{
		return code;
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

	const list<uint64_t> & getSuccessors()
	{
		return successors;
	}

	void addInstruction(BjoernInstructionNode *node)
	{
		instructions.push_back(node);
	}

	const list<BjoernInstructionNode *> & getInstructions()
	{
		return instructions;
	}

	~BjoernBasicBlockNode()
	{
		// freeInstructions();
	}

	void freeInstructions()
	{
		std::list<BjoernInstructionNode *>::const_iterator it;
		for (it = instructions.begin(); it != instructions.end(); ++it) {
			delete *it;
		}
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

	const string & getName()
	{
		return name;
	}

	void addBasicBlock(BjoernBasicBlockNode *basicBlock)
	{
		basicBlocks.push_back(basicBlock);
	}

	const list<BjoernBasicBlockNode *> & getBasicBlocks()
	{
		return basicBlocks;
	}




	~BjoernFunctionNode()
	{
		// freeBasicBlocks();
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
