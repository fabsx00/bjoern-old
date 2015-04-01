#include "tracePolicy.hpp"

using namespace bjoern;

TracePolicyX86::TracePolicyX86(const BaseSemantics::RiscOperatorsPtr ops) : ops(ops)
{
	const RegisterDictionary* dictReg = RegisterDictionary::dictionary_pentium4();
	descEax = dictReg->lookup("eax");
	descEcx = dictReg->lookup("ecx");
	descEdx = dictReg->lookup("edx");
}

void TracePolicyX86::startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state)
{
	// anything?
}

void TracePolicyX86::endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state)
{
	// anything?
}

void TracePolicyX86::derivePostCallState(BaseSemantics::StatePtr preCallState)
{

	/* In cdecl eax, ecx, and edx are callee-saved.
	 * Therefore they must be considered as unknown after a call.
	 */
	auto valUndef = preCallState->get_protoval()->undefined_(descEax->get_nbits());
	preCallState->writeRegister(*descEax, valUndef, ops.get());

	valUndef = preCallState->get_protoval()->undefined_(descEcx->get_nbits());
	preCallState->writeRegister(*descEcx, valUndef, ops.get());

	valUndef = preCallState->get_protoval()->undefined_(descEdx->get_nbits());
	preCallState->writeRegister(*descEdx, valUndef, ops.get());
}



bool TracePolicyX86::isCall(const SgAsmInstruction* instr)
{
	return isSgAsmX86Instruction(instr)->get_kind() == x86_call;
}

bool TracePolicyX86::isReturn(const SgAsmInstruction* instr)
{
	return isSgAsmX86Instruction(instr)->get_kind() == x86_ret;
}
