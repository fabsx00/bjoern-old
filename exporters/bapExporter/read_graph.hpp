#ifndef __READ_GRAPH_H__
#define __READ_GRAPH_H__

#include <map>
#include <set>
#include <algorithm>
using namespace std;

#include "general.h"

#include "BAP_SSA.hpp"

// Function to read the BAP-Output-Graphs

void read_cfg_addr_graph(const string &file_name, vector<pair<uint64_t, uint64_t>> &edges);
map<uint64_t, vector<uint64_t>> compress_cfg(vector<pair<uint64_t, uint64_t>> &cfg);
void print_cfg(map<uint64_t, vector<uint64_t>> &cfg);

void read_ssa_nodes(const string &file_name, vector<BAP_SSA *> &bap_ssas);
void make_ssa_ddg(vector<BAP_SSA *> &bap_ssas, map<BAP_SSA *, vector<BAP_SSA *> > &ddg);
void print_ddg(map<BAP_SSA *, vector<BAP_SSA *>> &ddg);

#endif

