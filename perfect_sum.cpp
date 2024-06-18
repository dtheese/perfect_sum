#include <string>

using namespace std;

#include "parameters.h"
#include "visitors.h"

#include "perfect_sum.h"

void build_group(
                   const my_uint_t N_original,
                   my_uint_t N_remaining,
                   const my_uint_t K_original,
                   my_uint_t K_remaining,
                   one_group_t &one_group,
                   all_groups_t &all_groups
                )
{
   if (K_original > 0)
   {
      if (K_remaining == 0)
      {
         if (N_remaining == 0)
            all_groups.insert(one_group);

         return;
      }
   }
   else
   {
      if (N_remaining == 0)
      {
         all_groups.insert(one_group);

         return;
      }
   }

   if (N_remaining > N_original)
      return;

   for (my_uint_t n{zeros_allowed ? my_uint_t(0) : my_uint_t(1)}; n <= N_original; ++n)
   {
      auto rval{visit(insert_visitor_t{n}, one_group)};

      if (rval.second)
      {
         build_group(
                       N_original,
                       N_remaining - n,
                       K_original,
                       K_remaining - 1,
                       one_group,
                       all_groups
                    );

         visit(erase_visitor_t{rval.first}, one_group);
      }
   }
}

void find_perfect_sums(
                         const my_uint_t N_p,
                         const my_uint_t K_p,
                         all_groups_t &all_groups
                      )
{
   one_group_t one_group{};

   if (permutations_allowed)
      one_group = one_group_vector_t{};
   else
   {
      if (duplicates_allowed)
         one_group = one_group_multiset_t{};
      else
         one_group = one_group_set_t{};
   }

   build_group(N_p, N_p, K_p, K_p, one_group, all_groups);
}
