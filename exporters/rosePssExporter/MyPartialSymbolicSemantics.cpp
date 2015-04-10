/*
 * MyPartialSymbolicSemantics.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: bjoern
 */

#include "MyPartialSymbolicSemantics.h"

namespace bjoern {
namespace MyPartialSymbolicSemantics {

RiscOperatorsPtr RiscOperators::instance(const RegisterDictionary *regdict) {
    BaseSemantics::SValuePtr protoval = PartialSymbolicSemantics::SValue::instance();

    BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
    BaseSemantics::MemoryCellListPtr memory = BaseSemantics::MemoryCellList::instance(protoval, protoval);
    memory->set_byte_restricted(false); // because extracting bytes from a word results in new variables for this domain
    BaseSemantics::StatePtr state = PartialSymbolicSemantics::State::instance(registers, memory);
    SMTSolver *solver = NULL;
    RiscOperatorsPtr ops = RiscOperatorsPtr(new RiscOperators(state, solver));
    return ops;
}

BaseSemantics::SValuePtr RiscOperators::readMemory(const RegisterDescriptor &segreg,
		const BaseSemantics::SValuePtr &addr,
		const BaseSemantics::SValuePtr &dflt,
		const BaseSemantics::SValuePtr &cond) {

	// XXX: we just skip large memory queries for now and return '0'. We probably should do something smarter.
	if (dflt->get_width() > 32) {
		return PartialSymbolicSemantics::SValue::instance(dflt->get_width());
	}
	return PartialSymbolicSemantics::RiscOperators::readMemory(segreg, addr, dflt, cond);
}

} /* namespace MyPartialSymbolicSmantics */
} /* namespace bjoern */
