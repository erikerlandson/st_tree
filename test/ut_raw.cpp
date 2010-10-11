#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_raw)

typedef tree<int> tree_type;
//typedef tree_type::bf_iterator bf_iterator;

BOOST_AUTO_TEST_CASE(default_ctor) {
    tree_type t1;
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK(t1.empty());
}

BOOST_AUTO_TEST_SUITE_END()
