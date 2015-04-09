/*
 * PssExporter.h
 *
 *  Created on: Mar 5, 2015
 *      Author: felix
 */

#ifndef PSSEXPORTER_H_
#define PSSEXPORTER_H_

/* --- ROSE --- */
#include <Cxx_Grammar.h>
#include <Diagnostics.h>
#include <BaseSemantics2.h>
#include <sawyer/GraphTraversal.h>

#include "BasicBlockSummary.h"
#include "Trace.h"

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;
using namespace Sawyer;
using namespace Container;

#define INVALID_ADDRESS 0

namespace bjoern {

class ITraceCollector {
public:
	virtual void addTrace(const SgAsmFunction*, TracePtr) = 0;
	virtual ~ITraceCollector() {}
};

struct TracePolicy {
	virtual void startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {}
	virtual void endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {}

	virtual void derivePostCallState(BaseSemantics::StatePtr preCallState) {}
	virtual bool isReturn(const SgAsmInstruction* instr) = 0;
	virtual bool isCall(const SgAsmInstruction* instr) = 0;
	virtual ~TracePolicy() {}
};

class PssProcessor : public SgSimpleProcessing
{
protected:
	BaseSemantics::DispatcherPtr disp;
	TracePolicy* tracePolicy;
	ITraceCollector* collector;
	SgAsmFunction* currentFunction;

	virtual void initDispatcher(const MemoryMap* memMap=nullptr) = 0;

	BasicBlockSummary::ATTRIBUTES processBb(const SgAsmBlock* block);
	void trace(const Graph<SgAsmBlock*>::VertexNode* vertex, TracePtr currentTrace);
	static Sawyer::Message::Facility mlog;

public:
	PssProcessor();
	virtual ~PssProcessor() {}
	void setTracePolicy(TracePolicy* tracePolicy);
	void setCollector(ITraceCollector* collector);
	static void initDiagnostics();

	void visit(SgNode *node);
};

class TracePolicyX86 : public TracePolicy {
/*!
 * Specific to  Linux, cdecl, and x86.
 */
protected:
	const BaseSemantics::RiscOperatorsPtr ops;
	const RegisterDescriptor* descEax;
	const RegisterDescriptor* descEcx;
	const RegisterDescriptor* descEdx;

public:
	TracePolicyX86(const BaseSemantics::RiscOperatorsPtr ops);
	virtual void startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state);
	virtual void endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state);
	virtual bool isCall(const SgAsmInstruction* instr);
	virtual bool isReturn(const SgAsmInstruction* instr);
	virtual void derivePostCallState(BaseSemantics::StatePtr preCallState);
};

class PssProcessorX86 : public PssProcessor
{
protected:
	MemoryMap map;
	virtual void initDispatcher(const MemoryMap* memMap=nullptr);
public:
	explicit PssProcessorX86(const SgAsmGenericFile* input);
	virtual ~PssProcessorX86();
};


} // namespace

#endif /* PSSEXPORTER_H_ */
