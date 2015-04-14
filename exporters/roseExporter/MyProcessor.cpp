#include "MyProcessor.hpp"

MyProcessor :: ~MyProcessor() { }

void MyProcessor :: init(const SgAsmGenericFile* asmFile)
{
	writer.init();
	useDefAnalyzer.init(asmFile);
	useDefAnalyzer.setProcessor(this);
}

/**
   Called for each node of the "binary AST"
   We only check whether the node is a function
   node and then manually walk the tree from there.
   That's not too efficient but shouldn't cause trouble.
*/

void MyProcessor :: visit(SgNode *node)
{
	SgAsmFunction* func = isSgAsmFunction(node);
	if(!func) return;
	visitFunction(func);
}

void MyProcessor :: visitFunction(SgAsmFunction *func)
{
	BjoernFunctionNode *bjoernFunc = createBjoernFuncFromSgFunc(func);
	writer.writeFunction(bjoernFunc);
	// this will call the writer internally
	useDefAnalyzer.analyze(func, bjoernFunc);
	delete bjoernFunc;
}

BjoernFunctionNode * MyProcessor :: createBjoernFuncFromSgFunc(SgAsmFunction *func)
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

void MyProcessor :: visitStatements(SgAsmFunction *func, BjoernFunctionNode *bjoernFunc)
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

void MyProcessor :: visitBlock(SgAsmBlock *block, BjoernFunctionNode *bjoernFunc)
{
	// create and add basic block to function
	
	BjoernBasicBlockNode *basicBlock = createBjoernBasicBlockFromSgBlock(block);
	bjoernFunc->addBasicBlock(basicBlock);
	basicBlock->setFunc(bjoernFunc);
	
}

BjoernBasicBlockNode * MyProcessor :: createBjoernBasicBlockFromSgBlock(SgAsmBlock *block)
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

BjoernInstructionNode * MyProcessor :: createBjoernInstructionFromSgInstruction(SgAsmInstruction *instr)
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


void MyProcessor :: handleTrace(list<rose_addr_t> &path,
				map<uint64_t, BasicBlockSummary *> & summaries)
{
	cout << "Handle Trace" << endl; 
}
