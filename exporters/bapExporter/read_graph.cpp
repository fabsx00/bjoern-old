#include "read_graph.hpp"


void parse_ssa_node(const string &line, vector<string> &lines)
 {
//  BB_385 [label="BB_385\naddr 0x400de5 @asm \"retq   \"\nlabel pc_0x400de5\nR_OF_1719:bool = phi(R_OF:bool, R_OF_1726:bool)\nR_CF_1718:bool = phi(R_CF:bool, R_CF_1725:bool)\nR_PF_1717:bool = phi(R_PF:bool, R_PF_1730:bool)\nR_AF_1716:bool = phi(R_AF:bool, R_AF_1727:bool)\nT_t1_282_1715:u64 = phi(T_t1_282:u64, T_t1_282_1722:u64)\nR_ZF_1714:bool = phi(R_ZF:bool, R_ZF_1732:bool)\nR_SF_1713:bool = phi(R_SF:bool, R_SF_1731:bool)\nR_RSP_1712:u64 = phi(R_RSP:u64, R_RSP_1724:u64)\nT_t2_283_1711:u64 = phi(T_t2_283:u64, T_t2_283_1723:u64)\nT_ra_285_1720:u64 = mem64:u64?u8[R_RSP_1712:u64, e_little]:u64\nR_RSP_1721:u64 = R_RSP_1712:u64 + 8:u64\njmp T_ra_285_1720:u64 @str \"ret\"\n"];

 // Make sure that a simple split-by-word is sufficient
 string tmp = line;
  tmp = replace(tmp, "(", "( ");
  tmp = replace(tmp, ")", " )");
  tmp = replace(tmp, "[", "[ ");
  tmp = replace(tmp, "]", " ]");
  tmp = replace(tmp, ",", " ,");
  tmp = replace(tmp, "~", "~ ");

 //\\n is not used as separator for halt
   if(tmp.find("halt") != string::npos)
    {
     tmp = replace(tmp, " halt", "\\n\\nhalt");
     tmp = replace(tmp, " \" ];", "\\n \" ];");
    }

  lines = split(tmp, "\\n");
  lines.pop_back(); // ignore last line: "];

 // Extract SSA-expressions
   for(uint32_t i=1, i_end=lines.size(); i<i_end; ++i)
    {
      if(starts_with(lines[i], "addr "))
       {
        ++i;      // skip next line; asm-pc-label
        continue; // skip this line; asm-code
       }
      lines.erase(lines.begin(), lines.begin()+i);
      break;
    }

 // Deal with continued lines
 vector<uint32_t> to_del;
   for(int32_t i=lines.size()-1; i>=0; --i)
    {
      if(starts_with(lines[i], " "))
       {
        lines[i-1] += lines[i];
        to_del.push_back(i);
       }
    }
   for(auto &it : to_del)
    {
     lines.erase(lines.begin()+it, lines.begin()+it+1);
    }

  // Remove double spaces
   for(uint32_t i=0, i_end=lines.size(); i<i_end; ++i)
    {
    string before = lines[i];
     lines[i] = replace(lines[i], "  ", " ");
      if(before != lines[i]) --i;
    }
 }


/*
void make_ssa_cfg(  const map<uint64_t, vector<uint64_t> > &cfg
                  , const map<uint64_t, vector<BAP_SSA *> > addr_to_BAP_SSA
                  , map<BAP_SSA *, vector<BAP_SSA *> > &ssa_cfg
                 )
 {
 vector<BAP_SSA *> flows_to;
   for(auto &it_from: addr_to_BAP_SSA)
    {
    // Each flows to the next
      for(uint32_t i=0, i_end=it_from.second.size()-1; i<i_end; ++i)
       {
        flows_to.push_back(it_from.second[i+1]);
        ssa_cfg[it_from.second[i]] = flows_to;
        flows_to.clear();
       }

    // Except the last one: that flows to whatever the CFG says
      for(const auto &to_block : cfg[it_from.first])
       {
       // or, more precisely, the first SSA_Expression of that block
         for(const auto &it : addr_to_BAP_SSA[to_block])
          {
           flows_to.push_back(it);
           break;
          }
       }
     ssa_cfg[it_from.second[i]] = flows_to;
     flows_to.clear();
    }
 }
*/


void make_ssa_ddg(  vector<BAP_SSA *> &bap_ssas
                  , map<BAP_SSA *, vector<BAP_SSA *> > &ddg
                 )
 {
 map<uint64_t, BAP_SSA *> defined_in;
 vector<BAP_SSA *> dummy;
   for(auto &it : bap_ssas)
    {
      if(it->has_left())
       {
        defined_in.emplace(it->left_hand_symbol_id, it);
        ddg.emplace(it, dummy);
       }
    }

   for(auto &it : bap_ssas)
    {
      for(auto &def : it->right_hand_symbol_ids)
       {
         if(defined_in.find(def) == defined_in.end()) // e.g. operators and such
          {
           continue;
          }
        ddg[defined_in[def]].push_back(it);
       }
    }
 }


/*
void make_ssa_ddg(  const map<uint64_t, vector<BAP_SSA *> > addr_to_BAP_SSA
//                  , const map<uint64_t, vector<uint64_t> > &cfg
                  , map<BAP_SSA *, vector<BAP_SSA *> > &ddg
                 )
 {
 vector<BAP_SSA *> flows_to;
   for(auto &it_from: addr_to_BAP_SSA)
    {
      for(auto &from : it_from.second)
       {
         if(!from->has_left())
          {
           continue;
          }
   
         for(auto &it_to: addr_to_BAP_SSA)
          {
            for(auto &to : it_to.second)
             {
               if(to->has_on_right_side(from->left_hand_symbol_id))
                {
                 flows_to.push_back(to);
                }
             }
          }

//      for(auto &to_addr : cfg[from.first])
//       {
//         for(auto &to : addr_to_BAP_SSA[to_addr])
//          {
//            if(to.has_on_right_side(from.second.left_side))
//             {
//              flows_to.push_back(to);
//             }
//          }
//       }

        ddg[from] = flows_to;
        flows_to.clear();
       }
    }
 }
*/




void parse_node_line(const string &line, string &bb_name, string &addr)
 {
 //  BB_Error [label="BB_Error\nlabel BB_Error @set \"specialblock\" assert false \n"];
 //  BB_0 [label="BB_0\naddr 0x4007b0 @asm \"sub    $0x8,%rsp\"" ... ];
 string l = trim(line);
 vector<string> words = split(l, " ");
  bb_name = words[0];
  addr = "";
    if(l.find("\\naddr 0x") != string::npos)
     {
       for(auto &w : words)
        {
          if(w.size() > 2 && w[0] == '0' && w[1] == 'x')
           {
            addr = w;
             if(ends_with(addr, "\\n"))
              {
               addr = addr.substr(0, addr.size()-2);
              }
            break;
           }
        }
     }
 }


void parse_edge_line(const string &line, string &in, string &out)
 {
 //  BB_384 -> BB_385 [label=""];
 string l = trim(line);
    vector<string> words = split(l, " ");
  in = words[0];
  out = words[2];
 }


void read_cfg_addr_graph(const string &file_name, vector<pair<uint64_t, uint64_t>> &edges)
 {
 map<string, uint64_t> name_to_addr;

 string data = from_file(file_name);
 vector<string> blocks = split(data, "\n  \n  \n");

 // Block 1: contains Nodes
 vector<string> lines = split(blocks[0], "\n");
   for(uint32_t i=2, i_end = lines.size(); i<i_end; ++i) // skip first two lines
    {
    string bb_name, addr;
     parse_node_line(lines[i], bb_name, addr);

      if(addr == "")
       {
        continue;
       }
    int64_t addr_int = hexstring_to_int(addr);
      if(addr_int == -1)
       {
        cout << "error: " << addr << endl;
        exit(1);
       }
     name_to_addr.emplace(bb_name, addr_int);
    }

 // Block 2: contains Edges
  lines = split(blocks[1], "\n");
   for(uint32_t i=1, i_end = lines.size()-3; i<i_end; ++i) // skip first line, skip last three lines
    {
    string in, out;
     parse_edge_line(lines[i], in, out);
      if(   name_to_addr.find(in) != name_to_addr.end()
         && name_to_addr.find(out) != name_to_addr.end()
        )
       {
        edges.emplace_back(name_to_addr[in], name_to_addr[out]);
       }
    }
 }



void read_ssa_nodes(const string &file_name, vector<BAP_SSA *> &bap_ssas)
 {
 string data = from_file(file_name);
 vector<string> blocks = split(data, "\n  \n  \n");

 map<string, int64_t> symbol_to_id;
 vector<string> bap_lines;

 // Block 1: contains Nodes
 vector<string> lines = split(blocks[0], "\n");
   for(uint32_t i=2, i_end = lines.size(); i<i_end; ++i) // skip first two lines
    {
//cout << "read ssa node " << i << ": " << lines[i] << endl;
    string bb_name, addr;
     parse_node_line(lines[i], bb_name, addr);

      if(addr == "")
       {
        continue;
       }

    int64_t addr_int = hexstring_to_int(addr);
      if(addr_int == -1)
       {
        cout << "error: " << addr << endl;
        exit(1);
       }

     parse_ssa_node(lines[i], bap_lines);
      for(uint32_t j=0, j_end = bap_lines.size(); j<j_end; ++j)
       { 
       BAP_SSA *bap_ssa = new BAP_SSA(addr_int, j, bap_lines[j], symbol_to_id);
        bap_ssas.push_back(bap_ssa);
       }
    }

  //Don't care about edge-block
 }


map<uint64_t, vector<uint64_t>> compress_cfg(vector<pair<uint64_t, uint64_t>> &cfg)
 {
 map<uint64_t, vector<uint64_t>> ret;
   for(auto &from_to : cfg)
    {
     ret[from_to.first].push_back(from_to.second);
    }

 // Make unique
   for(auto &it : ret)
    {
     sort(it.second.begin(), it.second.end());
    auto u = unique(it.second.begin(), it.second.end());
     it.second.resize(std::distance(it.second.begin(), u));
    }

  return ret;
 }




void print_ddg(map<BAP_SSA *, vector<BAP_SSA *>> &ddg)
 {
 uint64_t counter = 0;
   for(auto &from_to_pair : ddg)
    {
      for(auto &to : from_to_pair.second)
       {
        cout << from_to_pair.first->to_string();
        cout << "-->\n";
        cout << to->to_string() << "\n\n\n";
        ++counter;
       }
    }
  cout << "ddg has " << counter << " edges" << endl;
 }


void print_cfg(map<uint64_t, vector<uint64_t>> &cfg)
 {
   for(auto &from_to : cfg)
    {
     cout << "0x" << hex << from_to.first << " ->";
      for(auto &to : from_to.second)
       {
        cout << " 0x" << to;
       }
     cout << dec << endl;
    }
 }

