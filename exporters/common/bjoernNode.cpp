
#include "bjoernNode.hpp"

#include <stdexcept>

using namespace std;

BjoernNode::BjoernNode(): addr("-1"), childId(UNSET_ID), id(UNSET_ID) {;}
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
	if(id != UNSET_ID)	
		throw runtime_error("Re-setting id");
	
	id = anId;
}

void BjoernNode :: setChildId(unsigned long long anId)
{
	if(id != UNSET_ID)	
		throw runtime_error("Re-setting child-id");
	
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

void BjoernNode::setFunctionId(long long fId)
{
	functionId = fId;
}

long long BjoernNode::getFunctionId() const
{
	return functionId;
}
