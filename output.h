#ifndef OUTPUT_INCLUDED
#define OUTPUT_INCLUDED

#include <string>

using namespace std;

#include "type_aliases.h"

void print_error(const string &message);
void print_parameters();
void print_results(const all_groups_t &all_groups);
void print_usage(const char *program_name);
bool process_args(int argc, char *argv[]);

#endif
