#include "bjoernNodes.hpp"


BjoernNode::BjoernNode(): childId(0), id(0) {;}
BjoernNode::~BjoernNode() {;}


void BjoernNode::setAddr(uint64_t anAddr)
{
	stringstream convert;
	convert << "0x" << hex << (uint64_t) anAddr;
	addr = convert.str();
}

void BjoernNode::setAddr(const string &anAddr)
{
	addr = anAddr;
}

void BjoernNode::setChildId(unsigned long long anId)
{
	childId = anId;
}

void BjoernNode::setId(unsigned long long anId)
{
	if(id != 0)
	{
		cout << "Re-setting id" << endl;
		exit(1);
	}
	id = anId;
}

unsigned long long BjoernNode::getId() const
{
	return id;
}

const string & BjoernNode::getType() const
{
	return type;
}

string BjoernNode::getAddr() const
{
	return addr;
}

int BjoernNode::getChildId() const
{
	return childId;
}





string BjoernInstructionNode::getAddr() const
{
	stringstream s;
	s << hex << "0x" << bap_ssa->asm_addr << "_" << dec << bap_ssa->asm_counter;
	return s.str();
}

BjoernInstructionNode::BjoernInstructionNode()
{
	type = "Instruction";
}

BjoernInstructionNode::~BjoernInstructionNode() {;}

bool BjoernInstructionNode::is_jmp() const
{
	return jmp;
}

void BjoernInstructionNode::set_jmp(bool j)
{
	jmp = j;
}

void BjoernInstructionNode::setBAP_SSA(BAP_SSA *s)
{
	bap_ssa = s;
}

const BAP_SSA * BjoernInstructionNode::getBAP_SSA() const
{
	return bap_ssa;
}

//void BjoernInstructionNode::setCode(string str)
//{
//	code = str;
//}

const string & BjoernInstructionNode::getCode() const
{
	return bap_ssa->code;
}

void BjoernInstructionNode::addDataSuccessor(BjoernInstructionNode * suc)
{
	data_successors.push_back(suc);
}

const list<BjoernInstructionNode *> & BjoernInstructionNode::getDataSuccessors() const
{
	return data_successors;
}

void BjoernInstructionNode::clear()
{
	bap_ssa = NULL;
	data_successors.clear();
}




BjoernBasicBlockNode::BjoernBasicBlockNode()
{
	type = "BasicBlock";
}

string BjoernBasicBlockNode::getAddr() const
{
	return addr + "_b";
}

void BjoernBasicBlockNode::addSuccessor(uint64_t suc)
{
	successors.push_back(suc);
}

const list<uint64_t> & BjoernBasicBlockNode::getSuccessors() const
{
	return successors;
}

void BjoernBasicBlockNode::addInstruction(BjoernInstructionNode *node)
{
	instructions.push_back(node);
}

const list<BjoernInstructionNode *> & BjoernBasicBlockNode::getInstructions() const
{
	return instructions;
}

BjoernBasicBlockNode::~BjoernBasicBlockNode()
{
	freeInstructions();
}

void BjoernBasicBlockNode::clear()
{
	instructions.clear();
	successors.clear();
}

void BjoernBasicBlockNode::freeInstructions()
{
	std::list<BjoernInstructionNode *>::const_iterator it;
	for (it = instructions.begin(); it != instructions.end(); ++it) {
		delete *it;
	}
}





BjoernFunctionNode::BjoernFunctionNode()
{
	type = "FunctionDef";
}

string BjoernFunctionNode::getAddr() const
{
	return addr + "_f";
}

void BjoernFunctionNode::setName(const string &aName)
{
	name = aName;
}

const string & BjoernFunctionNode::getName() const
{
	return name;
}

void BjoernFunctionNode::addBasicBlock(BjoernBasicBlockNode *basicBlock)
{
	basicBlocks.push_back(basicBlock);
}

const list<BjoernBasicBlockNode *> & BjoernFunctionNode::getBasicBlocks() const
{
	return basicBlocks;
}

BjoernFunctionNode::~BjoernFunctionNode()
{
	freeBasicBlocks();
}

void BjoernFunctionNode::clear()
{
	basicBlocks.clear();
}

void BjoernFunctionNode::freeBasicBlocks()
{
	std::list<BjoernBasicBlockNode *>::const_iterator it;
	for (it = basicBlocks.begin(); it != basicBlocks.end(); ++it) {
		delete *it;
	}
}


