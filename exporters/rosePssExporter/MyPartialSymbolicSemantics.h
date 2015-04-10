/*
 * MyPartialSymbolicSemantics.h
 *
 *  Created on: Apr 10, 2015
 *      Author: bjoern
 */

#ifndef MYPARTIALSYMBOLICSEMANTICS_H_
#define MYPARTIALSYMBOLICSEMANTICS_H_

#include <rose.h>
#include <PartialSymbolicSemantics2.h>

using namespace rose;
using namespace BinaryAnalysis;
using namespace InstructionSemantics2;

namespace bjoern {
namespace MyPartialSymbolicSemantics {

class RiscOperators;
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators : public PartialSymbolicSemantics::RiscOperators {
protected:
	explicit RiscOperators(const BaseSemantics::SValuePtr &protoval, SMTSolver *solver=NULL)
	: PartialSymbolicSemantics::RiscOperators(protoval, solver) {
	    }
	explicit RiscOperators(const BaseSemantics::StatePtr &state, SMTSolver *solver=NULL)
	: PartialSymbolicSemantics::RiscOperators(state, solver) {
	}
public:

	static RiscOperatorsPtr instance(const RegisterDictionary *regdict);

	virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
		const BaseSemantics::SValuePtr &addr,
		const BaseSemantics::SValuePtr &dflt,
		const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;
};

} /* namespace MyPartialSymbolicSemantics */
} /* namespace bjoern */

#endif /* MYPARTIALSYMBOLICSEMANTICS_H_ */
