#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

uint64_t get_file_size(const char *file_name);

//PRNG has to be seeded
void init_randomness();

double uniform_rand();

// returns from low to high
int random_int(int low, int high);

string int_to_string(int val);
string int_to_hexstring(int val);

bool string_to_int(string s, int &val);
bool string_to_int(string s, unsigned int &val);
int string_to_int(string s);
bool string_to_double(string s, double &val);

bool hexstring_to_int(string s, int &val);
int hexstring_to_int(string s);
bool hexstring_to_int(string s, unsigned int &val);

bool copy_file(string src_file_name, string dest_file_name);
bool to_file(string dest_file_name, const char *data, uint64_t length);
bool to_file(string dest_file_name, string s);
bool to_file(string dest_file_name, const vector<string> &s, const string &seperator);
string from_file(string file_name);

vector<string> split(const string &s, const string &seperator);
string trim(const string &s);
string replace(const string &in, const string &what, const string &by);
bool starts_with(const string &s, const string &what);
bool ends_with(const string &s, const string &what);
#endif

