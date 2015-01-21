// Functions to convert the read BAP-output to bjoernNodes


void fill_instructions(  const vector<BAP_SSA *> &bap_ssas
                       , const map<BAP_SSA *, vector<BAP_SSA *>> &ddg
                       , map<BAP_SSA *, BjoernInstructionNode *> &bap_ssa_to_instr
                       , vector<BjoernInstructionNode> &insts
                      )
 {
 // Fill Instructions
   for(auto &it : bap_ssas)
    {
    BjoernInstructionNode dummy;
     dummy.setAddr(it->asm_addr);
     dummy.setBAP_SSA(it);

     insts.push_back(dummy);
    }

 uint64_t inst_pos = 0;
   for(auto &it : bap_ssas)
    {
     bap_ssa_to_instr.emplace(it, &(insts[inst_pos]));
     ++inst_pos;
    }
  cout << "[+] Filled Instructions" << endl;


 // Add DDG-Edges
   for(auto &from_to : ddg)
    {
      for(auto &to : from_to.second)
       {
        bap_ssa_to_instr[from_to.first]->addDataSuccessor(bap_ssa_to_instr[to]);
       }
    }
  cout << "[+] Added DDG-Edges" << endl;
 }


void fill_bbs(  const vector<BAP_SSA *> &bap_ssas
              , const map<uint64_t, vector<uint64_t>> &cfg
              , map<BAP_SSA *, BjoernInstructionNode *> &bap_ssa_to_instr
              , map<uint64_t, BjoernBasicBlockNode> &bbs
             )
 { 
 // Fill BBs with instructions

 // First step: get addr-to-vector-of-bap_ssa mapping
 map<uint64_t, vector<BAP_SSA *>> bap_instr_lists;
   for(auto &it : bap_ssas)
    {
    vector<BAP_SSA *> dummy;
      if(bap_instr_lists.find(it->asm_addr) == bap_instr_lists.end())
       {
        bap_instr_lists.emplace(it->asm_addr, dummy);
       }
    auto pos = bap_instr_lists.find(it->asm_addr);
      if(pos->second.size() <= it->asm_counter)
       {
        pos->second.resize(it->asm_counter+1);
       }
     pos->second[it->asm_counter] = it;
    }
  cout << "[+] Got Instruction-to-BB-mapping" << endl;
 

 // Second step: Use that mapping to add Instructions to BBs
//uint64_t ctr = 0;
   for(auto &it : bap_instr_lists)
    {
//cout << ++ctr << "/" << bap_instr_lists.size() << endl;
//cout << "0x" << hex << it.first << dec << endl;
    BjoernBasicBlockNode dummy;
     dummy.setAddr(it.first);
      for(auto &suc : it.second)
       {
         if(bap_ssa_to_instr.find(suc) == bap_ssa_to_instr.end())
          {
           cout << "not found: " << suc << endl;
          }
        dummy.addInstruction(bap_ssa_to_instr[suc]);
       }
     bbs.emplace(it.first, dummy);
     dummy.clear();
    }
  cout << "[+] Added Instructions to BBs" << endl;

 // Add CFG-Edges
   for(auto &from_to : cfg)
    {
      for(auto &to : from_to.second)
       {
        bbs[from_to.first].addSuccessor(to);
       }
    }
  cout << "[+] Added CFG-Edges" << endl;
 }


void fill_funcs(  map<uint64_t, BjoernBasicBlockNode> &bbs
                , const map<uint64_t, vector<uint64_t>> &cfg
                , map<uint64_t, BjoernFunctionNode> &funcs
               )
 { 
// Discover Functions
//TODO do it properly... this justs puts it into one big function to make sure everything is plotted
 BjoernFunctionNode func;
  func.setName("all");
  uint64_t first_addr = numeric_limits<uint64_t>::max();
   for(auto &it : bbs)
    {
      if(it.first < first_addr)
       {
        first_addr = it.first;
       }
     func.addBasicBlock(&(it.second));
    }
  func.setAddr(first_addr);
  funcs.emplace(first_addr, func);
  func.clear();
  cout << "[+] \"Discovered\" Functions" << endl;
 }


