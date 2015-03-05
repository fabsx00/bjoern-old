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

namespace bjoern {

class PssProcessor : public SgSimpleProcessing
{
protected:
	static Sawyer::Message::Facility mlog;
	MemoryMap map;

public:
	PssProcessor();
	PssProcessor(SgAsmGenericFile* asmFile);
	virtual ~PssProcessor() {}
	static void initDiagnostics();

	void visit(SgNode *node);
};

} // namespace

#endif /* PSSEXPORTER_H_ */
