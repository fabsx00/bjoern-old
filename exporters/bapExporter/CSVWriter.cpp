#include "CSVWriter.hpp"

#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;

enum bjoernKey_t {
	TYPE,
	ADDR,
	CHILD_ID,
	CODE,
	N_KEYS
};

const char *keys[] = {
	"type",
	"addr",
	"childId",
	"code"
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


void CSVWriter :: writeFunction(BjoernFunctionNode *func)
{
	resetMaps();
	writeFunctionNode(func);
	writeBasicBlocksOfFunc(func);
	writeInstructionsOfFunc(func);

	connectFunctionToEntryBlock(func);
	connectBasicBlocksViaControlFlow(func);
	connectBasicBlocksToInstructions(func);

	connectInstructionsViaDataFlow(func);
}

void CSVWriter :: writeInstructionsOfFunc(BjoernFunctionNode *func)
{
	const list<BjoernBasicBlockNode *> basicBlocks = func->getBasicBlocks();

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;

		// iterate over instructions

		list<BjoernInstructionNode *> instructions = basicBlock->getInstructions();
		for(list<BjoernInstructionNode *> :: const_iterator it2 = instructions.begin();
		    it2 != instructions.end(); it2++){
			(*it2)->setChildId(basicBlock->getId());
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

	// iterate over basic blocks

	for(list<BjoernBasicBlockNode *> :: const_iterator it =  basicBlocks.begin();
	    it != basicBlocks.end(); it++){
		BjoernBasicBlockNode *basicBlock = *it;
		basicBlock->setChildId(func->getId());
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


// Dummy; just calls connectInstructionsViaDataFlow for bbs
void CSVWriter::connectInstructionsViaDataFlow(BjoernFunctionNode *func)
 {
   for(auto &it : func->getBasicBlocks())
    {
     connectInstructionsViaDataFlow(it);
    }
 }

// Dummy; just calls connectInstructionsViaDataFlow for instr
void CSVWriter::connectInstructionsViaDataFlow(BjoernBasicBlockNode *basicBlock)
 {
   for(auto &it : basicBlock->getInstructions())
    {
     connectInstructionsViaDataFlow(it);
    }
 }

// Walk over data-successors of the instructions, print an edge for each
void CSVWriter::connectInstructionsViaDataFlow(BjoernInstructionNode *instr)
 {
	unsigned long long srcId = instr->getId();

	for(auto &dstNode : instr->getDataSuccessors())
	{
		unsigned long long dstId = dstNode->getId();
		writeEdge(srcId, dstId, "DATA_FLOW");
	}
 }



void CSVWriter :: connectBasicBlockToSuccessors(BjoernBasicBlockNode *basicBlock)
{
	const list<uint64_t> successors = basicBlock->getSuccessors();

	unsigned long long srcId = basicBlock->getId();

	for(list<uint64_t> :: const_iterator it = successors.begin();
	    it != successors.end(); it++){

		stringstream s; s << *it;
		map<string, BjoernNode *> :: iterator it2 =  addrToNode.find(s.str());
		if(it2 == addrToNode.end())
			continue;

		BjoernNode *dstNode = it2->second;
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
	if(node->getId() == 0)
 	{
		idToNode[curId] = node;
		addrToNode[node->getAddr()] = node;
		node->setId(curId);
	}
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

