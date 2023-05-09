#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

namespace
{
   using my_uint_t = unsigned long int;
   using args_as_strings_t = vector<string>;

   using one_group_vector_t = vector<my_uint_t>;
   using one_group_set_t = set<my_uint_t>;
   using one_group_multiset_t = multiset<my_uint_t>;
   using one_group_t = variant<one_group_vector_t, one_group_set_t, one_group_multiset_t>;

   using one_group_iterator_t = variant<
                                          one_group_vector_t::iterator,
                                          one_group_set_t::iterator,
                                          one_group_multiset_t::iterator
                                       >;

   using all_groups_t = set<one_group_t>;

   class ostream_conditional_deleter
   {
      public:
         ostream_conditional_deleter(bool must_delete_p): must_delete{must_delete_p} {}
         void operator()(ostream *os) const { if (must_delete) delete os; }

      private:
         bool must_delete;
   };

   using ostream_up_t = unique_ptr<ostream, ostream_conditional_deleter>;

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

   void print_results(const all_groups_t &all_groups);
   void print_usage(const char *program_name);
   bool process_args(int argc, char *argv[]);

   // Command line arguments and flags
   bool display_arguments{false};
   bool display_group_count{false};
   bool duplicates_allowed{false};
   bool permutations_allowed{false};
   bool zeros_allowed{false};
   ostream_up_t output_stream{&cout, ostream_conditional_deleter{false}};
   my_uint_t N{0};
   my_uint_t K{0};
};

int main(int argc, char *argv[])
{
   if (! process_args(argc, argv))
   {
      cerr << endl;

      return 1;
   }

   if (display_arguments)
   {
      *output_stream << endl;
      *output_stream << "Display parameters resulting from command line arguments: " << (display_arguments ? "true" : "false") << endl;
      *output_stream << "Display group count: " << (display_group_count ? "true" : "false") << endl;
      *output_stream << "Duplicates allowed: " << (duplicates_allowed ? "true" : "false") << endl;
      *output_stream << "Permutations allowed: " << (permutations_allowed ? "true" : "false") << endl;
      *output_stream << "Zeros allowed: " << (zeros_allowed ? "true" : "false") << endl;
      *output_stream << "N: " << N << endl;
      *output_stream << "K: " << K << endl;
   }

   all_groups_t all_groups;

   find_perfect_sums(N, K, all_groups);
   print_results(all_groups);

   *output_stream << endl;

   return 0;
}

namespace
{
   class insert_visitor_t
   {
      public:
         insert_visitor_t(my_uint_t n_p): n{n_p} {}

         pair<one_group_iterator_t, bool> operator()(one_group_vector_t &one_group_vector) const
         {
            if (
                  duplicates_allowed ||
                  find(one_group_vector.cbegin(), one_group_vector.cend(), n) == one_group_vector.cend())
            {
               one_group_vector.push_back(n);

               return {one_group_iterator_t{}, true};
            }

            return {one_group_iterator_t{}, false};
         }

         pair<one_group_iterator_t, bool> operator()(one_group_set_t &one_group_set) const
         {
            auto rval{one_group_set.insert(n)};
            one_group_set_t::iterator iter{rval.first};
            one_group_iterator_t one_group_iterator{in_place_index<1>, iter};

            return {one_group_iterator, rval.second};
         }

         pair<one_group_iterator_t, bool> operator()(one_group_multiset_t &one_group_multiset) const
         {
            one_group_multiset_t::iterator iter{one_group_multiset.insert(n)};
            one_group_iterator_t one_group_iterator{in_place_index<2>, iter};

            return {one_group_iterator, true};
         }

      private:
         my_uint_t n;
   };

   class erase_visitor_t
   {
      public:
         erase_visitor_t(one_group_iterator_t one_group_iterator_p): one_group_iterator{one_group_iterator_p} {}

         void operator()(one_group_vector_t &one_group_vector) const
         {
            one_group_vector.pop_back();
         }

         void operator()(one_group_set_t &one_group_set) const
         {
            one_group_set.erase(get<1>(one_group_iterator));
         }

         void operator()(one_group_multiset_t &one_group_multiset) const
         {
            one_group_multiset.erase(get<2>(one_group_iterator));
         }

      private:
         one_group_iterator_t one_group_iterator;
   };

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
            build_group(N_original, N_remaining - n, K_original, K_remaining - 1, one_group, all_groups);
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

   void print_results(const all_groups_t &all_groups)
   {
      *output_stream << endl;

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
         *output_stream << "Group count: " << all_groups.size() << endl;
      }
   }

   void print_usage(const char *program_name)
   {
      string pn{program_name != nullptr ? program_name : "<program name>"};

      cerr << endl;
      cerr << "USAGE" << endl;
      cerr << pn << " [OPTION]... <N> [K]" << endl;
      cerr << "   Generate all groups of positive (or, optionally, non-negative) integers that sum to the specified integer N." << endl;
      cerr << "   K is the size of the groups of integers to be returned (K = 0 denotes that any size is acceptable). Default is 0." << endl;
      cerr << "   Optionally, duplicates may be allowed in the generated groups." << endl;
      cerr << "   Optionally, all permutations of the generated groups may also be generated." << endl;

      cerr << endl;
      cerr << "OPTIONS" << endl;
      cerr << "   Mandatory arguments to long options are mandatory for short options too." << endl;
      cerr << endl;
      cerr << "   -a, --arguments     Display parameters resulting from command line arguments" << endl;
      cerr << "   -c, --count         Display the count of groups generated" << endl;
      cerr << "                       Default: Do not display the count of groups generated" << endl;
      cerr << "   -d, --duplicates    Duplicate elements are allowed to be generated" << endl;
      cerr << "                       Default: Do not allow duplicate elements" << endl;
      cerr << "   -h, --help          Print usage; overrides all other flags and exits" << endl;
      cerr << "   -o, --output=<FILE> Send output to <FILE> rather than stdout" << endl;
      cerr << "   -p, --permutations  All permutations of each group should be generated" << endl;
      cerr << "                       (but duplicate elements are not permuted)" << endl;
      cerr << "                       Default: Do not generate permutations" << endl;
      cerr << "   -z, --zero          0 may be used as an element in the groups" << endl;
      cerr << "                       Default: 0 may not be used as an element in the groups" << endl;

      cerr << endl;
      cerr << "RESTRICTIONS" << endl;
      cerr << "   With -z or --zero, K must not be 0. This would attempt to create groups of infinite size." << endl;
      cerr << "   If N = 0, -z or --zero must be present." << endl;

      cerr << endl;
      cerr << "EXIT STATUS" << endl;
      cerr << "   0 if groups could be generated as dictated by the command line arguments and flags" << endl;
      cerr << "   1 if groups could not be generated as dictated by the command line arguments and flags" << endl;
   }

   bool process_args(int argc, char *argv[])
   {
      bool args_are_valid{false};

      do
      {
         if (argc < 2) break;

         // This regex is intended to be applied to each individual argument, not to the entire command line
         const string arg_regex_string {
                                          "(-a|--arguments)|"
                                          "(-c|--count)|"
                                          "(-d|--duplicates)|"
                                          "(-h|--help)|"
                                          "((-o|--output)=(([[:alnum:]][[:alnum:]_-]*)(\\.)([[:alnum:]][[:alnum:]_-]*)?))|"
                                          "(-p|--permutations)|"
                                          "(-z|--zeros)|"
                                          "([0-9]+)"
                                       };

         const regex arg_regex(arg_regex_string);

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
                  break;
               }
            }

            if (bad_arg_found) break;
         }

         {
            // Set the value of command line flags that are not default
            bool bad_arg_found{false};
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
                  string filename;

                  if (s[2] == '=')
                     filename = s.substr(3);
                  else if (s[8] == '=')
                     filename = s.substr(9);
                  else
                     assert(0); // Should never reach this due to prior regex match

                  ostream_up_t ostream_up{new ofstream{filename}, ostream_conditional_deleter{true}};

                  if (! ostream_up || ! (dynamic_cast<ofstream *>(ostream_up.get()))->is_open())
                  {
                     bad_arg_found = true;
                     break;
                  }

                  output_stream = move(ostream_up);
               }
               else if (s == "-p" || s == "--permutations")
                  permutations_allowed = true;
               else if (s == "-z" || s == "--zeros")
                  zeros_allowed = true;
               else
                  break; // Now we expect no more command line flags
            }

            if (bad_arg_found || help_requested) break;

            // Extract <N> [K] command line argument(s)
            // Note that these must be in this order and must be the last argument(s)
            if (arg_index == args_as_strings.size()) break; // <N> is required but missing
            try {N = stoul(args_as_strings[arg_index++]);} catch(...) {break;}

            // If there's another argument, it must be K (which is optional)
            if (arg_index < args_as_strings.size())
               try {K = stoul(args_as_strings[arg_index++]);} catch(...) {break;}

            // Ensure there are no more arguments
            if (arg_index != args_as_strings.size()) break;
         }

         // Check if the command line arguments are semantically OK
         if (zeros_allowed && K == 0) break;
         if (! zeros_allowed && N == 0) break;

         // We made it! All arguments are both syntactically and semantically good!
         args_are_valid = true;
      }
      while (false);

      if (! args_are_valid)
         print_usage(argv[0]);

      return args_are_valid;
   }
};
