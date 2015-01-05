#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

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
#include "bjoernFunction.hpp"
#include "CSVWriter.hpp"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace std;

class MyProcessor : public SgSimpleProcessing
{
private:	
	CSVWriter writer;
public:
	MyProcessor() { }

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
				
		BjoernFunction *bjoernFunc = createBjoernFuncFromSgFunc(func);					
		visitStatements(func, bjoernFunc);
		
		writer.writeFunction(bjoernFunc);
		delete bjoernFunc;
	}

	BjoernFunction *createBjoernFuncFromSgFunc(SgAsmFunction *func)
	{
		BjoernFunction *bjoernFunc = new BjoernFunction();		
		
		if(!bjoernFunc)
			throw runtime_error("Out of memory");
		
		/* Initialize name and address */
		
		string name = func->get_name();
		SgAsmBlock *entryBlock = func->get_entry_block();
		assert(entryBlock != NULL);		
		rose_addr_t entryAddr = entryBlock->get_address();
		
		bjoernFunc->setName(name);
		bjoernFunc->setAddr(entryAddr);
		
		
		SgSymbolTable * symbolTable = func->get_symbol_table();
						
		return bjoernFunc;
	}
	
	
	void visitStatements(SgAsmFunction *func, BjoernFunction *funcNode)
	{
				
		SgAsmStatementPtrList statements = func->get_statementList();				
		
		for(size_t i = 0; i < statements.size(); i++){
			SgAsmBlock *block = dynamic_cast<SgAsmBlock *>(statements[i]);			
			// AsmUnparser().unparse(std::cout, stmt);			
			SgAsmIntegerValuePtrList successors = block->get_successors();
			for(size_t j = 0; j < successors.size(); j++){
				cout << successors[j] << endl;
			}
		}
	}


};

int main(int argc, char *argv[]) {
	
	/* Create a binary AST for the entire binary */
	SgProject* myProject = frontend(argc, argv);
	
	/* Get a reference to the root node*/
	SgNode*  rootNode = dynamic_cast<SgNode*>(myProject);
	
	/* Traverse AST in post-order */
	
	MyProcessor mp;
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
}
