#ifndef _BJOERN_NODES_
#define _BJOERN_NODES_

#include <string>
#include <sstream>
#include <list>

#include <stdint.h>

#include "read_graph.hpp"
#include "../common/bjoernNode.hpp"

using namespace std;

class BjoernInstructionNode : public BjoernNode {
private:
//	string code;		// Uses BAP_SSA-internal-representation
	BAP_SSA *bap_ssa;
	bool jmp;		// Not used right now; was used for function-discovery
	list<BjoernInstructionNode *> data_successors;

public:
	BjoernInstructionNode();
	virtual ~BjoernInstructionNode();
	bool is_jmp() const;
	void set_jmp(bool j);
	virtual string getAddr() const;
	void setBAP_SSA(BAP_SSA *s);
	const BAP_SSA * getBAP_SSA() const;
//	void setCode(string str);
	const string & getCode() const;
	void addDataSuccessor(BjoernInstructionNode * suc);
	const list<BjoernInstructionNode *> & getDataSuccessors() const;
	void clear(); // Used only to make sure that destructor does not *always* destroy bap_ssa/data_successors
};

class BjoernBasicBlockNode : public BjoernNode {
private:
	list<BjoernInstructionNode *> instructions;
	list<uint64_t> successors;
public:
	BjoernBasicBlockNode();
	void addSuccessor(uint64_t suc);
	virtual string getAddr() const;
	const list<uint64_t> & getSuccessors() const;
	void addInstruction(BjoernInstructionNode *node);
	const list<BjoernInstructionNode *> & getInstructions() const;
	virtual ~BjoernBasicBlockNode();
	void clear(); // Used only to make sure that destructor does not *always' destroy instructions
	void freeInstructions();
};

class BjoernFunctionNode : public BjoernNode {
private:
	string name;
	list<BjoernBasicBlockNode *> basicBlocks;
public:
	BjoernFunctionNode();
	void setName(const string &aName);
	const string & getName() const;
	virtual string getAddr() const;
	void addBasicBlock(BjoernBasicBlockNode *basicBlock);
	const list<BjoernBasicBlockNode *> & getBasicBlocks() const;
	virtual ~BjoernFunctionNode();
	void freeBasicBlocks();
        void clear(); // Used only to make sure that destructor does not *always' destroy basicBlocks
};

#endif
