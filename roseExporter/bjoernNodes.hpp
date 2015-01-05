#ifndef _BJOERN_NODES_
#define _BJOERN_NODES_

#include <string>
#include <sstream>

#include <stdint.h>

using namespace std;

class BjoernNode{
protected:
	string type;

public:
  	
};

class BjoernFunctionNode : public BjoernNode {
private:	
	string name;
	string addr;

public:
  
	BjoernFunctionNode()
	{
		type = "FunctionDef";
	}
	
	void setName(string &aName)
	{
		name = aName;
	}

	void setAddr(uint64_t anAddr)
	{
		stringstream convert;
		convert << (uint64_t) anAddr;
		addr = convert.str();
	}
	
};

#endif
