#include <cassert>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>

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
                         const my_uint_t N,
                         const my_uint_t K,
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

   build_group(N, N, K, K, one_group, all_groups);
}

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

bool process_args(int argc, char *argv[])
{
   bool args_are_valid{false};

   do
   {
      if (argc < 2)
         break;

      // This regex is intended to be applied to each individual argument,
      // not to the entire command line
      const string arg_regex_string {
                                       "(-a|--arguments)|"
                                       "(-c|--count)|"
                                       "(-d|--duplicates)|"
                                       "(-h|--help)|"
                                       "((-o|--output)=(([[:alnum:]][[:alnum:]_-]*)"
                                          "(\\.[[:alnum:]][[:alnum:]_-]*)?))|"
                                       "(-p|--permutations)|"
                                       "(-z|--zeros)|"
                                       "([0-9]+)"
                                    };

      const regex arg_regex{arg_regex_string};

      // Extract all args as strings
      args_as_strings_t args_as_strings;

      for (int i{1}; i < argc; ++i)
         args_as_strings.push_back(argv[i]);

      // Check if the command line arguments are syntactically OK
      {
         bool bad_arg_found{false};

         for (const string &s : args_as_strings)
         {
            if (smatch m; ! regex_match(s, m, arg_regex))
            {
               bad_arg_found = true;

               stringstream message;
               message << "Invalid argument " << s << " found";
               print_error(message.str());

               break;
            }
         }

         if (bad_arg_found) break;
      }

      // If output is to go to a file, this variable will hold the filename
      string filename;

      {
         // Set the value of command line flags that are not default
         bool help_requested{false};
         my_uint_t arg_index{0};

         for (; arg_index < args_as_strings.size(); ++arg_index)
         {
            const string &s{args_as_strings[arg_index]};

            if (s == "-a" || s == "--arguments")
               display_arguments = true;
            else if (s == "-c" || s == "--count")
               display_group_count = true;
            else if (s == "-d" || s == "--duplicates")
               duplicates_allowed = true;
            else if (s == "-h" || s == "--help")
            {
               help_requested = true;
               break;
            }
            else if (s.find("-o=") == 0 || s.find("--output=") == 0)
            {
               if (s[2] == '=')
                  filename = s.substr(3);
               else if (s[8] == '=')
                  filename = s.substr(9);
               else
                  assert(0); // Should never reach this due to prior regex match
            }
            else if (s == "-p" || s == "--permutations")
               permutations_allowed = true;
            else if (s == "-z" || s == "--zeros")
               zeros_allowed = true;
            else
               break; // We should now be done with flags and ready to read <N>
         }

         if (help_requested) break;

         // Extract <N> [K] command line argument(s)
         // Note that these must be in this order and must be the last argument(s)

         if (arg_index == args_as_strings.size())
         {
            print_error("<N> is required but is missing");

            break;
         }

         try
         {
            N = stoul(args_as_strings[arg_index]);
            ++arg_index;
         }
         catch(...)
         {
            // Should never reach this due to prior regex match
            stringstream message;

            message << "Invalid argument "
                    << args_as_strings[arg_index]
                    << " found where <N> is expected";

            print_error(message.str());

            break;
         }

         // If there's another argument, it must be K (which is optional)
         if (arg_index < args_as_strings.size())
         {
            try
            {
               K = stoul(args_as_strings[arg_index]);
               ++arg_index;
            }
            catch(...)
            {
               // Should never reach this due to prior regex match
               stringstream message;

               message << "Invalid argument "
                       << args_as_strings[arg_index]
                       << " found where [K] is expected";

               print_error(message.str());

               break;
            }
         }

         // Ensure there are no more arguments
         if (arg_index != args_as_strings.size())
         {
            print_error("Too many arguments");

            break;
         }
      }

      // Check if the command line arguments are semantically OK
      if (zeros_allowed && K == 0)
      {
         print_error(
                       "With -z or --zero, K must not be 0. "
                       "This would attempt to create groups of infinite size."
                    );

         break;
      }

      if (! zeros_allowed && N == 0)
      {
         print_error("If N = 0, -z or --zero must be present.");

         break;
      }

      // All that's left is to ensure that if output is to go to a file that that
      // file can be opened.
      if (filename != "")
      {
         ostream_up_t ostream_up{new ofstream{filename}, ostream_conditional_deleter{true}};

         if (! ostream_up || ! (dynamic_cast<ofstream *>(ostream_up.get()))->is_open())
         {
            stringstream message;

            message << "Could not open output file " << filename;
            print_error(message.str());

            break;
         }

         output_stream = move(ostream_up);
      }

      // We made it! All arguments are both syntactically and semantically good!
      args_are_valid = true;
   }
   while (false);

   if (! args_are_valid)
      print_usage(argv[0]);
   else if (display_arguments)
      print_parameters();

   return args_are_valid;
}
