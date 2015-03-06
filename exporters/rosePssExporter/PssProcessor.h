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

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;

namespace bjoern {

class PssProcessor : public SgSimpleProcessing
{
protected:
	static Sawyer::Message::Facility mlog;
	BaseSemantics::DispatcherPtr disp;

	virtual void initDispatcher(const MemoryMap* memMap=nullptr) = 0;

public:
	PssProcessor();
	PssProcessor(SgAsmGenericFile*);
	virtual ~PssProcessor() {}
	static void initDiagnostics();

	void visit(SgNode *node);
};

class PssProcessorX86 : public PssProcessor
{
protected:
	virtual void initDispatcher(const MemoryMap* memMap=nullptr);
};

} // namespace

#endif /* PSSEXPORTER_H_ */
