#ifndef _CSV_WRITER_HPP
#define _CSV_WRITER_HPP

#include "BasicBlockSummary.hpp"
#include "bjoernNodes.hpp"

#include <rose.h>

#include <fstream>
#include <map>
#include <list>

using namespace std;
using namespace bjoern;

class CSVWriter
{
public:
	CSVWriter() : curId(0) {}
	~CSVWriter();

	void init();
	void writeFunction(BjoernFunctionNode *func);

private:
	unsigned long long curId;
	ofstream nodeFile;
	ofstream edgeFile;

	map<unsigned long long, BjoernNode *> idToNode;
	/* We keep a nodeToId map implicitly by saving
	 ids inside nodes after writing */

	void openOutputFiles();
	void closeOutputFiles();
	void writeNodeHeader();
	void writeEdgeHeader();

	void finishNode(BjoernNode *node);
	void registerNodeForId(BjoernNode *node);
	void resetMaps();

	void writeFunctionNode(BjoernFunctionNode *func);
	void writeBjoernNode(BjoernNode *node);
	void writeBasicBlocksOfFunc(BjoernFunctionNode *func);
	void writeInstructionsOfFunc(BjoernFunctionNode *func);
	void writeInstruction(BjoernInstructionNode *instr);
	void writeSymbolsOfFunc(BjoernFunctionNode *func);

	void connectFunctionToEntryBlock(BjoernFunctionNode *func);
	void connectBasicBlocksViaControlFlow(BjoernFunctionNode *func);
	void connectBasicBlockToSuccessors(BjoernBasicBlockNode *basicBlock);
	void connectBasicBlocksToInstructions(BjoernFunctionNode *func);
	void connectBasicBlockToItsInstructions(BjoernBasicBlockNode *basicBlock);
	void connectBasicBlocksToSymbols(BjoernFunctionNode *func);
	void connectBasicBlockToSymbols(BjoernBasicBlockNode *basicBlock);

	void writeEdge(unsigned long long srcId, unsigned long long dstId,
		       const char *edgeType);

	void deinit();

};

#endif
