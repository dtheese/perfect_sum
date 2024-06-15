#include <cassert>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>

using namespace std;

#include "ostream_conditional_deleter.h"
#include "output.h"
#include "parameters.h"
#include "type_aliases.h"

#include "process_args.h"

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
            else if (s.starts_with("-o=") || s.starts_with("--output="))
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
