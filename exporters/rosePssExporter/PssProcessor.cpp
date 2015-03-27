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
#include <map>
#include <iostream>

#include "PssProcessor.h"

using namespace Diagnostics;

#define _DEBUG

namespace bjoern {

Sawyer::Message::Facility PssProcessor::mlog;

/*
 * Tracing
 */

// TODO: this should probably be put into a separate class.

typedef std::map<size_t, std::map<size_t, BaseSemantics::StatePtr>> StateMap;

std::ostream& operator<<(std::ostream& os, const StateMap& sm) {
	for (const auto& vertexEntry : sm) {
		os << "Vertex: " << vertexEntry.first << std::endl;
		for (const auto& traceEntry : vertexEntry.second) {
			os << "\tTrace: " << traceEntry.first << std::endl;
			os << *(traceEntry.second) << std::endl;
		}
	}
	return os;
}

static void processBb(BaseSemantics::DispatcherPtr disp, const SgAsmBlock* block, TracePolicy* policy) {
	auto statements = block->get_statementList();
	for (auto statement : statements) {
		auto instr = isSgAsmInstruction(statement);
		if (policy->skipInstruction(instr)) continue;
		disp->processInstruction(instr);
	}
}

static void createTrace(const Graph<SgAsmBlock*>::VertexNode* bb, BaseSemantics::DispatcherPtr disp, size_t& idTrace, StateMap& states, TracePolicy* policy) {
	// check if the bb has already been visited for this traces.
	auto& statesBb = states[bb->id()];
	if (statesBb.find(idTrace) != statesBb.end()) {
		// ok, we have been here before in this traces, abort...
		mlog[DEBUG] << "Looped to basic block " << bb->id() << " in trace " << idTrace << ", ending trace here.\n";
		return;
	}
	// process bb
	//// invoke pre-callback from policy.
	policy->startOfBb(bb->value(), idTrace, disp->get_state());
	processBb(disp, bb->value(), policy);
	//// invoke post-callback from policy.
	policy->endOfBb(bb->value(), idTrace, disp->get_state());

	// save the state
	auto state = disp->get_state();
	auto savedState = state->clone();
	// and add it to the states map
	statesBb[idTrace] = state->clone();

	// traverse all edges (there should be at most two static ones.)
	// TODO: how do we handle switch-case statements?
	size_t tempIdTrace = idTrace;
	for (auto& edge : bb->outEdges()) {
		auto targetVertex = *edge.target();

		disp->get_operators()->set_state(savedState->clone());
		createTrace(&targetVertex, disp, idTrace, states, policy);
		idTrace++;
	}
	// was the id incremented?
	if (idTrace > tempIdTrace) {
		// if so, undo the last increment.
		idTrace--;
	}
}

/*
 * PssProcessor
 */
void PssProcessor::initDiagnostics() {
	static bool initialized = false;
	if (initialized) return;
	mlog = Sawyer::Message::Facility("bjoern::PssProcessor", Diagnostics::destination);
	Diagnostics::mfacilities.insertAndAdjust(mlog);
	initialized = true;
}

PssProcessor::PssProcessor() {
	initDiagnostics();
	tracePolicy = nullptr;
}

void PssProcessor::visit(SgNode *node) {
	assert(tracePolicy != nullptr);
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

	// get CFG of the function and create corresponding DFS traversal.
	// TODO: we want to have a CFG that ends basic blocks not only at jump but also at call instructions.
	Graph<SgAsmBlock*> cfg;
	ControlFlow().build_block_cfg_from_ast(f, cfg);
	// get the start bb.
	auto startBb = *cfg.findVertex(0);
	// start trace 0 from the start bb.
	disp->get_state()->clear();
	size_t idTrace = 0;
	StateMap states;
	createTrace(&startBb, disp, idTrace, states, tracePolicy);
	mlog[DEBUG] << "States for function " << std::hex << f->get_address() << std::endl << states;

	// TODO: create a useful data-flow data structure from the generated traces/states.
}

void PssProcessor::setTracePolicy(TracePolicy* tracePolicy) {
	this->tracePolicy = tracePolicy;
}

/*
 * TracePolicyX86
 */

void TracePolicyX86::startOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state) {
	// anything?
}

void TracePolicyX86::endOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state) {
	// anything?
}

bool TracePolicyX86::skipInstruction(const SgAsmInstruction* instr) {
	// we don't want to actually execute call instructions.
	return "call" == instr->get_mnemonic();
}

/*
 * PssProcessorX86
 */
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
