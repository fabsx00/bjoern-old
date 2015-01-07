#ifndef _CSV_WRITER_HPP
#define _CSV_WRITER_HPP

#include "bjoernNodes.hpp"

#include <fstream>
#include <map>

class CSVWriter
{

private:
	unsigned long long curId;
	ofstream nodeFile;
	ofstream edgeFile;

	map<unsigned long long, BjoernNode *> idToNode;
	map<string, BjoernNode *> addrToNode;
	/* We keep a nodeToId map implicitly by saving
	 ids inside nodes after writing */

	void finishNode(BjoernNode *node);
	void registerNodeForId(BjoernNode *node);
	void resetMaps();

	void writeFunctionNode(BjoernFunctionNode *func);
	void writeBjoernNode(BjoernNode *node);
	void writeBasicBlocksOfFunc(BjoernFunctionNode *func);
	void writeInstructionsOfFunc(BjoernFunctionNode *func);
	void writeInstruction(BjoernInstructionNode *instr);

	void connectFunctionToEntryBlock(BjoernFunctionNode *func);
	void connectBasicBlocksViaControlFlow(BjoernFunctionNode *func);
	void connectBasicBlockToSuccessors(BjoernBasicBlockNode *basicBlock);
	void connectBasicBlocksToInstructions(BjoernFunctionNode *func);
	void connectBasicBlockToItsInstructions(BjoernBasicBlockNode *basicBlock);

	void writeEdge(unsigned long long srcId, unsigned long long dstId,
		       const char *edgeType);

public:
	CSVWriter() : curId(0) {}
	~CSVWriter();

	void init();
	void openOutputFiles();
	void writeNodeHeader();
	void writeEdgeHeader();

	void deinit();
	void closeOutputFiles();

	void writeFunction(BjoernFunctionNode *func);

};


#endif
