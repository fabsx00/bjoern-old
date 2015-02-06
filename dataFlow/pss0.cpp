#define __STDC_FORMAT_MACROS // this needs to be there for some reason.

#include <inttypes.h>
#include <iostream>
#include <vector>
#include <rose.h>
#include <DispatcherX86.h>
#include <BinaryDataFlow.h>
#include <BinaryControlFlow.h>
#include <PartialSymbolicSemantics2.h>
#include <AstSimpleProcessing.h>
#include <Cxx_Grammar.h>
#include <BinaryCallingConvention.h>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <sawyer/GraphBoost.h>
#include <memory>
#include <utility>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

using namespace Sawyer::Container;

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
	MemoryMap* map;

	MyProcessor(MemoryMap* map) : map(map) {}

	void visit(SgNode *node)
	{
		SgAsmFunction* f = isSgAsmFunction(node);
		if (f == NULL) return;
		if (f->get_name() != "main") return;

		const RegisterDictionary *dictReg = RegisterDictionary::dictionary_pentium4();
        auto opsRisc = PartialSymbolicSemantics::RiscOperators::instance(dictReg);
        opsRisc->set_memory_map(map);

		auto dispX86 = DispatcherX86::instance(opsRisc);

		DataFlow df(dispX86);
		CFG cfg;
		ControlFlow().build_block_cfg_from_ast(node, cfg);
		auto dfgs = df.buildGraphPerVertex(cfg, 0);

		size_t i = 0;
		for (DataFlow::Graph& dfg : dfgs.values())
		{
			std::stringstream path;
			path << "graphs/pss_mem_graph_" << i++ << ".dot";
			writeDfg(dfg, path.str());
		}

		std::cout << "Wrote " << i << " graphs\n";
	}
};

int main(int argc, char *argv[]) {

	std::cout << argv[1] << std::endl;

	SgProject* proj = frontend(argc, argv);
	auto fileList = proj->get_fileList();
	if (fileList.size() != 1)
	{
		std::cerr << "Error: no or multiple input files given.\n";
		return 0;
	}

	auto file = fileList[0];
	auto binFile = isSgBinaryComposite(file);
	if (binFile == nullptr)
	{
		std::cerr << "Error: input file is not in a known executable file format.";
		return 0;
	}
	SgAsmGenericFile* asmFile = binFile->get_binaryFile();
	auto sections = asmFile->get_mapped_sections();

	MemoryMap* map = new MemoryMap();
	for (auto section : sections)
	{
		rose_addr_t size = section->get_size();
		rose_addr_t mappedSize = section->get_mapped_size();
		rose_addr_t va = section->get_mapped_preferred_va();

		if (size == 0) continue;


		std::cout << "Adding section: "<< section->get_name()->get_string() << " va: " << std::hex << va << ", size: " << size << ", mapped size: " << mappedSize << std::endl;
		uint8_t* buff = new uint8_t[mappedSize];
		memset(buff, 0, mappedSize);
		if (section->read_content(0, buff, size) != size)
		{
			std::cout << "\tFailed...\n";
			continue;
		}

		auto sBuff = MemoryMap::StaticBuffer::instance(buff, mappedSize);
		auto interval = AddressInterval::baseSize(va, mappedSize);
		map->insert(interval,  MemoryMap::Segment(sBuff, 0, MemoryMap::READ_WRITE_EXECUTE));
	}

	SgNode* rootNode = dynamic_cast<SgNode*>(proj);
	MyProcessor mp(map);
	t_traverseOrder order = postorder;
	mp.traverse(rootNode, order);
	return 0;
}
