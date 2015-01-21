#include "dot_export.hpp"


void dot_export::draw_bb(const BjoernBasicBlockNode &b)
 {
 // subgraph-header
  f << "\tsubgraph cluster_" << b.getAddr() << " {\n";
  f << "\tstyle=filled;\n";
  f << "\tcolor=lightgrey;\n";
  f << "\tlabel=\"" << b.getAddr() << "\"\n\n";

 // draw ssa_eqs
    for(auto &e : b.getInstructions())
     {
      f << "\tn_" << e->getBAP_SSA()->asm_addr << "_" << e->getBAP_SSA()->asm_counter << " [label=\"" << e->getBAP_SSA()->code << "\"];\n";
     }

 // draw invisible edges, connecting the ssa_edges
 bool first = true;
    for(auto &e : b.getInstructions())
     {
       if(!first)
        {
         f << " -> ";
        }
      f << "\tn_" << e->getBAP_SSA()->asm_addr << "_" << e->getBAP_SSA()->asm_counter;
      first = false;
     }
  f << " [style=invis];\n";


 // subgraph-footer
  f << "\t}\n\n";
 }


void dot_export::draw_bbs(map<uint64_t, BjoernBasicBlockNode> &bbs)
 {
   for(auto &it : bbs)
    {
     draw_bb(it.second);
    }
 }


void dot_export::draw_cfg_edge(uint64_t from, uint64_t to)
 {
  f << "n_" << from << "_0 -> n_" << to << "_0"
    << " [penwidth=3, ltail=cluster_0x" << hex << from << ", lhead=cluster_0x" << to << dec << "];\n";
 }


void dot_export::draw_cfg_edges(const map<uint64_t, vector<uint64_t>> &cfg)
 {
  f << "\n\n";
   for(auto &from_pair : cfg)
    {
      for(auto &to : from_pair.second)
       {
        draw_cfg_edge(from_pair.first, to);
       }
    }
 }


void dot_export::draw_ddg_edge(BAP_SSA *from, BAP_SSA *to)
 {
  f << "n_" << from->asm_addr << "_" << from->asm_counter
    << " -> n_" << to->asm_addr << "_" << to->asm_counter
    << " [color=red, constraint=false];\n";
 }


void dot_export::draw_ddg_edges(const map<BAP_SSA *, vector<BAP_SSA *>> &ddg)
 {
  f << "\n\n";
   for(auto &from_pair : ddg)
    {
      for(auto &to : from_pair.second)
       {
        draw_ddg_edge(from_pair.first, to);
       }
    }
 }


dot_export::dot_export()
 {
  ;
 }


dot_export::dot_export(  const string &file_name
                       , map<uint64_t, BjoernBasicBlockNode> &bbs
                       , map<uint64_t, vector<uint64_t>> &cfg
                       , map<BAP_SSA *, vector<BAP_SSA *>> &ssa_ddg
                      )
 {
  write(file_name, bbs, cfg, ssa_ddg);
 }


void dot_export::write(  const string &file_name
                       , map<uint64_t, BjoernBasicBlockNode> &bbs
                       , map<uint64_t, vector<uint64_t>> &cfg
                       , map<BAP_SSA *, vector<BAP_SSA *>> &ssa_ddg
                      )
 {
  f.open(file_name);
  f << "digraph G {\n";
  f << "\tnode [shape=rectangle];\n";
  f << "\tcompoung = true;\n";


  draw_bbs(bbs);
  draw_cfg_edges(cfg);
  draw_ddg_edges(ssa_ddg);

  f << "\n}\n";
  f.close();
 }

