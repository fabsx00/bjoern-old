#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <inttypes.h>
#include <iostream>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <SymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>

#include "bjoernNodes.hpp"
#include "CSVWriter.hpp"
#include "bjoernUseDefAnalyzer.hpp"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace boost::algorithm;
using namespace std;

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
public:
	MyProcessor()
	{
	}

	~MyProcessor(){}

	void init(const SgAsmGenericFile* asmFile)
	{
		writer.init();
		useDefAnalyzer.init(asmFile);
	}

	/**
	   Called for each node of the "binary AST"
	   We only check whether the node is a function
	   node and then manually walk the tree from there.
	   That's not too efficient but shouldn't cause trouble.
	*/

	void visit(SgNode *node)
	{
		SgAsmFunction* func = isSgAsmFunction(node);
		if(!func) return;
		visitFunction(func);
	}

	void visitFunction(SgAsmFunction *func)
	{
		BjoernFunctionNode *bjoernFunc = createBjoernFuncFromSgFunc(func);
		writer.writeFunction(bjoernFunc);
		delete bjoernFunc;

		useDefAnalyzer.analyze(func);
	}

	BjoernFunctionNode *createBjoernFuncFromSgFunc(SgAsmFunction *func)
	{
		BjoernFunctionNode *bjoernFunc = new BjoernFunctionNode();

		if(!bjoernFunc)
			throw runtime_error("Out of memory");

		/* Initialize name and address */

		string name = func->get_name();
		SgAsmBlock *entryBlock = func->get_entry_block();
		assert(entryBlock != NULL);
		rose_addr_t entryAddr = entryBlock->get_address();

		bjoernFunc->setName(name);
		bjoernFunc->setAddr(entryAddr);

		visitStatements(func, bjoernFunc);
		return bjoernFunc;
	}

	/**
	   Iterate over statement list. SgASMStatements are actually basic blocks.
	*/

	void visitStatements(SgAsmFunction *func, BjoernFunctionNode *bjoernFunc)
	{

		// iterate over set of basic blocks

		SgAsmStatementPtrList statements = func->get_statementList();

		for(size_t i = 0; i < statements.size(); i++){
			SgAsmBlock *block = isSgAsmBlock(statements[i]);
			if(!block){
				// Not sure if this is ever reached.
				throw runtime_error("SgAsmStatements that are not blocks exist!");
			}

			visitBlock(block, bjoernFunc);
		}
	}

	/**
	   Called for each basic block:

	   Create a basic-block node and attach it to the function

	 */

	void visitBlock(SgAsmBlock *block, BjoernFunctionNode *bjoernFunc)
	{
		// create and add basic block to function

		BjoernBasicBlockNode *basicBlock = createBjoernBasicBlockFromSgBlock(block);
		bjoernFunc->addBasicBlock(basicBlock);
		basicBlock->setFunc(bjoernFunc);

	}

	BjoernBasicBlockNode *createBjoernBasicBlockFromSgBlock(SgAsmBlock *block)
	{
		BjoernBasicBlockNode *basicBlock = new BjoernBasicBlockNode();
		if(!basicBlock)
			throw runtime_error("Out of memory");

		basicBlock->setAddr(block->get_address());

		// Add successors

		SgAsmIntegerValuePtrList successors = block->get_successors();
		for(size_t j = 0; j < successors.size(); j++){
			basicBlock->addSuccessor(successors[j]->get_value());
		}


		// Add instructions to basic-block

		SgAsmStatementPtrList blockStmts = block->get_statementList();
		for(size_t j = 0; j < blockStmts.size(); j++){
			SgAsmStatement *stmt = blockStmts[j];
			SgAsmInstruction *instr = isSgAsmInstruction(stmt);
			if(!instr)
				continue;

			BjoernInstructionNode *instrNode = createBjoernInstructionFromSgInstruction(instr);
			basicBlock->addInstruction(instrNode);
		}

		return basicBlock;
	}

	BjoernInstructionNode *createBjoernInstructionFromSgInstruction(SgAsmInstruction *instr)
	{
		BjoernInstructionNode *instrNode = new BjoernInstructionNode();
		if(!instrNode)
			throw runtime_error("Out of memory");

		stringstream sstr;
		MyUnparser().unparse(sstr, instr);
		string code = sstr.str();
		trim(code);
		instrNode->setCode(code);
		instrNode->setAddr(instr->get_address());

		return instrNode;
	}

};


SgBinaryComposite *getBinary(SgProject *project)
{
	auto fileList = project->get_fileList();
	if (fileList.size() != 1) {
		cerr << "No or multiple input files given." << endl;
		return nullptr;
	}

	auto file = fileList[0];
	SgBinaryComposite *binFile = isSgBinaryComposite(file);
	if (fileList.size() != 1) {
		cerr << "No or multiple input files given." << endl;
		return nullptr;
	}

	return binFile;
}

/**

   Entry point for exporter.
   The exporter expects a path to a binary as its only argument.
 */

int main(int argc, char *argv[]) {

	/* Create a binary AST for the entire binary */
	SgProject* myProject = frontend(argc, argv);

	/* Get a reference to the root node */
	SgNode*  rootNode = dynamic_cast<SgNode*>(myProject);

	auto binFile = getBinary(myProject);
	if(binFile == nullptr)
		return 1;

	/* Traverse AST in post-order */

	MyProcessor mp;
	mp.init(binFile->get_binaryFile());
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
}
