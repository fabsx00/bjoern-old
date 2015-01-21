#include "BAP_SSA.hpp"

// Helper function:
// Puts id for symbol in map, if not present.
// If present, it returns it.
int64_t get_id_for_symbol(map<string, int64_t> &symbol_to_id, const string &symbol)
 {
   if(symbol_to_id.find(symbol) == symbol_to_id.end())
    {
     symbol_to_id.emplace(symbol, symbol_to_id.size());
    }
  return symbol_to_id[symbol];
 }



string BAP_SSA::to_string() const
 {
 stringstream s;
  s << "0x" << hex << asm_addr << dec;
  s << "\t" << asm_counter << "\n";
  s << "\t" << code << "\n";
  return s.str();
 }


// Apart from filling stuff, this Constructor also splits the line into
// "symbols" and saves their ids
BAP_SSA::BAP_SSA(  uint64_t asm_addr
                 , uint16_t asm_counter
                 , const string &line
                 , map<string, int64_t> &symbol_to_id
                )
 {
  this->code = line;
  this->asm_addr = asm_addr;
  this->asm_counter = asm_counter;

 vector<string> words = split(line, " ");
 uint32_t equal_pos = find(words.begin(), words.end(), "=") - words.begin();
   if(equal_pos != 1 && equal_pos != words.size())
    {
     cout << "[-] Found equal sign at pos " << equal_pos << "; is expected at pos 1 or not at all" << endl;
     cout << "[/] line: " << line << endl;
     exit(1);
    }

   if(equal_pos == 1)
    {
     left_hand_symbol_id = get_id_for_symbol(symbol_to_id, words[0]);

      for(uint32_t i=2, i_end=words.size(); i<i_end; ++i)
       {
        right_hand_symbol_ids.insert(get_id_for_symbol(symbol_to_id, words[i]));
       }
    }
   else
    {
     left_hand_symbol_id = -1;
      for(uint32_t i=0, i_end=words.size(); i<i_end; ++i)
       {
        right_hand_symbol_ids.insert(get_id_for_symbol(symbol_to_id, words[i]));
       }
    }
 }


bool BAP_SSA::has_left() const
 {
  return left_hand_symbol_id != -1;
 }


bool BAP_SSA::has_on_right_side(int64_t symbol)
 {
  return right_hand_symbol_ids.find(symbol) != right_hand_symbol_ids.end();
 }


