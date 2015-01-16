#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

#include <inttypes.h>
#include <iostream>
#include <vector>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <BinaryControlFlow.h>
#include <BinaryTaintedFlow.h>
#include <SymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <sawyer/GraphBoost.h>

//
// Taint Analysis using ROSE
//


using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

typedef Sawyer :: Container :: Graph<SgAsmBlock *> CFG;

class MyProcessor : public SgSimpleProcessing
{
private:

public:
	MyProcessor()
	{

	}

	void visit(SgNode *node)
	{
		SgAsmFunction* f = isSgAsmFunction(node);
		if (f == NULL) return;

		std :: cout << f->get_name() << std::endl;
		std :: cout << "====================================" << std :: endl;

		// create dataflow engine
		SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(node);
		const RegisterDictionary *regdict = interp->get_registers();
		BaseSemantics ::RiscOperatorsPtr symbolicOps = SymbolicSemantics::RiscOperators::instance(regdict);
		DispatcherX86Ptr cpu = DispatcherX86::instance(symbolicOps);

		BaseSemantics :: SValuePtr esp_0 = symbolicOps->readRegister(cpu->REG_ESP);
		TaintedFlow taintAnalysis(cpu);

		TaintedFlow::Approximation approximation = TaintedFlow :: UNDER_APPROXIMATE;
		taintAnalysis.approximation(approximation);
		size_t cfgStartVertex = 0;


		// build control flow graph

		CFG cfg;
		ControlFlow().build_block_cfg_from_ast(node, cfg);

		  ///////////////////////
		 //  Taint Analysis   //
		///////////////////////

		taintAnalysis.computeFlowGraphs(cfg, cfgStartVertex);

		TaintedFlow::StatePtr initialState = taintAnalysis.stateInstance(TaintedFlow::BOTTOM);

		// at this point, we would need to taint different variables

		initialState->setIfExists(DataFlow::Variable(), TaintedFlow::NOT_TAINTED);


		// actually run the taint analysis

		taintAnalysis.runToFixedPoint(cfg, cfgStartVertex, initialState);

		BOOST_FOREACH(const typename CFG::VertexNode &vertex, cfg.vertices()){
			// rose_addr_t lastInsnAddr =
			//	SageInterface::querySubTree<SgAsmInstruction>(vertex.value()).back()->get_address();
			TaintedFlow::StatePtr state = taintAnalysis.getFinalState(vertex.id());
			std :: cout << *state << std :: endl;
		}

		// print 'variables' of function

		// BOOST_FOREACH(const DataFlow::Variable &variable, taintAnalysis.variables() )
		//	std :: cout << variable << std :: endl;



	}
};

int main(int argc, char *argv[]) {
	SgProject* myProject = frontend(argc, argv);
	SgNode*  rootNode = dynamic_cast<SgNode*>(myProject);
	MyProcessor mp;
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
}
