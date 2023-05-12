#ifndef VISITORS_INCLUDED
#define VISITORS_INCLUDED

#include <algorithm>
#include <utility>

using namespace std;

#include "fundamental_types.h"
#include "parameters.h"
#include "type_aliases.h"

class insert_visitor_t
{
   public:
      insert_visitor_t(my_uint_t n_p): n{n_p} {}

      pair<one_group_iterator_t, bool>
      operator()(one_group_vector_t &one_group_vector) const
      {
         if (
               duplicates_allowed ||
               find(
                      one_group_vector.cbegin(),
                      one_group_vector.cend(),
                      n
                   ) == one_group_vector.cend()
            )
         {
            one_group_vector.push_back(n);

            return {one_group_iterator_t{}, true};
         }

         return {one_group_iterator_t{}, false};
      }

      pair<one_group_iterator_t, bool>
      operator()(one_group_set_t &one_group_set) const
      {
         auto rval{one_group_set.insert(n)};
         one_group_set_t::iterator iter{rval.first};
         one_group_iterator_t one_group_iterator{in_place_index<1>, iter};

         return {one_group_iterator, rval.second};
      }

      pair<one_group_iterator_t, bool>
      operator()(one_group_multiset_t &one_group_multiset) const
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
      erase_visitor_t(one_group_iterator_t one_group_iterator_p):
         one_group_iterator{one_group_iterator_p} {}

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

#endif
