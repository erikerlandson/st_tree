#include <boost/test/unit_test.hpp>

#include "ootree.h"

using namespace ootree;
using namespace std;

BOOST_AUTO_TEST_SUITE(ut_raw)


BOOST_AUTO_TEST_CASE(default_ctor) {
    tree<int> t1;
    BOOST_CHECK(t1.empty());
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}

BOOST_AUTO_TEST_CASE(insert_root) {
    tree<int> t1;
    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().is_root(), true);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(insert_subnodes) {
    tree<int> t1;

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
    tree<int> t1;

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
    tree<int> t1;

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


BOOST_AUTO_TEST_CASE(erase) {
    tree<int> t1;

    t1.insert(7);
    t1.root().insert(8);

    t1.root().erase(t1.root().begin());
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
}


BOOST_AUTO_TEST_CASE(bf_iterator_empty) {
    tree<int> t1;
    BOOST_CHECK_EQUAL(t1.bf_begin() == t1.bf_end(), true);
    BOOST_CHECK_EQUAL(t1.bf_begin() != t1.bf_end(), false);
}


BOOST_AUTO_TEST_CASE(bf_iterator) {
    tree<int> t1;
    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t1.root()[0].insert(7);
    t1.root()[1].insert(13);
    t1.root()[0].insert(11);
    t1.root()[1].insert(17);
    BOOST_CHECK_EQUAL(t1.size(), 7);
    BOOST_CHECK_EQUAL(t1.depth(), 3);

    int ref[] = { 2, 3, 5, 7, 11, 13, 17 };
    int nref = sizeof(ref)/sizeof(*ref);

    BOOST_CHECK_EQUAL(int(t1.size()), nref);

    // pre-increment
    int* jr = ref;
    for (typeof(t1.bf_end()) j(t1.bf_begin());  j != t1.bf_end();  ++j,++jr) {
        if (jr - ref  >= nref) break;
        BOOST_CHECK_EQUAL(j->data(), *jr);
    }
    BOOST_CHECK_EQUAL(int(jr-ref), nref);

    // post-increment
    jr = ref;
    for (typeof(t1.bf_end()) j(t1.bf_begin());  j != t1.bf_end();  j++,++jr) {
        if (jr - ref  >= nref) break;
        BOOST_CHECK_EQUAL(j->data(), *jr);
    }
    BOOST_CHECK_EQUAL(int(jr-ref), nref);
}

BOOST_AUTO_TEST_SUITE_END()
