#ifndef _BJOERN_USE_DEF_ANALYZER_HPP
#define _BJOERN_USE_DEF_ANALYZER_HPP

#include <boost/algorithm/string/trim.hpp>
#include <string>
#include <inttypes.h>
#include <iostream>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <SymbolicSemantics2.h>
#include <PartialSymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>

#include <BaseSemantics2.h>

#include "bjoernNodes.hpp"
#include "CSVWriter.hpp"
#include "tracePolicy.hpp"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;
using namespace rose::BinaryAnalysis::InstructionSemantics2::PartialSymbolicSemantics;
using namespace boost::algorithm;
using namespace std;
using namespace Sawyer;
using namespace Container;
using namespace bjoern;

#define INVALID_ADDRESS (0)

class BjoernUseDefAnalyzer{

protected:
	BaseSemantics::DispatcherPtr disp;
	MemoryMap memoryMap;
	const RegisterDictionary* dictReg;
	RiscOperatorsPtr opsRisc;
	TracePolicyX86* tp;

	void initMemoryMap(const SgAsmGenericFile *asmFile)
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

	void initDispatcher()
	{
		dictReg = RegisterDictionary::dictionary_pentium4();
		opsRisc = PartialSymbolicSemantics::RiscOperators::instance(dictReg);
		opsRisc->set_memory_map(&memoryMap);
		disp = DispatcherX86::instance(opsRisc);
	}

	void initTracePolicy()
	{
		 tp = new TracePolicyX86(disp->get_operators());
	}

public:

	void init(const SgAsmGenericFile* asmFile)
	{
		initMemoryMap(asmFile);
		initDispatcher();
	}

	void analyze(SgAsmFunction *func)
	{
		Graph<SgAsmBlock*> cfg;
		ControlFlow().build_block_cfg_from_ast(func, cfg);

		auto startBb = *cfg.findVertex(0);

	}
};

#endif
