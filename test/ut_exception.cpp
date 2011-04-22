#include <boost/test/unit_test.hpp>

#include "ootree.h"
#include "ut_common.h"


BOOST_AUTO_TEST_SUITE(ut_exception)


BOOST_AUTO_TEST_CASE(exceptions) {
    parent_exception px("a");
    orphan_exception ox("b");
    cycle_exception cx("c");
    empty_exception ex("d");
    missing_exception mx("e");

    string t;

    t = px.what();
    BOOST_CHECK_EQUAL(t, "a");

    t = ox.what();
    BOOST_CHECK_EQUAL(t, "b");

    t = cx.what();
    BOOST_CHECK_EQUAL(t, "c");

    t = ex.what();
    BOOST_CHECK_EQUAL(t, "d");

    t = mx.what();
    BOOST_CHECK_EQUAL(t, "e");
}


BOOST_AUTO_TEST_SUITE_END() // ut_exception
