#ifndef _BJOERN_FUNCTION_NODE_HPP
#define _BJOERN_FUNCTION_NODE_HPP

class BjoernFunctionNode : public BjoernNode {
private:
	string name;
	list<BjoernBasicBlockNode *> basicBlocks;
public:

	BjoernFunctionNode() : BjoernNode()
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
