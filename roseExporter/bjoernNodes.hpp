#ifndef _BJOERN_NODES_
#define _BJOERN_NODES_

#include <string>
#include <sstream>

#include <stdint.h>

using namespace std;

class BjoernNode{
protected:
	unsigned long long id;
	string type;

public:
	void setId(unsigned long long anId)
	{
		id = anId;
	}
	
};

class BjoernFunction : public BjoernNode {
private:	
	string name;
	string addr;

public:
  
	BjoernFunction()
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
