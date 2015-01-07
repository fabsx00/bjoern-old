#ifndef _CSV_WRITER_HPP
#define _CSV_WRITER_HPP

#include "bjoernNodes.hpp"

#include <fstream>

class CSVWriter
{

private:
	unsigned long long curId;
	ofstream nodeFile;
	ofstream edgeFile;

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
