#include "output.h"
#include "parameters.h"
#include "perfect_sum.h"
#include "process_args.h"
#include "type_aliases.h"

int main(int argc, char *argv[])
{
   if (! process_args(argc, argv))
      return 1;

   all_groups_t all_groups;

   find_perfect_sums(N, K, all_groups);
   print_results(all_groups);
}
