#ifndef _BJOERN_INSTRUCTION_NODE_HPP
#define _BJOERN_INSTRUCTION_NODE_HPP

class BjoernInstructionNode : public BjoernCodeNode {
public:
	BjoernInstructionNode() : BjoernCodeNode()
	{
		type = "Instruction";
	}

};

#endif
