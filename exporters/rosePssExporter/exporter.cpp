/*
 * exporter.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: felix
 */

#define __STDC_FORMAT_MACROS

#include <inttypes.h>
#include <memory>
#include <utility>
#include <iostream>
#include <vector>

/* --- BOOST --- */
//#include <boost/graph/graphviz.hpp>
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>

/* --- ROSE --- */
#include <rose.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <Diagnostics.h>

#include "PssProcessor.h"

using namespace rose;
using namespace Diagnostics;


int main(int argc, char** argv) {

	for(int i=0; i<argc; i++) {
		char* a = argv[i];
		std::cout << a << std::endl;
	}

	// init logging
	Diagnostics::initialize();
	bjoern::PssProcessor::initDiagnostics();

	// parse inputs
	SgProject* proj = frontend(argc, argv);
	auto fileList = proj->get_fileList();
	if (fileList.size() != 1) {
		mlog[ERROR] << "No or multiple input files given.";
		return 0;
	}

	// init processor
	auto file = fileList[0];
	auto binFile = isSgBinaryComposite(file);
	if (binFile == nullptr) {
		mlog[ERROR] << "Input file is not in a known executable file format.";
		return 0;
	}
	bjoern::PssProcessor proc(binFile->get_binaryFile());
	SgNode* rootNode = dynamic_cast<SgNode*>(proj);
	t_traverseOrder order = postorder;

	// process callgraph
	proc.traverse(rootNode, order);
	return 0;
}


