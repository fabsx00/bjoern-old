#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <inttypes.h>
#include <iostream>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <PartialSymbolicSemantics2.h>
#include <SymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>

#include "MyProcessor.hpp"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace boost::algorithm;
using namespace std;


SgBinaryComposite *getBinary(SgProject *project)
{
	auto fileList = project->get_fileList();
	if (fileList.size() != 1) {
		cerr << "No or multiple input files given." << endl;
		return nullptr;
	}

	auto file = fileList[0];
	SgBinaryComposite *binFile = isSgBinaryComposite(file);
	if (fileList.size() != 1) {
		cerr << "No or multiple input files given." << endl;
		return nullptr;
	}

	return binFile;
}

/**

   Entry point for exporter.
   The exporter expects a path to a binary as its only argument.
 */

int main(int argc, char *argv[]) {

	/* Create a binary AST for the entire binary */
	SgProject* myProject = frontend(argc, argv);

	/* Get a reference to the root node */
	SgNode*  rootNode = dynamic_cast<SgNode*>(myProject);

	auto binFile = getBinary(myProject);
	if(binFile == nullptr)
		return 1;

	/* Traverse AST in post-order */

	MyProcessor mp;
	mp.init(binFile->get_binaryFile());
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
}
