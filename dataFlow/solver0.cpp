#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

#include <inttypes.h>
#include <iostream>
#include <vector>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <BinaryControlFlow.h>
#include <SymbolicSemantics2.h>
#include <PartialSymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <sawyer/GraphBoost.h>
#include "YicesSolver.h"

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

typedef Sawyer :: Container :: Graph<SgAsmBlock *> CFG;

template <class Name>
class GraphVizWriterDfg {
public:
	GraphVizWriterDfg(Name _name) : name(_name) {}
	template <class VertexOrEdge>
	void operator()(std::ostream& out, const VertexOrEdge& v) const {
		out << "[label=" << boost::escape_dot_string(name[v]) << "]";
	}
private:
	Name name;
};

template <typename CFG>
struct GraphVizWriterCfg {
	const CFG &cfg;
	GraphVizWriterCfg(CFG &cfg): cfg(cfg) {}
	typedef boost::graph_traits<rose::BinaryAnalysis::ControlFlow::Graph>::vertex_descriptor Vertex;
	void operator()(std::ostream &output, const Vertex &v) {
		SgAsmBlock *block = get_ast_node(cfg, v);
		output <<"[ label=\"" <<StringUtility::addrToString(block->get_address()) <<"\" ]";
	}
};


void writeDfg(const DataFlow::Graph& dfg, const std::string& pathOut)
{
	auto vertices = dfg.vertices();
	std::vector<std::string> names;

	for (auto vertex : vertices)
	{
		std::stringstream s;
		s << vertex.value();
		names.push_back(s.str());
	}

	boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> bdfg;
	for (auto edge : dfg.edges())
	{
		boost::add_edge(edge.source()->id(), edge.target()->id(), bdfg);
	}

	std::ofstream out;
	out.open(pathOut.c_str());
	GraphVizWriterDfg<std::vector<std::string>> lw(names);

	boost::write_graphviz(out, bdfg, lw);
}


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
		if (f->get_name() != "main") return;

		const RegisterDictionary *dictReg = RegisterDictionary::dictionary_pentium4();
		/*
		SymbolicSemantics::SValuePtr protoval = SymbolicSemantics::SValue::instance();
		SymbolicSemantics::MemoryStatePtr memory = SymbolicSemantics::MemoryState::instance(protoval, protoval);

        BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, dictReg);
        BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
		*/

		YicesSolver solver;
		solver.set_linkage(YicesSolver::LM_LIBRARY);
        auto opsRisc = SymbolicSemantics::RiscOperators::instance(dictReg, &solver);
        opsRisc->set_compute_usedef();
		auto dispX86 = DispatcherX86::instance(opsRisc);

		DataFlow df(dispX86);
		CFG cfg;
		ControlFlow().build_block_cfg_from_ast(node, cfg);
		auto dfgs = df.buildGraphPerVertex(cfg, 0);

		size_t i = 0;
		for (DataFlow::Graph& dfg : dfgs.values())
		{
			std::stringstream path;
			path << "real_smt_graph_" << i++ << ".dot";
			writeDfg(dfg, path.str());
		}

		std::cout << "Wrote " << i << " graphs\n";
	}
};

int main(int argc, char *argv[]) {
	SgProject* myProject = frontend(argc, argv);
	SgNode*  rootNode = dynamic_cast<SgNode*>(myProject);
	MyProcessor mp;
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
}
