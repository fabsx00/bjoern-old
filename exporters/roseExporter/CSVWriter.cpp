#include "CSVWriter.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <list>

using namespace std;

enum bjoernKey_t {
	TYPE,
	ADDR,
	CHILD_ID,
	CODE,
	FUNCTION_ID,
	N_KEYS
};

const char *keys[] = {
	"type",
	"addr",
	"childId",
	"code",
	"functionId"
};

// Public

void CSVWriter :: init()
{
	openOutputFiles();
	writeNodeHeader();
	writeEdgeHeader();
}

void CSVWriter :: writeFunction(BjoernFunctionNode *func)
{
	resetMaps();
	writeFunctionNode(func);
	func->setFunctionIdOfChildren();
	writeBasicBlocksOfFunc(func);
	writeInstructionsOfFunc(func);
	writeSymbolsOfFunc(func);

	connectFunctionToEntryBlock(func);
	connectBasicBlocksViaControlFlow(func);
	connectBasicBlocksToInstructions(func);
	connectBasicBlocksToSymbols(func);

}

// Private

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
		nodeFile << keys[i]
			 << ":string:nodeIndex"
			 << '\t';
	}
	nodeFile << keys[N_KEYS - 1]
		 << ":string:nodeIndex"
		 << endl;
}

void CSVWriter :: writeEdgeHeader()
{
	edgeFile << "start" << "\t"
		 << "end" << "\t"
		 << "type" << endl;
}

void CSVWriter :: writeFunctionNode(BjoernFunctionNode *func)
{
	writeBjoernNode(func);

	nodeFile << "\t" << func->getName();
	nodeFile << endl;
	finishNode(func);
}



void CSVWriter :: writeInstructionsOfFunc(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;

		// iterate over instructions

		const list<BjoernInstructionNode *> instructions = basicBlock->getInstructions();
		for(list<BjoernInstructionNode *> :: const_iterator it2 = instructions.begin();
		    it2 != instructions.end(); it2++){
			writeInstruction(*it2);
		}
	}
}

void CSVWriter :: writeInstruction(BjoernInstructionNode *instr)
{
	writeBjoernNode(instr);
	nodeFile << "\t\"" << instr->getCode() << "\"";
	nodeFile << endl;
	finishNode(instr);
}

void CSVWriter :: writeBasicBlocksOfFunc(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;
		writeBjoernNode(basicBlock);
		nodeFile<< "\t\t" << basicBlock->getFunctionId();
		nodeFile << endl;
		finishNode(basicBlock);
	}

}

void CSVWriter :: writeSymbolsOfFunc(BjoernFunctionNode *func)
{
	map<string, BjoernSymbolNode *> symbolToNode = func->getSymbolToNode();
	for(auto it : symbolToNode){
		// auto sym = it->first;
		auto node = it.second;
		writeBjoernNode(node);
		nodeFile << endl;
		finishNode(node);
	}
}

void CSVWriter :: connectFunctionToEntryBlock(BjoernFunctionNode *func)
{
	unsigned long long srcId = func->getId();
	string entryAddr = func->getAddr();

	BjoernBasicBlockNode *entryBlock = func->getEntryBlock();
	unsigned long long dstId = entryBlock->getId();

	writeEdge(srcId, dstId, "FUNCTION_OF_CFG");

}

void CSVWriter :: connectBasicBlocksViaControlFlow(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;
		connectBasicBlockToSuccessors(basicBlock);
	}

}

void CSVWriter :: connectBasicBlockToSuccessors(BjoernBasicBlockNode *basicBlock)
{
	const list<uint64_t> successors = basicBlock->getSuccessors();

	unsigned long long srcId = basicBlock->getId();

	BjoernFunctionNode *func = basicBlock->getFunc();

	for(list<uint64_t> :: const_iterator it = successors.begin();
	    it != successors.end(); it++){

		stringstream s; s << *it;
		BjoernBasicBlockNode *dstNode = func->getBasicBlockByAddr(s.str());
		if(!dstNode) continue;

		unsigned long long dstId = dstNode->getId();
		writeEdge(srcId, dstId, "FLOWS_TO");
	}

}

void CSVWriter :: connectBasicBlocksToInstructions(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;
		connectBasicBlockToItsInstructions(basicBlock);
	}
}

void CSVWriter :: connectBasicBlockToItsInstructions(BjoernBasicBlockNode *basicBlock)
{
	unsigned long srcId = basicBlock->getId();

	const list<BjoernInstructionNode *> instructions = basicBlock->getInstructions();
	for(list<BjoernInstructionNode *> :: const_iterator it = instructions.begin();
	    it != instructions.end(); it++){
		BjoernInstructionNode *instr = *it;
		unsigned long long dstId = instr->getId();
		writeEdge(srcId, dstId, "IS_BASIC_BLOCK_OF");
	}
}

void CSVWriter :: connectBasicBlocksToSymbols(BjoernFunctionNode *func)
{
// TODO
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
