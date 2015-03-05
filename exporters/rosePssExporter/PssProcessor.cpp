/*
 * PssExporter.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: felix
 */

#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryControlFlow.h>
#include <PartialSymbolicSemantics2.h>

#include "PssProcessor.h"

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;
using namespace Diagnostics;

#define _DEBUG

namespace bjoern {

Sawyer::Message::Facility PssProcessor::mlog;

void PssProcessor::initDiagnostics() {
	static bool initialized = false;
	if (initialized) return;
	mlog = Sawyer::Message::Facility("bjoern::PssProcessor", Diagnostics::destination);
	Diagnostics::mfacilities.insertAndAdjust(mlog);
	initialized = true;
}

PssProcessor::PssProcessor() {
	initDiagnostics();
}

PssProcessor::PssProcessor(SgAsmGenericFile* asmFile) : PssProcessor() {

	// initialize the memory map
	auto sections = asmFile->get_mapped_sections();
	for (auto section : sections)
	{
		rose_addr_t size = section->get_size();
		rose_addr_t mappedSize = section->get_mapped_size();
		rose_addr_t va = section->get_mapped_preferred_va();

		if (size == 0) continue;


		mlog[TRACE] << "Adding section: "<< section->get_name()->get_string() << " va: " << std::hex << va << ", size: " << size << ", mapped size: " << mappedSize << std::endl;
		uint8_t* data = new uint8_t[mappedSize];
		memset(data, 0, mappedSize);
		if (section->read_content(0, data, size) != size)
		{
			mlog[ERROR] << "Failed to access section \"" << section->get_name()->get_string() << "\".";
			continue;
		}

		// no need to free data buffer, because buff is ref counted (right?)
		auto buff = MemoryMap::StaticBuffer::instance(data, mappedSize);
		auto interval = AddressInterval::baseSize(va, mappedSize);
		map.insert(interval,  MemoryMap::Segment(buff, 0, MemoryMap::READ_WRITE_EXECUTE));
	}
}

void PssProcessor::visit(SgNode *node) {
	SgAsmFunction* f = isSgAsmFunction(node);
	if (f == NULL) return;

#ifdef _DEBUG
	// only consider main()
	if (f->get_name() != "main") return;
#endif

	const RegisterDictionary *dictReg = RegisterDictionary::dictionary_pentium4();
	auto opsRisc = PartialSymbolicSemantics::RiscOperators::instance(dictReg);
	opsRisc->set_memory_map(&map);

	auto dispX86 = DispatcherX86::instance(opsRisc);
	auto entryBb = f->get_entry_block();
	auto statements = entryBb->get_statementList();

	for (auto statement : statements)
	{
		auto instr = isSgAsmInstruction(statement);
		dispX86->processInstruction(instr);

		mlog[DEBUG] << "-----------------\n" << *(dispX86->get_state());
	}
}

} // namespace
