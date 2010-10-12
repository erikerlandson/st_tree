#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_raw)

typedef tree<int> tree_type;
//typedef tree_type::bf_iterator bf_iterator;

BOOST_AUTO_TEST_CASE(default_ctor) {
    tree_type t1;
    BOOST_CHECK(t1.empty());
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}

BOOST_AUTO_TEST_CASE(insert_root) {
    tree_type t1;
    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().is_root(), true);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(insert_subnodes) {
    tree_type t1;

    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);

    t1.root().insert(8);
    BOOST_CHECK_EQUAL(t1.size(), 2);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 1);

    t1.root().insert(9);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);

    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_EQUAL(t1.root()[0].data(), 8);
    BOOST_CHECK_EQUAL(t1.root()[1].data(), 9);
}


BOOST_AUTO_TEST_CASE(clear) {
    tree_type t1;

    t1.insert(7);
    t1.root().insert(8);
    t1.root().insert(9);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);

    t1.clear();
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_EQUAL(t1.empty(), true);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(reinsert) {
    tree_type t1;

    t1.insert(7);
    t1.root().insert(8);
    t1.root().insert(9);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);

    t1.insert(3);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);
    BOOST_CHECK_EQUAL(t1.root().data(), 3);
}

BOOST_AUTO_TEST_SUITE_END()
