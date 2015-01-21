#ifndef __BAP_SSA_H__
#define __BAP_SSA_H__

#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

#include "general.h"

// This class capsules a single BAP-SSA-Expression.
// For SSA-DDG-Generation, not the symbols itself, but
// only unique symbol-ids are saved.
// AKA: No real parsing, just keyword-checking...
// Note: Also operators or "phi(" may appear on right side,
//       but since those don't appear on the left side, the don't cause
//       False-Positives; the are just dead freight.

class BAP_SSA
 {
 public:
  string code; //TODO: remove?
  uint64_t asm_addr;                       // Address of asm-statement
  uint16_t asm_counter;                    // n-th BAP_SSA for that asm-statement
  int64_t left_hand_symbol_id;             // symbol left of assignment; -1 if not present
  std::set<int64_t> right_hand_symbol_ids; // symbols right of assignment

  BAP_SSA(uint64_t asm_addr, uint16_t asm_counter, const string &line, map<string, int64_t> &symbol_to_id);
  bool has_left() const;
  bool has_on_right_side(int64_t symbol);
  string to_string() const;
 };


#endif

