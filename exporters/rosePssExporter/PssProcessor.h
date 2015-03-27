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

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;
using namespace Sawyer;
using namespace Container;

namespace bjoern {

struct TracePolicy {
	virtual void startOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state) {}
	virtual void endOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state) {}
	virtual bool skipInstruction(const SgAsmInstruction* instr) { return false; }
	virtual ~TracePolicy() {}
};

class PssProcessor : public SgSimpleProcessing
{
protected:
	static Sawyer::Message::Facility mlog;
	BaseSemantics::DispatcherPtr disp;
	TracePolicy* tracePolicy;

	virtual void initDispatcher(const MemoryMap* memMap=nullptr) = 0;

public:
	PssProcessor();
	virtual ~PssProcessor() {}
	void setTracePolicy(TracePolicy* tracePolicy);
	static void initDiagnostics();

	void visit(SgNode *node);
};

class TracePolicyX86 : public TracePolicy {
public:
	virtual void startOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state);
	virtual void endOfBb(const SgAsmBlock* bb, const size_t idTrace, BaseSemantics::StatePtr state);
	virtual bool skipInstruction(const SgAsmInstruction* instr);
};

class PssProcessorX86 : public PssProcessor
{
protected:
	virtual void initDispatcher(const MemoryMap* memMap=nullptr);
public:
	PssProcessorX86(const SgAsmGenericFile* input);
};


} // namespace

#endif /* PSSEXPORTER_H_ */
