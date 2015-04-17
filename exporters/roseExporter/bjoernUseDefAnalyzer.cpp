#include "MyProcessor.hpp"
#include "bjoernUseDefAnalyzer.hpp"

using namespace BinaryAnalysis :: InstructionSemantics2 :: BaseSemantics;

void BjoernUseDefAnalyzer :: initMemoryMap(const SgAsmGenericFile *asmFile)
{
	auto sections = asmFile->get_mapped_sections();
	for (auto section : sections)
	{
		rose_addr_t size = section->get_size();
		rose_addr_t va = section->get_mapped_preferred_va();
		if (size == 0 || va == INVALID_ADDRESS)
			continue;

		uint8_t* data = new uint8_t[size];
		memset(data, 0, size);

		if (section->read_content(0, data, size) != size)
		{
			cerr << "Error reading section" << endl;
			continue;
		}

		// no need to free data buffer, because buff is ref counted (right?)
		auto buff = MemoryMap::StaticBuffer::instance(data, size);
		auto interval = AddressInterval::baseSize(va, size);
		memoryMap.insert(interval,  MemoryMap::Segment(buff, 0, MemoryMap::READ_WRITE_EXECUTE));
	}
}


void BjoernUseDefAnalyzer ::initDispatcher()
{
	dictReg = RegisterDictionary::dictionary_pentium4();
	opsRisc = MyPartialSymbolicSemantics::RiscOperators::instance(dictReg);
	opsRisc->set_memory_map(&memoryMap);
	disp = DispatcherX86::instance(opsRisc);
}

void BjoernUseDefAnalyzer :: initTracePolicy()
{
	tp = new TracePolicyX86(disp->get_operators());
}


/*
  Traverse all possible paths in a CFG. A depth first search
  is performed where no edge is ever expanded twice. When a
  node is reached where no more edges can be expanded, a
  callback function is invoked, allowing the path to be
  processed.
*/

void BjoernUseDefAnalyzer :: traceCFG(const Graph<SgAsmBlock*>::VertexNode* entryNode)
{
	visited.clear();
	path.clear();
	clearSummaries();

	// Debug message to verify that initial
	// state is indeed 'empty'.
	// cout << *(disp->get_state()) << endl;

	traceCFG_r(entryNode, disp);
}

void BjoernUseDefAnalyzer ::clearSummaries()
{
	for(auto it = summaries.begin(); it != summaries.end(); it++){
		delete it->second;
	}
	summaries.clear();
}


bool BjoernUseDefAnalyzer :: isTerminatingEdge(Graph<SgAsmBlock*>::EdgeNode edge,
					       uint64_t edgeId)
{
	// has edge already been expanded?

	if( visited.find(edgeId) != visited.end())
		return true;
	return false;
}


void BjoernUseDefAnalyzer :: traceCFG_r(const Graph<SgAsmBlock*>::VertexNode* vertex,
					BaseSemantics::DispatcherPtr disp)
{

	SgAsmBlock* bb = vertex->value();
	curBBAddress = getAddressForNode(*vertex);
	processBasicBlock(bb);

	// Save state after basic block execution
	auto stateAfterBB = disp->get_state();

	unsigned int nEdgesExpanded = 0;
	for (auto& edge : vertex->outEdges()) {

		uint64_t edgeId = edgeToId(edge);

		if(isTerminatingEdge(edge, edgeId)){
			continue;
		}

		visited[edgeId] = true;
		nEdgesExpanded ++;
		auto targetVertex = *edge.target();
		path.push_back(getAddressForNode(*vertex));
		// clone state so that modification is allowed
		disp->get_operators()->set_state(stateAfterBB->clone());

		traceCFG_r(&targetVertex, disp);

		disp->get_operators()->set_state(stateAfterBB);
		path.pop_back();
		// visited.erase(edgeId);
	}

	if(nEdgesExpanded == 0){
		// reached a node where no more edges
		// were expandable.
		path.push_back(getAddressForNode(*vertex));
		registerTrace();
		path.pop_back();
	}

	curBBAddress =  getAddressForNode(*vertex);
	removeEntryInBasicBlockSummary(bb);

}

void BjoernUseDefAnalyzer :: registerTrace()
{
	for(auto addr : path){
		registerStateOfBasicBlock(addr);
	}

}

void BjoernUseDefAnalyzer :: registerStateOfBasicBlock(rose_addr_t addr)
{
	BasicBlockSummary *summary = summaries[addr];
	addSymbolsToFunctionNode(summary);
	connectBasicBlockToSymbols(summary, addr);
}

void BjoernUseDefAnalyzer :: addSymbolsToFunctionNode(BasicBlockSummary *summary)
{
	list<string> symbols;

	summary->getUsedForRegisters(symbols);
	summary->getDefinedRegisters(symbols);
	summary->getUsedMemory(symbols);
	summary->getDefinedMemory(symbols);

	symbols.sort();
	symbols.unique();

	for(auto sym : symbols)
		curFuncNode -> addSymbol(sym);
}

void BjoernUseDefAnalyzer :: connectBasicBlockToSymbols(BasicBlockSummary *summary,
						       rose_addr_t addr)
{
	stringstream sstr;
	sstr << addr;
	BjoernBasicBlockNode *bb = curFuncNode->getBasicBlockByAddr(sstr.str());

	list<string> symbols;
	summary->getUsedForRegisters(symbols);
	summary->getUsedMemory(symbols);

	for(auto sym : symbols){
		auto symbolNode = curFuncNode -> getSymbolNodeByName(sym);
		bb->addUsedSymbol(symbolNode);
	}

	symbols.clear();

	summary->getDefinedRegisters(symbols);
	summary->getDefinedMemory(symbols);

	for(auto sym : symbols){
		auto symbolNode = curFuncNode -> getSymbolNodeByName(sym);
		bb->addDefinedSymbol(symbolNode);
	}

	bb->uniquifySymbols();
}


/**
   Called for each basic block during trace construction.
   processor-state can be accessed via `disp`
   the trace policy is in `tp`

*/

void BjoernUseDefAnalyzer :: processBasicBlock(SgAsmBlock *basicBlock)
{
	auto previousState = disp->get_state()->clone();
	auto attributes = processStatements(basicBlock);
	updateBasicBlockSummary(attributes, previousState);
}


void BjoernUseDefAnalyzer :: updateBasicBlockSummary(BasicBlockSummary::ATTRIBUTES attributes,
						     BaseSemantics :: StatePtr &previousState)
{

	// If a summary does not exist for this block yet,
	// create it.
	if(summaries.find(curBBAddress) == summaries.end()){
		summaries[curBBAddress] = new BasicBlockSummary;
	}

	auto preCallState = disp->get_state()->clone();
	auto finalState = disp->get_state()->clone();

	if(attributes & BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL){
		tp->derivePostCallState(disp->get_state());
		finalState = disp->get_state()->clone();
	}

	// cout << *finalState << endl;

	removeUnmodifiedEntries(preCallState, previousState);
	removeUnmodifiedEntries(finalState, previousState);

	// cout << "After removal: " << endl;
	// cout << curBBAddress << endl;
	// cout << *finalState << endl;
	// cout << "================" << endl;

	summaries[curBBAddress]->pushState(finalState, preCallState);
}

void BjoernUseDefAnalyzer :: removeUnmodifiedEntries(BaseSemantics :: StatePtr & thisState,
						     BaseSemantics :: StatePtr & previousState)
{
	auto regState = dynamic_pointer_cast<RegisterStateGeneric>(thisState->get_register_state());
	auto memState = dynamic_pointer_cast<MemoryCellList>(thisState->get_memory_state());

	auto prevRegState = dynamic_pointer_cast<RegisterStateGeneric>(previousState->get_register_state());
	auto prevMemState = dynamic_pointer_cast<MemoryCellList>(previousState->get_memory_state());

	// This isn't too efficient. Maybe we can
	// just create a new clean object without cloning

	auto newState = thisState->clone();
	newState->clear();
	newState->clear_memory(); // not sure this is required

	assert(dynamic_pointer_cast<MemoryCellList>(newState->get_memory_state())->get_cells().size() == 0);
	assert(dynamic_pointer_cast<RegisterStateGeneric>(newState->get_register_state())->get_stored_registers().size() == 0);

	// Add registers

	auto storedRegs = regState->get_stored_registers();
	for(auto reg : storedRegs){

		auto prevRegDict = prevRegState->get_register_dictionary();
		auto regName = prevRegDict->lookup(reg.desc);
		if(regName == "") continue;

		auto oldVal = prevRegState->readRegister(reg.desc, disp->get_operators().get() );
		auto curVal = reg.value;

		// add to newState if register differs
		if(!oldVal->must_equal(curVal)){
			newState->writeRegister(reg.desc, reg.value, disp->get_operators().get());
		}
	}

	// Add memory

	auto prevCellList = prevMemState->get_cells();
	auto curCellList = memState->get_cells();

	// According to the docs, the most recently added cells
	// are at the beginning of the list. We can thus determine
	// cells that have been newly created easily, these are the
	// first n cells, where n is the difference in size between
	// the new list and the original list.


	int n = curCellList.size() - prevCellList.size();
	if(n < 0){

		// State has changed completely, just take
		// it as it is.

		for(auto cell : curCellList){
			newState->writeMemory(cell->get_address(), cell->get_value(), disp->get_operators().get(),
					      disp->get_operators().get());
		}
		thisState = newState;
		return;

	}

	auto it = curCellList.begin();

	for(; it != curCellList.end(); it++){
		auto cell = *it;

		if(n == 0) break;

		// add newly created cells
		newState->writeMemory(cell->get_address(), cell->get_value(), disp->get_operators().get(),
				      disp->get_operators().get());

		n--;
	}

	// add cells that changed

	for(auto prevCell : prevCellList){
		auto curCell = *it;

		assert(prevCell->get_address()->must_equal(curCell->get_address()));

		if(!curCell->get_value()->must_equal(prevCell->get_value()))
			newState->writeMemory(curCell->get_address(), curCell->get_value(),
					      disp->get_operators().get(),
					      disp->get_operators().get());

		assert(it != curCellList.end());
		it++;
	}

	thisState = newState;
}

void BjoernUseDefAnalyzer :: removeEntryInBasicBlockSummary(SgAsmBlock *basicBlock)
{

	if(summaries.find(curBBAddress) == summaries.end()){
		cout << "This should not happen" << endl;
	}

	summaries[curBBAddress]->popState();
}



BasicBlockSummary::ATTRIBUTES BjoernUseDefAnalyzer :: processStatements(SgAsmBlock *basicBlock)
{
	auto statements = basicBlock->get_statementList();
	for (auto statement : statements){
		auto instr = isSgAsmInstruction(statement);

		if (tp->isCall(instr)) {
			// we don't execute calls.
			return BasicBlockSummary::ATTRIBUTES::ENDS_IN_CALL;
		}
		if (tp->isReturn(instr)) {
			// we don't execute rets either.
			return BasicBlockSummary::ATTRIBUTES::ENDS_IN_RET;
		}

		processInstruction(instr);
	}
	return BasicBlockSummary::ATTRIBUTES::NONE;
}

void BjoernUseDefAnalyzer :: processInstruction(SgAsmInstruction *instr)
{

	try {
		disp->processInstruction(instr);
	}catch (BaseSemantics::Exception& e){
		// Not all instructions are supported.
		// Just catch and return
		return;
	}
}

/**
   Create an id from and edges source and target.
   We assume that only a single edge can exist between
   a source and a target, i.e., the CFG is not a multigraph.
*/

uint64_t BjoernUseDefAnalyzer :: edgeToId(Graph<SgAsmBlock*>::EdgeNode edge)
{
	// we assume here that rose_addr_t is 32 bit
	auto src_addr = getAddressForNode(*edge.source());
	auto dst_addr = getAddressForNode(*edge.target());
	return ((uint64_t) src_addr << 32) | dst_addr;
}


rose_addr_t BjoernUseDefAnalyzer :: getAddressForNode(Graph<SgAsmBlock*>::VertexNode node)
{
	return node.value()->get_address();
}



void BjoernUseDefAnalyzer :: init(const SgAsmGenericFile* asmFile)
{
	initMemoryMap(asmFile);
	initDispatcher();
	initTracePolicy();
}

/**
   Analyze 'func' and write resulting nodes to
   'bjoernFuncNode'.
*/

void BjoernUseDefAnalyzer :: analyze(SgAsmFunction *func,
				     BjoernFunctionNode *bjoernFuncNode)
{

	initDispatcher();
	initTracePolicy();

	Graph<SgAsmBlock*> cfg;
	ControlFlow().build_block_cfg_from_ast(func, cfg);
	curFuncNode = bjoernFuncNode;
	auto entryNode = *cfg.findVertex(0);
	traceCFG(&entryNode);
}
