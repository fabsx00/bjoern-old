#ifndef __DOT_EXPORT_H__
#define __DOT_EXPORT_H__

#include <iostream>
#include <fstream>
using namespace std;

#include "bjoernNodes.hpp"
#include "read_graph.hpp"

class dot_export
 {
 private:
  ofstream f;

  void draw_bb(const BjoernBasicBlockNode &b);
  void draw_bbs(map<uint64_t, BjoernBasicBlockNode> &bbs);
  void draw_cfg_edge(uint64_t from, uint64_t to);
  void draw_cfg_edges(const map<uint64_t, vector<uint64_t>> &cfg);
  void draw_ddg_edge(BAP_SSA *from, BAP_SSA *to);
  void draw_ddg_edges(const map<BAP_SSA *, vector<BAP_SSA *>> &ddg);

 public:
  dot_export();
  dot_export(  const string &file_name
             , map<uint64_t, BjoernBasicBlockNode> &bbs
             , map<uint64_t, vector<uint64_t>> &cfg
             , map<BAP_SSA *, vector<BAP_SSA *>> &ssa_ddg
            );

  void write(  const string &file_name
             , map<uint64_t, BjoernBasicBlockNode> &bbs
             , map<uint64_t, vector<uint64_t>> &cfg
             , map<BAP_SSA *, vector<BAP_SSA *>> &ssa_ddg
            );
 };

#endif

