#include "CSVWriter.hpp"

#include <iostream>
using namespace std;

enum bjoernKey_t {
	TYPE,
	ADDR,
	CHILD_ID
};

const char *keys[] = {
	"type",
	"addr",
	"childId",
};

void CSVWriter :: init()
{
	openOutputFiles();
}

void CSVWriter :: openOutputFiles()
{
	// TODO
}


void CSVWriter :: writeFunction(BjoernFunctionNode *func)
{
	// TODO
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
	// TODO
}
