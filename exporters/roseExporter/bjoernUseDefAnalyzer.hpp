#ifndef _BJOERN_USE_DEF_ANALYZER_HPP
#define _BJOERN_USE_DEF_ANALYZER_HPP

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

#include "bjoernNodes.hpp"
#include "tracePolicy.hpp"
#include "BasicBlockSummary.hpp"
#include "MyPartialSymbolicSemantics.h"


using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics;
using namespace boost::algorithm;
using namespace std;
using namespace Sawyer;
using namespace Container;
using namespace bjoern;

#define INVALID_ADDRESS (0)

class MyProcessor;

class BjoernUseDefAnalyzer{

protected:
	BaseSemantics::DispatcherPtr disp;
	MemoryMap memoryMap;
	const RegisterDictionary* dictReg;
	RiscOperatorsPtr opsRisc;
	TracePolicyX86* tp;

	map<uint64_t, bool> visited;
	list<rose_addr_t> path;
	map<uint64_t, BasicBlockSummary *> summaries;
	rose_addr_t curBBAddress;
	BjoernFunctionNode *curFunctionNode;

	MyProcessor *processor;

	void initMemoryMap(const SgAsmGenericFile *asmFile);
	void initDispatcher();
	void initTracePolicy();
	
	void traceCFG(const Graph<SgAsmBlock*>::VertexNode* entryNode);	
	void clearSummaries();   
	bool isTerminatingEdge(Graph<SgAsmBlock*>::EdgeNode edge,
			       uint64_t edgeId);
	
	void traceCFG_r(const Graph<SgAsmBlock*>::VertexNode* vertex,
			BaseSemantics::DispatcherPtr disp);
	
	void processBasicBlock(SgAsmBlock *basicBlock);	
	void updateBasicBlockSummary(BasicBlockSummary::ATTRIBUTES attributes);	
	void removeEntryInBasicBlockSummary(SgAsmBlock *basicBlock);
	BasicBlockSummary::ATTRIBUTES processStatements(SgAsmBlock *basicBlock);
	void processInstruction(SgAsmInstruction *instr);
	uint64_t edgeToId(Graph<SgAsmBlock*>::EdgeNode edge);
	rose_addr_t getAddressForNode(Graph<SgAsmBlock*>::VertexNode node);
			

public:
	void init(const SgAsmGenericFile* asmFile);
	void setProcessor(MyProcessor *proc);
	void analyze(SgAsmFunction *func, BjoernFunctionNode *funcNode);	

};

#endif
