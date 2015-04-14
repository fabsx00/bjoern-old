#ifndef _MY_PROCESSOR_HPP
#define _MY_PROCESSOR_HPP

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
#include "CSVWriter.hpp"
#include "bjoernUseDefAnalyzer.hpp"

/**
   The ROSE ASMUnparser is configured by
   sub-classing and modifying callbacks.
   The class below will output only the
   disassembled instruction and omit
   the byte-sequence and character dump.
*/

class MyUnparser : public AsmUnparser {

public:

	MyUnparser()
	{
		insn_callbacks.pre.clear();
		insn_callbacks.post.clear();
	}

};

class MyProcessor : public SgSimpleProcessing
{
private:
	CSVWriter writer;
	BjoernUseDefAnalyzer useDefAnalyzer;
	BjoernFunctionNode *curFunction;
public:
	MyProcessor() : SgSimpleProcessing() {}
	virtual ~MyProcessor();

	void init(const SgAsmGenericFile* asmFile);
	void visit(SgNode *node);
	void visitFunction(SgAsmFunction *func);
	BjoernFunctionNode *createBjoernFuncFromSgFunc(SgAsmFunction *func);
	void visitStatements(SgAsmFunction *func, BjoernFunctionNode *bjoernFunc);
	void visitBlock(SgAsmBlock *block, BjoernFunctionNode *bjoernFunc);
	BjoernBasicBlockNode *createBjoernBasicBlockFromSgBlock(SgAsmBlock *block);
	BjoernInstructionNode *createBjoernInstructionFromSgInstruction(SgAsmInstruction *instr);

};

#endif
