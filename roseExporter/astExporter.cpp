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

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace std;

class MyProcessor : public SgSimpleProcessing
{
private:
	list<BjoernFunction *> functions;
	unsigned long long curId = 0;
public:
	MyProcessor()
	{
		
	}
	
	void visit(SgNode *node)
	{		
	      	SgAsmFunction* func = isSgAsmFunction(node);
		if(!func) return;
		
		visitFunction(func);
		
	}

	void visitFunction(SgAsmFunction *func)
	{
				
		BjoernFunction *funcNode = createBjoernFuncFromSgFunc(func);					
		functions.push_back(funcNode);		
	}

	BjoernFunction *createBjoernFuncFromSgFunc(SgAsmFunction *func)
	{
		BjoernFunction *funcNode = new BjoernFunction();
		funcNode->setId(curId);
		curId++;

		if(!funcNode)
			throw runtime_error("Out of memory");
		
		/* Initialize name and address */
		
		string name = func->get_name();
		SgAsmBlock *entryBlock = func->get_entry_block();
		assert(entryBlock != NULL);		
		rose_addr_t entryAddr = entryBlock->get_address();
		
		funcNode->setName(name);
		funcNode->setAddr(entryAddr);

		/* Initialize statement nodes */
		
		SgAsmStatementPtrList statements = func->get_statementList();				
		SgSymbolTable * symbolTable = func->get_symbol_table();
		
		
		for(size_t i = 0; i < statements.size(); i++){
			SgAsmBlock *block = dynamic_cast<SgAsmBlock *>(statements[i]);			
			// AsmUnparser().unparse(std::cout, stmt);			
			SgAsmIntegerValuePtrList successors = block->get_successors();
			for(size_t j = 0; j < successors.size(); j++){
				cout << successors[j] << endl;
			}
		}	

		return funcNode;
	}
	

	void visitStatements(SgAsmStatementPtrList &statements, SgAsmFunction *func)
	{
		/* ... */
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
