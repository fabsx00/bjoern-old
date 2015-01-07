#include "CSVWriter.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

enum bjoernKey_t {
	TYPE,
	ADDR,
	CHILD_ID,
	NAME,
	N_KEYS
};

const char *keys[] = {
	"type",
	"addr",
	"childId",
	"name"
};

void CSVWriter :: init()
{
	openOutputFiles();
	writeNodeHeader();
	writeEdgeHeader();
}

void CSVWriter :: openOutputFiles()
{
	nodeFile.open("nodes.csv");
	edgeFile.open("edge.csv");

	if(!nodeFile.is_open() || !edgeFile.is_open())
		throw runtime_error("Cannot open output files");

}

void CSVWriter :: writeNodeHeader()
{
	for(int i = 0; i < N_KEYS - 1; i++){
		nodeFile << keys[i] << '\t';
	}
	nodeFile << keys[N_KEYS - 1] << endl;
}

void CSVWriter :: writeEdgeHeader()
{
	edgeFile << "start" << "\t"
		 << "end" << "\t"
		 << "type" << endl;
}


void CSVWriter :: writeFunction(BjoernFunctionNode *func)
{
	resetMaps();
	writeFunctionNode(func);
	writeBasicBlocksOfFunc(func);
	connectFunctionToEntryBlock(func);
}

void CSVWriter :: writeFunctionNode(BjoernFunctionNode *func)
{
	writeBjoernNode(func);

	nodeFile << "\t" << func->getName();
	nodeFile << endl;
	finishNode(func);
}

void CSVWriter :: writeBasicBlocksOfFunc(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	     it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;
		writeBjoernNode(basicBlock);
		nodeFile << endl;
		finishNode(basicBlock);
	}

}

void CSVWriter :: connectFunctionToEntryBlock(BjoernFunctionNode *func)
{
	unsigned long long srcId = func->getId();
	string entryAddr = func->getAddr();
	map<string, BjoernNode *> :: iterator it = addrToNode.find(entryAddr);


	if(it == addrToNode.end())
		throw runtime_error("Error: can't find entry node for function\n");

	BjoernNode *entryBlock = it->second;
	unsigned long long dstId = entryBlock->getId();

	writeEdge(srcId, dstId, "FUNCTION_OF_CFG");

}

void CSVWriter :: writeEdge(unsigned long long srcId, unsigned long long dstId,
		const char *edgeType)
{
	edgeFile << srcId << "\t" << dstId << "\t" << edgeType << endl;
}

void CSVWriter :: finishNode(BjoernNode *node)
{
	registerNodeForId(node);
	curId ++;
}

void CSVWriter :: writeBjoernNode(BjoernNode *node)
{
	nodeFile << node->getType() << "\t"
		 << node->getAddr() << "\t"
		 << node->getChildId();
}

void CSVWriter :: registerNodeForId(BjoernNode *node)
{
	idToNode[curId] = node;
	addrToNode[node->getAddr()] = node;
	node->setId(curId);
}

void CSVWriter :: resetMaps()
{
	idToNode.clear();
}

CSVWriter :: ~CSVWriter()
{
	deinit();
}

void CSVWriter :: deinit()
{
	closeOutputFiles();
}

void CSVWriter :: closeOutputFiles()
{
	nodeFile.close();
	edgeFile.close();
}
