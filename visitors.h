#ifndef VISITORS_INCLUDED
#define VISITORS_INCLUDED

#include <utility>

using namespace std;

#include "fundamental_types.h"
#include "type_aliases.h"

class insert_visitor_t
{
   public:
      insert_visitor_t(my_uint_t n_p);

      pair<one_group_iterator_t, bool>
      operator()(one_group_vector_t &one_group_vector) const;

      pair<one_group_iterator_t, bool>
      operator()(one_group_set_t &one_group_set) const;

      pair<one_group_iterator_t, bool>
      operator()(one_group_multiset_t &one_group_multiset) const;

   private:
      my_uint_t n;
};

class erase_visitor_t
{
   public:
      erase_visitor_t(one_group_iterator_t one_group_iterator_p);

      void operator()(one_group_vector_t &one_group_vector) const;
      void operator()(one_group_set_t &one_group_set) const;
      void operator()(one_group_multiset_t &one_group_multiset) const;

   private:
      one_group_iterator_t one_group_iterator;
};

#endif
