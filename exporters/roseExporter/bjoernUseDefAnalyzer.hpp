#ifndef _BJOERN_USE_DEF_ANALYZER_HPP
#define _BJOERN_USE_DEF_ANALYZER_HPP

#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <inttypes.h>
#include <iostream>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <SymbolicSemantics2.h>
#include <PartialSymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>

#include <BaseSemantics2.h>

#include "bjoernNodes.hpp"
#include "CSVWriter.hpp"
#include "tracePolicy.hpp"

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

class BjoernUseDefAnalyzer{

protected:
	BaseSemantics::DispatcherPtr disp;
	MemoryMap memoryMap;
	const RegisterDictionary* dictReg;
	RiscOperatorsPtr opsRisc;
	TracePolicyX86* tp;

	map<uint64_t, bool> visited;

	void initMemoryMap(const SgAsmGenericFile *asmFile)
	{
		auto sections = asmFile->get_mapped_sections();
		for (auto section : sections)
		{
			rose_addr_t size = section->get_size();
			rose_addr_t va = section->get_mapped_preferred_va();
			if (size == 0 || va == INVALID_ADDRESS)
				continue;

			uint8_t* data = new uint8_t[size];
			memset(data, 0, size);

			if (section->read_content(0, data, size) != size)
			{
				cerr << "Error reading section" << endl;
				continue;
			}

			// no need to free data buffer, because buff is ref counted (right?)
			auto buff = MemoryMap::StaticBuffer::instance(data, size);
			auto interval = AddressInterval::baseSize(va, size);
			memoryMap.insert(interval,  MemoryMap::Segment(buff, 0, MemoryMap::READ_WRITE_EXECUTE));
		}
	}

	void initDispatcher()
	{
		dictReg = RegisterDictionary::dictionary_pentium4();
		opsRisc = PartialSymbolicSemantics::RiscOperators::instance(dictReg);
		opsRisc->set_memory_map(&memoryMap);
		disp = DispatcherX86::instance(opsRisc);
	}

	void initTracePolicy()
	{
		 tp = new TracePolicyX86(disp->get_operators());
	}


	/*
	   Traverses all possible paths through a function's CFG in a
	   depth-first search (DFS) manner. Each path is assigned an
	   ID. For each path the state is saved for each basic block.
	   Loops are not considered (the DFS traversal stops when a basic
	   block is reached for the second time).
	*/

	void traceCFG(const Graph<SgAsmBlock*>::VertexNode* entryNode)
	{
		visited.clear();

		traceCFG_r(entryNode, disp);
	}


	bool isTerminatingEdge(Graph<SgAsmBlock*>::EdgeNode edge,
			       uint64_t edgeId)
	{
		if( visited.find(edgeId) != visited.end()){
				// call 'complete' function here
				return true;
		}

		return false;
	}


	void traceCFG_r(const Graph<SgAsmBlock*>::VertexNode* vertex,
			   BaseSemantics::DispatcherPtr disp)
	{
		SgAsmBlock* bb = vertex->value();


		bool thereAreEdges = false;
		for (auto& edge : vertex->outEdges()) {
			thereAreEdges = true;

			uint64_t edgeId = edgeToId(edge);
			visited[edgeId] = true;

			if(isTerminatingEdge(edge, edgeId)){
				// call 'complete' function
				continue;
			}

			auto targetVertex = *edge.target();
			traceCFG_r(&targetVertex, disp);

			visited.erase(edgeId);
		}

		if(!thereAreEdges){
			// reached a node with no outgoing edges
			// call 'complete' function'
		}

	}

	/**
	   Create an id from and edges source and target.
	   We assume that only a single edge can exist between
	   a source and a target, i.e., the CFG is not a multigraph.
	*/

	uint64_t edgeToId(Graph<SgAsmBlock*>::EdgeNode edge)
	{
		// we assume here that rose_addr_t is 32 bit
		auto src_addr = (*edge.source()).value()->get_address();
		auto dst_addr = (*edge.target()).value()->get_address();
		return ((uint64_t) src_addr << 32) | dst_addr;
	}



public:

	void init(const SgAsmGenericFile* asmFile)
	{
		initMemoryMap(asmFile);
		initDispatcher();
	}

	void analyze(SgAsmFunction *func)
	{
		Graph<SgAsmBlock*> cfg;
		ControlFlow().build_block_cfg_from_ast(func, cfg);

		auto entryNode = *cfg.findVertex(0);
		traceCFG(&entryNode);
	}
};

#endif
