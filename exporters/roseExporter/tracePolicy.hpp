#ifndef _TRACE_POLICY_HPP
#define _TRACE_POLICY_HPP

#include <rose.h>
#include <BaseSemantics2.h>

using namespace rose::BinaryAnalysis::InstructionSemantics2;

namespace bjoern {
	struct TracePolicy {
		virtual void startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {}
		virtual void endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state) {}

		virtual void derivePostCallState(BaseSemantics::StatePtr preCallState) {}
		virtual bool isReturn(const SgAsmInstruction* instr) = 0;
		virtual bool isCall(const SgAsmInstruction* instr) = 0;
		virtual ~TracePolicy() {}
	};

	class TracePolicyX86 : public TracePolicy {
		/*!
		 * Specific to  Linux, cdecl, and x86.
     */
	protected:
		const BaseSemantics::RiscOperatorsPtr ops;
		const RegisterDescriptor* descEax;
		const RegisterDescriptor* descEcx;
		const RegisterDescriptor* descEdx;

	public:
		TracePolicyX86(const BaseSemantics::RiscOperatorsPtr ops);
		virtual void startOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state);
		virtual void endOfBb(const SgAsmBlock* bb, BaseSemantics::StatePtr state);
		virtual bool isCall(const SgAsmInstruction* instr);
		virtual bool isReturn(const SgAsmInstruction* instr);
		virtual void derivePostCallState(BaseSemantics::StatePtr preCallState);
	};
}

#endif
