#ifndef OSTREAM_CONDITIONAL_DELETER_INCLUDED
#define OSTREAM_CONDITIONAL_DELETER_INCLUDED

#include <ostream>

using namespace std;

class ostream_conditional_deleter
{
   public:
      ostream_conditional_deleter(bool must_delete_p): must_delete{must_delete_p} {}
      void operator()(ostream *os) const { if (must_delete) delete os; }

   private:
      bool must_delete;
};

#endif
