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


public:
	CSVWriter() : curId(0) {}
	~CSVWriter();

	void init();
	void openOutputFiles();
	void writeNodeHeader();

	void deinit();
	void closeOutputFiles();

	void writeFunction(BjoernFunctionNode *func);
	void writeFunctionNode(BjoernFunctionNode *func);
	void writeBjoernNode(BjoernNode *node);
	void writeBasicBlocksOfFunc(BjoernFunctionNode *func);

};


#endif
