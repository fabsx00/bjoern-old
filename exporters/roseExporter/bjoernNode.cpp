
#include "bjoernNode.hpp"

#include <stdexcept>

using namespace std;

BjoernNode::BjoernNode(): childId(0), id(0) {;}
BjoernNode::~BjoernNode() {;}

void BjoernNode :: setAddr(uint64_t anAddr)
{
	stringstream convert;
	convert << (uint64_t) anAddr;
	addr = convert.str();
}

void BjoernNode::setAddr(const string &anAddr)
{
	addr = anAddr;
}

void BjoernNode :: setId(unsigned long long anId)
{
	if(id != 0)
		throw runtime_error("Re-setting id");				
	
	id = anId;
}

void BjoernNode :: setChildId(unsigned long long anId)
{
	childId = anId;
}


unsigned long long BjoernNode :: getId() const
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
