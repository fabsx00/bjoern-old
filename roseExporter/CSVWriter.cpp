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


void CSVWriter :: writeFunction(BjoernFunctionNode *func)
{
	writeFunctionNode(func);
}

void CSVWriter :: writeFunctionNode(BjoernFunctionNode *func)
{
	writeBjoernNode(func);


	nodeFile << "\t" << func->getName();

	nodeFile << endl;
}

void CSVWriter :: writeBjoernNode(BjoernNode *node)
{
	nodeFile << node->getType() << "\t"
		 << node->getAddr() << "\t"
		 << node->getChildId();
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
