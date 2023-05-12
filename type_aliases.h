#ifndef TYPE_ALIASES_INCLUDED
#define TYPE_ALIASES_INCLUDED

#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <variant>
#include <vector>

using namespace std;

#include "fundamental_types.h"
#include "ostream_conditional_deleter.h"

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

using ostream_up_t = unique_ptr<ostream, ostream_conditional_deleter>;

#endif
