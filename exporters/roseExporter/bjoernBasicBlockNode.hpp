#ifndef _BJOERN_BASIC_BLOCK_NODE_HPP
#define _BJOERN_BASIC_BLOCK_NODE_HPP


class BjoernSymbolNode;
class BjoernFunctionNode;

class BjoernBasicBlockNode : public BjoernNode {
private:
	list<BjoernInstructionNode *> instructions;
	list<uint64_t> successors;
	BjoernFunctionNode *func;
	list<BjoernSymbolNode *> usedSymbols;
	list<BjoernSymbolNode *> definedSymbols;
public:
	BjoernBasicBlockNode() : BjoernNode()
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

	void setFunc(BjoernFunctionNode *bjoernFunc)
	{
		func = bjoernFunc;
	}

	BjoernFunctionNode *getFunc()
	{
		return func;
	}

	void addUsedSymbol(BjoernSymbolNode *node)
	{
		usedSymbols.push_back(node);
	}

	void addDefinedSymbol(BjoernSymbolNode *node)
	{
		definedSymbols.push_back(node);
	}

	void uniquifySymbols()
	{
		usedSymbols.unique();
		definedSymbols.unique();
	}

	void addInstruction(BjoernInstructionNode *node)
	{
		instructions.push_back(node);
	}

	const list<BjoernInstructionNode *> & getInstructions()
	{
		return instructions;
	}


	const list<BjoernSymbolNode *> & getUsedSymbols()
	{
		return usedSymbols;
	}

	const list<BjoernSymbolNode *> & getDefinedSymbols()
	{
		return definedSymbols;
	}

	~BjoernBasicBlockNode()
	{
		freeInstructions();
	}

	void freeInstructions()
	{
		std::list<BjoernInstructionNode *>::const_iterator it;
		for (it = instructions.begin(); it != instructions.end(); ++it) {
			delete *it;
		}
	}

};

#endif
