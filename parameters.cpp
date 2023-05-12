#include <iostream>

using namespace std;

#include "parameters.h"

bool display_arguments{false};
bool display_group_count{false};
bool duplicates_allowed{false};
bool permutations_allowed{false};
bool zeros_allowed{false};
ostream_up_t output_stream{&cout, ostream_conditional_deleter{false}};
my_uint_t N{0};
my_uint_t K{0};
