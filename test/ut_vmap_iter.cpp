#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_valmap_iterator_adaptor)

BOOST_AUTO_TEST_CASE(default_ctor) {
    typedef vector<int*> container_type;
    typedef container_type::iterator iterator;
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type> > vm_iterator;

    vm_iterator j;
}

BOOST_AUTO_TEST_SUITE_END()
