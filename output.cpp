#include <iostream>
#include <limits>

using namespace std;

#include "parameters.h"

#include "output.h"

void print_error(const string &message)
{
   cerr << "ERROR: " << message << endl;
   cerr << endl;
   cerr << "Press Enter to print usage and exit. ";

   cin.ignore(numeric_limits<streamsize>::max(), '\n');
   cerr << endl;
}

void print_parameters()
{
   *output_stream << "Display parameters resulting from command line arguments: "
                  << (display_arguments ? "true" : "false")
                  << endl;

   *output_stream << "Display group count: " << (display_group_count ? "true" : "false") << endl;
   *output_stream << "Duplicates allowed: " << (duplicates_allowed ? "true" : "false") << endl;
   *output_stream << "Permutations allowed: " << (permutations_allowed ? "true" : "false") << endl;
   *output_stream << "Zeros allowed: " << (zeros_allowed ? "true" : "false") << endl;
   *output_stream << "N: " << N << endl;
   *output_stream << "K: " << K << endl;
   *output_stream << endl;
}

void print_results(const all_groups_t &all_groups)
{
   auto print_visitor_t = [](const auto &one_group)
   {
      for (my_uint_t one_uint : one_group)
         *output_stream << one_uint << " ";
   };

   for (const one_group_t &one_group : all_groups)
   {
      visit(print_visitor_t, one_group);
      *output_stream << endl;
   }

   if (display_group_count)
   {
      *output_stream << endl;

      *output_stream << "Group count: "
                     << all_groups.size()
                     << endl;
   }

   if (output_stream.get() == &cout)
      *output_stream << endl;
}

void print_usage(const char *program_name)
{
   string pn{program_name != nullptr ? program_name : "<program name>"};

   cerr << "USAGE" << endl;
   cerr << pn << " [-h | --help]" << endl;
   cerr << pn << " [OPTION]... <N> [K]" << endl;

   cerr << "   Generate all groups of positive (or, optionally, non-negative)"
        << endl
        << "   integers that sum to the specified integer N."
        << endl
        << endl;

   cerr << "   K is the size of the groups of integers to be returned."
        << endl
        << "   K = 0 returns all groups that sum to N, regardless of size."
        << endl
        << "   Default: 0"
        << endl
        << endl;

   cerr << "   Optionally, duplicates may be allowed in the generated groups."
        << endl;

   cerr << "   Optionally, all permutations of the generated groups may be generated."
        << endl;

   cerr << endl;
   cerr << "OPTIONS" << endl;
   cerr << "   Mandatory arguments to long options are mandatory for short options too." << endl;
   cerr << endl;
   cerr << "   -a, --arguments     Display parameters resulting from command line arguments." << endl;
   cerr << "   -c, --count         Display the count of groups generated." << endl;
   cerr << "                       Default: Do not display the count of groups generated" << endl;
   cerr << "   -d, --duplicates    Duplicate elements are allowed to be generated." << endl;
   cerr << "                       Default: Do not allow duplicate elements" << endl;
   cerr << "   -h, --help          Print usage; overrides all other flags and exits" << endl;
   cerr << "   -o, --output=<FILE> Send output to <FILE> rather than stdout." << endl;
   cerr << "   -p, --permutations  All permutations of each group should be generated." << endl;
   cerr << "                       (but duplicate elements are not permuted)" << endl;
   cerr << "                       Default: Do not generate permutations" << endl;
   cerr << "   -z, --zero          0 may be used as an element in the groups." << endl;
   cerr << "                       Default: 0 may not be used as an element in the groups" << endl;

   cerr << endl;
   cerr << "RESTRICTIONS" << endl;

   cerr << "   With -z or --zero, K must not be 0. "
           "This would attempt to create groups of infinite size."
        << endl;

   cerr << "   If N = 0, -z or --zero must be present."
        << endl;

   cerr << endl;
   cerr << "EXIT STATUS" << endl;

   cerr << "   0 if groups could be generated as dictated by "
           "the command line arguments and flags."
        << endl;
 
   cerr << "   1 if groups could not be generated as dictated by "
           "the command line arguments and flags."
        << endl;

   cerr << endl;
}
