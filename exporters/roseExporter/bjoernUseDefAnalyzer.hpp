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
	list<rose_addr_t> path;

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
	  Traverse all possible paths in a CFG. A depth first search
	  is performed where no edge is ever expanded twice. When a
	  node is reached where no more edges can be expanded, a
	  callback function is invoked, allowing the path to be
	  processed.
	*/

	void traceCFG(const Graph<SgAsmBlock*>::VertexNode* entryNode)
	{
		visited.clear();
		path.clear();
		traceCFG_r(entryNode, disp);
	}


	bool isTerminatingEdge(Graph<SgAsmBlock*>::EdgeNode edge,
			       uint64_t edgeId)
	{
		// has edge already been expanded?

		if( visited.find(edgeId) != visited.end())
				return true;
		return false;
	}


	void traceCFG_r(const Graph<SgAsmBlock*>::VertexNode* vertex,
			   BaseSemantics::DispatcherPtr disp)
	{
		SgAsmBlock* bb = vertex->value();

		unsigned int nEdgesExpanded = 0;

		for (auto& edge : vertex->outEdges()) {

			uint64_t edgeId = edgeToId(edge);
			visited[edgeId] = true;

			if(isTerminatingEdge(edge, edgeId)){
				// call 'complete' function
				continue;
			}

			nEdgesExpanded ++;
			auto targetVertex = *edge.target();

			path.push_back(getAddressForNode(*vertex));
			traceCFG_r(&targetVertex, disp);
			path.pop_back();

			visited.erase(edgeId);
		}

		if(nEdgesExpanded == 0){
			// reached a node where no more edges
			// were expandable.
			// call 'complete'
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
		auto src_addr = getAddressForNode(*edge.source());
		auto dst_addr = getAddressForNode(*edge.target());
		return ((uint64_t) src_addr << 32) | dst_addr;
	}


	rose_addr_t getAddressForNode(Graph<SgAsmBlock*>::VertexNode node)
	{
		return node.value()->get_address();
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
