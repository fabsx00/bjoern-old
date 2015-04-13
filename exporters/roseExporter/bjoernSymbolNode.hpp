#ifndef _BJOERN_SYMBOL_NODE_HPP
#define _BJOERN_SYMBOL_NODE_HPP

#include <string>

using namespace std;

class BjoernSymbolNode : public BjoernNode {
public:
	BjoernSymbolNode() : BjoernNode()
	{
		type = "Symbol";
	}
	
};

#endif
