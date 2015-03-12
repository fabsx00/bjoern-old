/*
 * PssExporter.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: felix
 */

#include <rose.h>
#include <BinaryControlFlow.h>
#include <PartialSymbolicSemantics2.h>
#include <DispatcherX86.h>
#include <sawyer/GraphTraversal.h>

#include "PssProcessor.h"

using namespace Diagnostics;
using namespace Sawyer;
using namespace Container;

#define _DEBUG

namespace bjoern {

Sawyer::Message::Facility PssProcessor::mlog;

void PssProcessor::initDiagnostics() {
	static bool initialized = false;
	if (initialized) return;
	mlog = Sawyer::Message::Facility("bjoern::PssProcessor", Diagnostics::destination);
	Diagnostics::mfacilities.insertAndAdjust(mlog);
	initialized = true;
}

PssProcessor::PssProcessor() {
	initDiagnostics();
}

static void processBb(BaseSemantics::DispatcherPtr disp, const SgAsmBlock* block) {
	auto statements = block->get_statementList();
	for (auto statement : statements) {
		auto instr = isSgAsmInstruction(statement);
		disp->processInstruction(instr);
	}
}

static void createTrace(const Graph<SgAsmBlock*>::VertexNode* vertex, BaseSemantics::DispatcherPtr disp, size_t& idTrace) {
	// process bb
	processBb(disp, vertex->value());
	// save the state
	auto state = disp->get_state();
	auto savedState = state->clone();
	// TODO: save the state persistently somewhere ;-)

	// traverse all edges (there should be at most two static ones.)
	// TODO: how do we handle switch-case statements?
	for (auto& edge : vertex->outEdges()) {
		if (edge.isSelfEdge()) continue;
		auto targetVertex = *edge.target();

		disp->get_operators()->set_state(savedState->clone());
		createTrace(&targetVertex, disp, idTrace);
		idTrace++;
	}
}

void PssProcessor::visit(SgNode *node) {
	SgAsmFunction* f = isSgAsmFunction(node);
	if (f == NULL) return;

#ifdef _DEBUG
	// only consider main()
	if (f->get_name() != "main") return;
#endif

	/* Traverse all possible paths through the CFG in a depth-first search (DFS) manner.
	 * Each path is assigned an ID. For each path the state is saved for each basic block.
	 * Loops are not considered (the DFS traversal stops when a basic block is reached for the second time).
	 */

	// Get CFG of the function and create corresponding DFS traversal.
	// TODO: we want to have a CFG that ends basic blocks not only at jump but also at call instructions.
	Graph<SgAsmBlock*> cfg;
	ControlFlow().build_block_cfg_from_ast(f, cfg);
	// XXX: does the start vertex always have ID 0?
	auto startVertex = *cfg.findVertex(0);
	// Start trace 0 from the start bb.
	disp->get_state()->clear();
	size_t idTrace = 0;
	createTrace(&startVertex, disp, idTrace);
	// TODO: create a useful data-flow data structure from the generated traces/states.
}

/* --- PssProcessorX86 --- */
void PssProcessorX86::initDispatcher(const MemoryMap* memMap) {
	const RegisterDictionary* dictReg = RegisterDictionary::dictionary_pentium4();
	auto opsRisc = PartialSymbolicSemantics::RiscOperators::instance(dictReg);
	if (memMap!=nullptr)
	{
		opsRisc->set_memory_map(memMap);
	}
	disp = DispatcherX86::instance(opsRisc);
}

PssProcessorX86::PssProcessorX86(const SgAsmGenericFile* asmFile) {
	// create memory map for given binary
	MemoryMap map;
	auto sections = asmFile->get_mapped_sections();
	for (auto section : sections)
	{
		rose_addr_t size = section->get_size();
		rose_addr_t mappedSize = section->get_mapped_size();
		rose_addr_t va = section->get_mapped_preferred_va();

		if (size == 0) continue;

		mlog[TRACE] << "Adding section: "<< section->get_name()->get_string() << " va: " << std::hex << va << ", size: " << size << ", mapped size: " << mappedSize << std::endl;
		uint8_t* data = new uint8_t[mappedSize];
		memset(data, 0, mappedSize);
		if (section->read_content(0, data, size) != size)
		{
			mlog[ERROR] << "Failed to access section \"" << section->get_name()->get_string() << "\".";
			continue;
		}

		// no need to free data buffer, because buff is ref counted (right?)
		auto buff = MemoryMap::StaticBuffer::instance(data, mappedSize);
		auto interval = AddressInterval::baseSize(va, mappedSize);
		map.insert(interval,  MemoryMap::Segment(buff, 0, MemoryMap::READ_WRITE_EXECUTE));
	}
	initDispatcher(&map);
}

} // namespace
