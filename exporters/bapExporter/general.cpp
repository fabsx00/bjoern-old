#include "general.h"
#include <time.h>

uint64_t get_file_size(const char *file_name)
 {
 ifstream f(file_name, ios::binary);
   if(!f)
    {
     return -1;
    }

  f.seekg(0, ios_base::beg);
 uint64_t begin = f.tellg();
  f.seekg(0, std::ios_base::end);
 uint64_t ret = f.tellg();
  f.close();

  return ret - begin;
 }


//PRNG has to be seeded
void init_randomness()
 {
  srand(time(NULL));
 }
 

double uniform_rand()
 {
  return rand() / double(RAND_MAX);
 }

// returns from low to high
int random_int(int low, int high)
 {
  return low + rand() / (RAND_MAX / (high - low + 1) + 1);
 }


string int_to_hexstring(int val)
 {
 stringstream s;
  s << hex << val;
  return s.str();
 }

string int_to_string(int val)
 {
 stringstream s;
  s << val;
  return s.str();
 }

bool hexstring_to_int(string s, int &val)
 {
  val = 0;
   if(s.size() == 0)
    {
     val = -1;
     return false;
    }

  bool is_negative = false;
  unsigned int i = 0;
   if(s[0] == '-')
    {
      if(s.size() == 1) return false;
     i=1;
     is_negative = true;
    }

   if(s.size() > i+2 && s[i] == '0' && s[i+1] == 'x') i += 2;

   for(unsigned int end=s.size(); i<end; ++i)
    {
      if(s[i] >= '0' && s[i] <= '9')
       {
        val *= 16;
        val += s[i] - '0';
       }
      else if(s[i] >= 'a' && s[i] <= 'f')
       {
        val *= 16;
        val += s[i] - 'a' + 10;
       }
      else if(s[i] >= 'A' && s[i] <= 'F')
       {
        val *= 16;
        val += s[i] - 'A' + 10;
       }
      else
       {
        val = -1;
        return false;
       }
    }
//cout << "stringToInt, returning: " << val << endl;
   if(is_negative) val = -val;
  return true;
 }


bool hexstring_to_int(string s, unsigned int &val)
 {
  val = 0;
   if(s.size() == 0)
    {
     val = -1;
     return false;
    }

  unsigned int i = 0;
   if(s.size() > i+2 && s[i] == '0' && s[i+1] == 'x') i += 2;

   for(unsigned int end=s.size(); i<end; ++i)
    {
      if(s[i] >= '0' && s[i] <= '9')
       {
        val *= 16;
        val += s[i] - '0';
       }
      else if(s[i] >= 'a' && s[i] <= 'f')
       {
        val *= 16;
        val += s[i] - 'a' + 10;
       }
      else if(s[i] >= 'A' && s[i] <= 'F')
       {
        val *= 16;
        val += s[i] - 'A' + 10;
       }
      else
       {
        val = -1;
        return false;
       }
    }
//cout << "stringToInt, returning: " << val << endl;
  return true;
 }



int hexstring_to_int(string s)
 {
 int val;
  hexstring_to_int(s, val);
  return val;
 }



bool string_to_int(string s, int &val)
 {
//cout << "stringToInt, called with: " << s << endl;
  val = 0;
   if(s.size() == 0)
    {
     val = -1;
     return false;
    }

  bool is_negative = false;
  unsigned int i = 0;
   if(s[0] == '-')
    {
      if(s.size() == 1) return false;
     i=1;
     is_negative = true;
    }

   for(unsigned int end=s.size(); i<end; ++i)
    {
      if(s[i] >= '0' && s[i] <= '9')
       {
        val *= 10;
        val += s[i] - '0';
       }
      else
       {
        val = -1;
        return false;
       }
    }
//cout << "stringToInt, returning: " << val << endl;
   if(is_negative) val = -val;
  return true;
 }
 

bool string_to_int(string s, unsigned int &val)
 {
//cout << "stringToInt, called with: " << s << endl;
  val = 0;
   if(s.size() == 0)
    {
     return false;
    }

  unsigned int i = 0;

   for(unsigned int end=s.size(); i<end; ++i)
    {
      if(s[i] >= '0' && s[i] <= '9')
       {
        val *= 10;
        val += s[i] - '0';
       }
      else
       {
        return false;
       }
    }
//cout << "stringToInt, returning: " << val << endl;
  return true;
 }



bool string_to_double(string s, double &val)
 {
 stringstream ss;
  ss << s;
   try
    {
     ss >> val;
    }
   catch(...)
    {
     return false;
    }

  return true;
 }


int string_to_int(string s)
 {
 int val;
  string_to_int(s, val);
  return val;
 }


bool copy_file(string src_file_name, string dest_file_name)
 {
 ifstream src_file(src_file_name.c_str(), ios::binary);
   if(!src_file)
    {
     return false;
    }

 ofstream dest_file(dest_file_name.c_str(), ios::binary);
   if(!dest_file)
    {
     src_file.close();
     return false;
    }

  dest_file << src_file.rdbuf();
   if(!dest_file)
    {
     src_file.close();
     dest_file.close();
     return false;
    }

  src_file.close();
  dest_file.close();
  return true;
 }


bool to_file(string dest_file_name, const char *data, uint64_t length)
 {
 ofstream dest_file(dest_file_name.c_str(), ios::binary);
   if(!dest_file)
    {
     return false;
    }

  dest_file.write(data, length);
   if(!dest_file)
    {
     dest_file.close();
     return false;
    }

  dest_file.close();
  return true;
 }


bool to_file(string dest_file_name, string s)
 {
  return to_file(dest_file_name, s.c_str(), s.size()+1);
 }


bool to_file(string dest_file_name, const vector<string> &s, const string &seperator)
 {
 const char *sep = seperator.c_str();
 int sep_len = seperator.size();
 ofstream dest_file(dest_file_name.c_str(), ios::binary);
   if(!dest_file)
    {
     return false;
    }

   for(unsigned int i=0; i<s.size(); ++i)
    {
     dest_file.write(s[i].c_str(), s[i].size());
      if(!dest_file)
       {
        dest_file.close();
        return false;
       }
     dest_file.write(sep, sep_len);
      if(!dest_file)
       {
        dest_file.close();
        return false;
       }
    }

  dest_file.close();
  return true;
 }



string from_file(string file_name)
 {
 std::ifstream file(file_name.c_str());
   if(!file) return "";

 std::stringstream ss;
  ss << file.rdbuf();
 std::string s = ss.str();
  file.close();
  ss.clear();
  return s;
 }


vector<string> split(const string &s, const string &seperator)
 {
 vector<string> ret;
   if(s.size() == 0)
    {
     return ret;
    }

   if(seperator.size() == 0)
    {
     ret.push_back(s);
     return ret;
    }

 string::size_type pos, last_pos = 0;
   for(;;)
    {
     pos = s.find(seperator, last_pos);
      if(pos == string::npos)
       {
        ret.push_back(s.substr(last_pos));
        return ret;
       }
      else if(pos == last_pos)
       {
        ret.push_back("");
       }
      else
       {
        ret.push_back(s.substr(last_pos, pos-last_pos));
       }
     last_pos = pos + seperator.size();
    }
 }


string trim(const string &s)
 {
   if(s == "") return "";
 unsigned int start_pos = 0, end=s.size();
   for(;; ++start_pos)
    {
      if(   start_pos == end
         || (   s[start_pos] != ' '
             && s[start_pos] != '\t'
             && s[start_pos] != '\n'
             && s[start_pos] != '\r'))
       {
        break;
       }
    }
//cout << "start_pos: " << start_pos << endl;
   if(start_pos == end)
    {
//cout << "return empty, because start == s.size" << endl;
     return "";
    }

 unsigned int end_pos = end-1;
   for(;; --end_pos)
    {
      if(  end_pos < start_pos
         || (   s[end_pos] != ' '
             && s[end_pos] != '\t'
             && s[end_pos] != '\n'
             && s[end_pos] != '\r'))
       {
        break;
       }
    }

   if(end_pos < start_pos)
    {
//cout << "return empty, because end < start" << endl;
     return "";
    }

//cout << "start_pos: " << start_pos << endl;
//cout << "end_pos: " << end_pos << endl;
  return s.substr(start_pos, end_pos-start_pos+1);
 }


string replace(const string &in, const string &what, const string &by)
{
  if(in == "" || what == "") return "";

stringstream s;
string::size_type pos = 0, last = 0, in_size=in.size(), what_size=what.size();

  while(1)
   {
    pos = in.find(what, pos);
//cout << "pos: " << pos << endl;
     if(pos == string::npos)
      {
       s << in.substr(last, in_size-last);
       break;
      }
    s << in.substr(last, pos-last);
    s << by;
    pos += what_size;
     if(pos == in_size) break;
    last = pos;
   }

 return s.str();
}


bool starts_with(const string &s, const string &what)
 {
 unsigned int end = what.size();
   if(s.size() < end) return false;
   for(unsigned int i=0; i<end; ++i)
    {
      if(s[i] != what[i]) return false;
    }
  return true;
 }


bool ends_with(const string &s, const string &what)
 {
 unsigned int end = what.size();
   if(s.size() < end) return false;
   for(unsigned int i=0, j=s.size()-end; i<end; ++i, ++j)
    {
      if(s[j] != what[i]) return false;
    }
  return true;
 }



