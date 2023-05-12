#ifndef PERFECT_SUMS_INCLUDED
#define PERFECT_SUMS_INCLUDED

#include <string>

using namespace std;

#include "fundamental_types.h"
#include "type_aliases.h"

void build_group(
                   const my_uint_t N_original,
                   my_uint_t N_remaining,
                   const my_uint_t K_original,
                   my_uint_t K_remaining,
                   one_group_t &one_group,
                   all_groups_t &all_groups
                );

void find_perfect_sums(
                         const my_uint_t N,
                         const my_uint_t K,
                         all_groups_t &all_groups
                      );

#endif
