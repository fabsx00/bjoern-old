#include <sstream>
#include <string>
#include <map>
#include <set>
#include <utility> // for pair
#include <cstdint>
using namespace std;

#include "bjoernNodes.hpp"
#include "CSVWriter.hpp"
#include "general.h"
#include "read_graph.hpp"

#include "dot_export.hpp"
#include "fill.hpp"


int main(int argc, char **argv)
 {
   if(argc != 3)
    {
     cout << "[/] Usage: " << argv[0] << " <bap-cfg> <bap-ssa-ddg>" << endl;
     return 0;
    }
 string cfg_file = argv[1];
 string ddg_file = argv[2];
  
 vector<pair<uint64_t, uint64_t>> cfg_addr;
  read_cfg_addr_graph(cfg_file, cfg_addr);
 map<uint64_t, vector<uint64_t>> cfg = compress_cfg(cfg_addr);
//  print_cfg(cfg);

 vector<BAP_SSA *> bap_ssas;
  read_ssa_nodes(ddg_file, bap_ssas);
 map<BAP_SSA *, vector<BAP_SSA *> > ddg;
  make_ssa_ddg(bap_ssas, ddg);
//  print_ddg(ddg);


 vector<BjoernInstructionNode> insts;
 map<BAP_SSA *, BjoernInstructionNode *> bap_ssa_to_instr;
  fill_instructions(bap_ssas, ddg, bap_ssa_to_instr, insts);

 map<uint64_t, BjoernBasicBlockNode> bbs;
  fill_bbs(bap_ssas, cfg, bap_ssa_to_instr, bbs);
 

 map<uint64_t, BjoernFunctionNode> funcs;
  fill_funcs(bbs, cfg, funcs);


 dot_export ex("combined_graph.dot", bbs, cfg, ddg);


 CSVWriter w;
  w.init();
   for(auto &it : funcs)
    {
     w.writeFunction(&(it.second));
    }
  w.deinit();

  return 0;
 }


