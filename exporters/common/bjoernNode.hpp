#ifndef _BJOERN_NODE_HPP
#define _BJOERN_NODE_HPP

#include <string>
#include <stdint.h>
#include <sstream>

#define UNSET_ID (0)

using namespace std;

class BjoernNode{
protected:
	string type;
	string addr;
	int childId;
	int id; /* set after writing */
	long long functionId;

public:

	BjoernNode();
	virtual ~BjoernNode();

	void setAddr(uint64_t anAddr);
	void setAddr(const string &anAddr);
	void setId(unsigned long long anId);
	void setChildId(unsigned long long anId);
	unsigned long long getId() const;
	const string & getType() const;
	virtual string getAddr() const;
	int getChildId() const;

	void setFunctionId(long long fId);
	long long getFunctionId() const;

};

class BjoernCodeNode : public BjoernNode {
protected:
	string code;
public:
	BjoernCodeNode() : BjoernNode() { }

	void setCode(string str)
	{
		code = str;
	}

	const string & getCode()
	{
		return code;
	}

	virtual ~BjoernCodeNode(){}
};


#endif
