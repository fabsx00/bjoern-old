#ifndef _BJOERN_INSTRUCTION_NODE_HPP
#define _BJOERN_INSTRUCTION_NODE_HPP

class BjoernInstructionNode : public BjoernNode {
private:
	string code;

public:
	BjoernInstructionNode() : BjoernNode()
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

#endif
