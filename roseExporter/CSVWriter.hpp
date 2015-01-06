#ifndef _CSV_WRITER_HPP
#define _CSV_WRITER_HPP

#include "bjoernNodes.hpp"

class CSVWriter
{
  
private:
  unsigned long long curId;
  
public:
  CSVWriter() : curId(0) {} 

  void writeFunction(BjoernFunctionNode *func);

};


#endif
