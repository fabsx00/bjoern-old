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
#include <utility>

#include "PssProcessor.h"

using namespace Diagnostics;

namespace bjoern {

Sawyer::Message::Facility PssProcessor::mlog;

/*
 * Tracing
 */
std::ostream& operator<<(std::ostream& os, const SummaryMap& summaryMap) {
	for (const auto& entry : summaryMap) {
		os << "+++ Basic block: " << std::hex << entry.first << " +++\n";
		os << entry.second;
	}
	return os;
}

static BasicBlockSummary::ATTRIBUTES processBb(BaseSemantics::DispatcherPtr disp, const SgAsmBlock* block, TracePolicy* policy) {

	// invoke pre-callback from policy.
	//// policy->startOfBb(block, disp->get_state());
	auto statements = block->get_statementList();
	for (auto statement : statements) {
		auto instr = isSgAsmInstruction(statement);
		if (policy->isCall(instr)) {
			// we don't execute calls.
			return BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL;
		}
		if (policy->isReturn(instr)) {
			// we don't execute rets either.
			return BasicBlockSummary::ATTRIBUTES::ENDS_IN_RET;
		}

		disp->processInstruction(instr);
	}
	// invoke post-callback from policy.
	//// policy->endOfBb(block, disp->get_state());
	return BasicBlockSummary::ATTRIBUTES::NONE;
}

static void createTrace(const Graph<SgAsmBlock*>::VertexNode* vertex, BaseSemantics::DispatcherPtr disp, size_t& idTrace, SummaryMap& summaries, TracePolicy* policy) {
	// check if the bb has already been visited for this traces.
	SgAsmBlock* bb = vertex->value();
	auto& summaryBb = summaries[bb->get_address()];
	auto& statesBb = summaryBb.sm;
	if (statesBb.find(idTrace) != statesBb.end()) {
		// ok, we have been here before in this traces, abort...
		PssProcessor::mlog[DEBUG] << "Looped to basic block " << vertex->id() << " in trace " << idTrace << ", ending trace here.\n";
		return;
	}

	// process bb
	summaryBb.attributes = processBb(disp, bb, policy);
	statesBb[idTrace] = disp->get_state()->clone();
	if (summaryBb.attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL) {
		// bb ends in call...
		PssProcessor::mlog[DEBUG] << "Vertex " << vertex->id() << " is call.\n";
		// apply our calling convention policy.
		auto preCallState = disp->get_state();
		policy->derivePostCallState(preCallState);
	}
	// save the state
	auto statePassOn = disp->get_state();
	// and add it to the states map

	// traverse all edges (there should be at most two static ones.)
	// TODO: how do we handle switch-case statements?
	size_t tempIdTrace = idTrace;
	for (auto& edge : vertex->outEdges()) {
		auto targetVertex = *edge.target();

		disp->get_operators()->set_state(statePassOn->clone());
		createTrace(&targetVertex, disp, idTrace, summaries, policy);
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

PssProcessor::PssProcessor() : tracePolicy(nullptr), collector(nullptr) {
	initDiagnostics();
}

void PssProcessor::visit(SgNode *node) {

	/* Traverses all possible paths through a function's CFG in a depth-first search (DFS) manner.
	 * Each path is assigned an ID. For each path the state is saved for each basic block.
	 * Loops are not considered (the DFS traversal stops when a basic block is reached for the second time).
	 */

	assert(tracePolicy != nullptr);
	assert(collector != nullptr);

	SgAsmFunction* f = isSgAsmFunction(node);
	if (f == NULL) return;
	mlog[TRACE] << "Processing function " << f->get_name() << std::endl;
#if 0
	// DEBUG: only consider main()
	if (f->get_name() != "main") return;
#endif

	// get CFG of the function and create corresponding DFS traversal.
	Graph<SgAsmBlock*> cfg;
	ControlFlow().build_block_cfg_from_ast(f, cfg);
	// get the start bb.
	auto startBb = *cfg.findVertex(0);
	// start trace 0 from the start bb.
	disp->get_state()->clear();
	size_t idTrace = 0;
	SummaryMap summaries;
	createTrace(&startBb, disp, idTrace, summaries, tracePolicy);
	// finally, pass the summary map to the collector.
	collector->addSummaryMap(f->get_name(), std::move(summaries));
}

void PssProcessor::setTracePolicy(TracePolicy* tracePolicy) {
	this->tracePolicy = tracePolicy;
}

void PssProcessor::setCollector(ISummaryMapCollector* collector) {
	this->collector = collector;
}

/*
 * TracePolicyX86
 */

void TracePolicyX86::startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {
	// anything?
}

void TracePolicyX86::endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {
	// anything?
}

void TracePolicyX86::derivePostCallState(BaseSemantics::StatePtr preCallState) {

	/* In cdecl eax, ecx, and edx are callee-saved.
	 * Therefore they must be considered as unknown after a call.
	 */
	auto valUndef = preCallState->get_protoval()->undefined_(descEax->get_nbits());
	preCallState->writeRegister(*descEax, valUndef, ops.get());

	valUndef = preCallState->get_protoval()->undefined_(descEcx->get_nbits());
	preCallState->writeRegister(*descEcx, valUndef, ops.get());

	valUndef = preCallState->get_protoval()->undefined_(descEdx->get_nbits());
	preCallState->writeRegister(*descEdx, valUndef, ops.get());
}

bool TracePolicyX86::isCall(const SgAsmInstruction* instr) {
	return isSgAsmX86Instruction(instr)->get_kind() == x86_call;
}

bool TracePolicyX86::isReturn(const SgAsmInstruction* instr) {
	return isSgAsmX86Instruction(instr)->get_kind() == x86_ret;
}

TracePolicyX86::TracePolicyX86(const BaseSemantics::RiscOperatorsPtr ops) : ops(ops) {
	const RegisterDictionary* dictReg = RegisterDictionary::dictionary_pentium4();
	descEax = dictReg->lookup("eax");
	descEcx = dictReg->lookup("ecx");
	descEdx = dictReg->lookup("edx");
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
		rose_addr_t va = section->get_mapped_preferred_va();

		if (size == 0) continue;

		mlog[TRACE] << "Adding section: "<< section->get_name()->get_string() << " va: " << std::hex << va << ", size: " << size << ", mapped size: " << section->get_mapped_size() << std::endl;
		uint8_t* data = new uint8_t[size];
		memset(data, 0, size);
		if (section->read_content(0, data, size) != size)
		{
			mlog[ERROR] << "Failed to access section \"" << section->get_name()->get_string() << "\".";
			continue;
		}

		// no need to free data buffer, because buff is ref counted (right?)
		auto buff = MemoryMap::StaticBuffer::instance(data, size);
		auto interval = AddressInterval::baseSize(va, size);
		map.insert(interval,  MemoryMap::Segment(buff, 0, MemoryMap::READ_WRITE_EXECUTE));
	}
	initDispatcher(&map);
	TracePolicyX86* tp = new TracePolicyX86(disp->get_operators());
	setTracePolicy(tp);
}

PssProcessorX86::~PssProcessorX86() {
	delete tracePolicy;
}
} // namespace
