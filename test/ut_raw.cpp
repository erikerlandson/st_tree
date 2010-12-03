#include <boost/test/unit_test.hpp>

#include <iostream>
#include <sstream>

#include "ootree.h"

using namespace ootree;
using namespace std;


BOOST_AUTO_TEST_SUITE(ut_raw)


#define CHECK_TREE(t, meth, ref) { \
    stringstream tst; \
\
    for (typeof(t.bf_end()) j(t.bf_begin());  j != t.bf_end();  ++j) { \
        if (j != t.bf_begin()) tst << " "; \
        tst << j->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}


#define CHECK_TREE_DF_POST(t, meth, ref) { \
    stringstream tst; \
\
    for (typeof(t.df_post_end()) j(t.df_post_begin());  j != t.df_post_end();  ++j) { \
        if (j != t.df_post_begin()) tst << " "; \
        tst << j->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}


#define CHECK_TREE_DF_PRE(t, meth, ref) { \
    stringstream tst; \
\
    for (typeof(t.df_pre_end()) j(t.df_pre_begin());  j != t.df_pre_end();  ++j) { \
        if (j != t.df_pre_begin()) tst << " "; \
        tst << j->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}


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


BOOST_AUTO_TEST_CASE(erase_noarg) {
    tree<int> t1;

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t1.root()[0].insert(7);
    t1.root()[0].insert(11);
    t1.root()[1].insert(13);
    t1.root()[1].insert(17);

    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    t1.root()[1].erase();
    CHECK_TREE(t1, data(), "2 3 7 11");

    t1.root()[0][1].erase();
    CHECK_TREE(t1, data(), "2 3 7");

    t1.root().erase();
    BOOST_CHECK_EQUAL(t1.empty(), true);
    CHECK_TREE(t1, data(), "");
}


BOOST_AUTO_TEST_CASE(bf_iterator_empty) {
    tree<int> t1;
    BOOST_CHECK_EQUAL(t1.bf_begin() == t1.bf_end(), true);
    BOOST_CHECK_EQUAL(t1.bf_begin() != t1.bf_end(), false);
}


BOOST_AUTO_TEST_CASE(bf_iterator) {
    tree<int> t1;
    CHECK_TREE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");

    t1.root()[0].insert(7);
    t1.root()[1].insert(13);
    t1.root()[0].insert(11);
    t1.root()[1].insert(17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
}


BOOST_AUTO_TEST_CASE(df_post_iterator_empty) {
    tree<int> t1;
    BOOST_CHECK_EQUAL(t1.df_post_begin() == t1.df_post_end(), true);
    BOOST_CHECK_EQUAL(t1.df_post_begin() != t1.df_post_end(), false);
}


BOOST_AUTO_TEST_CASE(df_post_iterator) {
    tree<int> t1;
    CHECK_TREE_DF_POST(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_POST(t1, data(), "2");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE_DF_POST(t1, data(), "3 5 2");

    t1.root()[0].insert(7);
    t1.root()[1].insert(13);
    t1.root()[0].insert(11);
    t1.root()[1].insert(17);
    CHECK_TREE_DF_POST(t1, data(), "7 11 3 13 17 5 2");
}


BOOST_AUTO_TEST_CASE(df_pre_iterator_empty) {
    tree<int> t1;
    BOOST_CHECK_EQUAL(t1.df_pre_begin() == t1.df_pre_end(), true);
    BOOST_CHECK_EQUAL(t1.df_pre_begin() != t1.df_pre_end(), false);
}


BOOST_AUTO_TEST_CASE(df_pre_iterator) {
    tree<int> t1;
    CHECK_TREE_DF_PRE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_PRE(t1, data(), "2");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 5");

    t1.root()[0].insert(7);
    t1.root()[1].insert(13);
    t1.root()[0].insert(11);
    t1.root()[1].insert(17);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 7 11 5 13 17");
}



BOOST_AUTO_TEST_CASE(node_ply) {
    tree<int> t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, ply(), "0 1 1");

    t1.root()[0].insert(7);
    t1.root()[0].insert(11);
    t1.root()[1].insert(13);
    t1.root()[1].insert(17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, ply(), "0 1 1 2 2 2 2");

    t1.root().insert(77);
    CHECK_TREE(t1, data(), "2 3 5 77 7 11 13 17");
    CHECK_TREE(t1, ply(), "0 1 1 1 2 2 2 2");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 5 77 13 17");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 77");
    CHECK_TREE(t1, ply(), "0 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");
}

BOOST_AUTO_TEST_CASE(node_depth) {
    tree<int> t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, depth(), "1");
    CHECK_TREE(t1, ply(), "0");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, ply(), "0 1 1");

    t1.root()[0].insert(7);
    t1.root()[0].insert(11);
    t1.root()[1].insert(13);
    t1.root()[1].insert(17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, depth(), "3 2 2 1 1 1 1");
    CHECK_TREE(t1, ply(), "0 1 1 2 2 2 2");

    t1.root().insert(77);
    CHECK_TREE(t1, data(), "2 3 5 77 7 11 13 17");
    CHECK_TREE(t1, depth(), "3 2 2 1 1 1 1 1");
    CHECK_TREE(t1, ply(), "0 1 1 1 2 2 2 2");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 5 77 13 17");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 77");
    CHECK_TREE(t1, depth(), "2 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, depth(), "1");
}

BOOST_AUTO_TEST_CASE(node_subtree_size) {
    tree<int> t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, subtree_size(), "1");

    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, subtree_size(), "3 1 1");

    t1.root()[0].insert(7);
    t1.root()[0].insert(11);
    t1.root()[1].insert(13);
    t1.root()[1].insert(17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, subtree_size(), "7 3 3 1 1 1 1");

    t1.root().insert(77);
    CHECK_TREE(t1, data(), "2 3 5 77 7 11 13 17");
    CHECK_TREE(t1, subtree_size(), "8 3 3 1 1 1 1 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 5 77 13 17");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2 77");
    CHECK_TREE(t1, subtree_size(), "2 1");

    t1.root().erase(t1.root().begin());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, subtree_size(), "1");
}

BOOST_AUTO_TEST_CASE(erase_node) {
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

    t1.root().erase(t1.root().begin());
    BOOST_CHECK_EQUAL(t1.size(), 4);
    BOOST_CHECK_EQUAL(t1.depth(), 3);

    CHECK_TREE(t1, data(), "2 5 13 17");
    CHECK_TREE(t1, ply(), "0 1 2 2");
    CHECK_TREE(t1, subtree_size(), "4 3 1 1");
}

BOOST_AUTO_TEST_CASE(clear_node) {
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

    t1.root().clear();
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);

    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");
    CHECK_TREE(t1, subtree_size(), "1");
}

BOOST_AUTO_TEST_CASE(node_op_equal) {
    tree<int> t1;
    t1.insert(2);
    t1.root().insert(3);

    tree<int> t2;
    t2.insert(5);
    t2.root().insert(7);
    t2.root().insert(11);

    BOOST_CHECK_EQUAL(t1.root().data(), 2);

    t1.root()[0] = t2.root();
    BOOST_CHECK_EQUAL(t1.size(), 4);
    BOOST_CHECK_EQUAL(t1.depth(), 3);

    CHECK_TREE(t1, data(), "2 5 7 11");
    CHECK_TREE(t1, ply(), "0 1 2 2");
    CHECK_TREE(t1, subtree_size(), "4 3 1 1");

    CHECK_TREE(t2, data(), "5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");
}


BOOST_AUTO_TEST_CASE(node_op_equal_root) {
    tree<int> t1;
    t1.insert(2);
    t1.root().insert(3);

    tree<int> t2;
    t2.insert(5);
    t2.root().insert(7);
    t2.root().insert(11);

    t1.root() = t2.root();
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);

    CHECK_TREE(t1, data(), "5 7 11");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");

    CHECK_TREE(t2, data(), "5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
}


BOOST_AUTO_TEST_CASE(node_op_equal_subtree) {
    tree<int> t1;

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t1.root()[0].insert(7);
    t1.root()[0].insert(11);
    t1.root()[1].insert(13);
    t1.root()[1].insert(17);

    BOOST_CHECK_THROW(t1.root()[0] = t1.root(), ootree::exception);

    t1.root() = t1.root()[0];
    CHECK_TREE(t1, data(), "3 7 11");
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);

    t1.root()[1].insert(13);
    t1.root()[1].insert(17);
    t1.root()[1] = t1.root()[1][1];
    CHECK_TREE(t1, data(), "3 7 17");
}


BOOST_AUTO_TEST_CASE(node_swap) {
    tree<int> t1;
    tree<int> t2;
    
    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t1.root()[0].insert(7);
    t1.root()[0].insert(11);

    t2.insert(102);
    t2.root().insert(103);
    t2.root().insert(105);
    t2.root()[0].insert(107);
    t2.root()[0].insert(111);

    // swap a leaf
    swap(t1.root()[0][0], t2.root()[0][0]);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 3 5 107 11");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");

    BOOST_CHECK_EQUAL(t2.size(), 5);
    BOOST_CHECK_EQUAL(t2.depth(), 3);
    CHECK_TREE(t2, data(), "102 103 105 7 111");
    CHECK_TREE(t2, ply(), "0 1 1 2 2");
    CHECK_TREE(t2, depth(), "3 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "5 3 1 1 1");

    // put it back
    swap(t1.root()[0][0], t2.root()[0][0]);

    // swap an internal
    swap(t1.root()[0], t2.root()[0]);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 103 5 107 111");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");
    BOOST_CHECK_EQUAL(t2.size(), 5);
    BOOST_CHECK_EQUAL(t2.depth(), 3);
    CHECK_TREE(t2, data(), "102 3 105 7 11");
    CHECK_TREE(t2, ply(), "0 1 1 2 2");
    CHECK_TREE(t2, depth(), "3 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "5 3 1 1 1");

    // put it back
    swap(t1.root()[0], t2.root()[0]);

    // swap the root
    swap(t1.root(), t2.root());
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "102 103 105 107 111");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");
    BOOST_CHECK_EQUAL(t2.size(), 5);
    BOOST_CHECK_EQUAL(t2.depth(), 3);
    CHECK_TREE(t2, data(), "2 3 5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1 2 2");
    CHECK_TREE(t2, depth(), "3 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "5 3 1 1 1");

    // put it back
    swap(t1.root(), t2.root());

    // swap different plies
    swap(t1.root(), t2.root()[0]);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    CHECK_TREE(t1, data(), "103 107 111");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
    BOOST_CHECK_EQUAL(t2.size(), 7);
    BOOST_CHECK_EQUAL(t2.depth(), 4);
    CHECK_TREE(t2, data(), "102 2 105 3 5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1 2 2 3 3");
    CHECK_TREE(t2, depth(), "4 3 1 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "7 5 1 3 1 1 1");

    // put them back
    swap(t1.root(), t2.root()[0]);

    // on same tree
    swap(t1.root()[0], t1.root()[1]);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 5 3 7 11");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 1 2 1 1");
    CHECK_TREE(t1, subtree_size(), "5 1 3 1 1");

    // put them back
    swap(t1.root()[0], t1.root()[1]);

    // no-no
    BOOST_CHECK_THROW(swap(t1.root(), t1.root()[0]), ootree::exception);
    BOOST_CHECK_THROW(swap(t1.root()[1], t1.root()), ootree::exception);
}


BOOST_AUTO_TEST_CASE(graft) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);    

    t2.graft(t1.root());

    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    t1.insert(2);
    t2.graft(t1);

    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);

    t2.graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2 3 5");    

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);

    t2.graft(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2 3 5");    
}

BOOST_AUTO_TEST_CASE(insert_node) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);    

    t2.insert(t1.root());

    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "2");    
}

BOOST_AUTO_TEST_CASE(node_op_equality) {
    tree<int> t1;

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t1.root()[0].insert(7);
    t1.root()[1].insert(13);
    t1.root()[0].insert(11);
    t1.root()[1].insert(17);

    tree<int> t2;

    t2.insert(2);
    t2.root().insert(3);
    t2.root().insert(5);
    t2.root()[0].insert(7);
    t2.root()[1].insert(13);
    t2.root()[0].insert(11);
    t2.root()[1].insert(17);

    BOOST_CHECK_EQUAL(t1.root() == t1.root(), true);
    BOOST_CHECK_EQUAL(t1.root()[0] == t1.root()[0], true);
    BOOST_CHECK_EQUAL(t1.root()[0][0] == t1.root()[0][0], true);

    BOOST_CHECK_EQUAL(t1.root() == t2.root(), true);
    BOOST_CHECK_EQUAL(t1.root()[0] == t2.root()[0], true);
    BOOST_CHECK_EQUAL(t1.root()[0][0] == t2.root()[0][0], true);

    BOOST_CHECK_EQUAL(t1.root()[0] == t2.root()[1], false);
    BOOST_CHECK_EQUAL(t1.root()[0][0] == t2.root()[0][1], false);    
}

BOOST_AUTO_TEST_CASE(node_op_lessthan) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);
    t2.insert(2);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);

    t2.root().data() = 2;
    t1.root().insert(3);
    t1.root().insert(5);
    t2.root().insert(3);
    t2.root().insert(5);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    t2.root()[1].data() = 7;
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);
}


BOOST_AUTO_TEST_CASE(node_derived_comp_ops) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);
    t2.insert(2);
    BOOST_CHECK_EQUAL(t1.root() != t2.root(), false);
    BOOST_CHECK_EQUAL(t2.root() != t1.root(), false);
    BOOST_CHECK_EQUAL(t1.root() >  t2.root(), false);
    BOOST_CHECK_EQUAL(t2.root() >  t1.root(), false);
    BOOST_CHECK_EQUAL(t1.root() <= t2.root(), true);
    BOOST_CHECK_EQUAL(t2.root() <= t1.root(), true);
    BOOST_CHECK_EQUAL(t1.root() >= t2.root(), true);
    BOOST_CHECK_EQUAL(t2.root() >= t1.root(), true);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1.root() != t2.root(), true);
    BOOST_CHECK_EQUAL(t2.root() != t1.root(), true);
    BOOST_CHECK_EQUAL(t1.root() >  t2.root(), false);
    BOOST_CHECK_EQUAL(t2.root() >  t1.root(), true);
    BOOST_CHECK_EQUAL(t1.root() <= t2.root(), true);
    BOOST_CHECK_EQUAL(t2.root() <= t1.root(), false);
    BOOST_CHECK_EQUAL(t1.root() >= t2.root(), false);
    BOOST_CHECK_EQUAL(t2.root() >= t1.root(), true);
}


BOOST_AUTO_TEST_CASE(tree_op_equality) {
    tree<int> t1;
    tree<int> t2;

    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, false);

    t1.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1 == t2, false);
}


BOOST_AUTO_TEST_CASE(tree_op_lessthan) {
    tree<int> t1;
    tree<int> t2;
    BOOST_CHECK_EQUAL(t1 < t2, false);

    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 < t2, true);
    BOOST_CHECK_EQUAL(t2 < t1, false);

    t1.insert(2);
    BOOST_CHECK_EQUAL(t1 < t2, false);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1 < t2, true);
    BOOST_CHECK_EQUAL(t2 < t1, false);

    BOOST_CHECK_EQUAL(t1 < t1, false);
}


BOOST_AUTO_TEST_CASE(tree_derived_comp_ops) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);
    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 != t2, false);
    BOOST_CHECK_EQUAL(t2 != t1, false);
    BOOST_CHECK_EQUAL(t1 >  t2, false);
    BOOST_CHECK_EQUAL(t2 >  t1, false);
    BOOST_CHECK_EQUAL(t1 <= t2, true);
    BOOST_CHECK_EQUAL(t2 <= t1, true);
    BOOST_CHECK_EQUAL(t1 >= t2, true);
    BOOST_CHECK_EQUAL(t2 >= t1, true);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1 != t2, true);
    BOOST_CHECK_EQUAL(t2 != t1, true);
    BOOST_CHECK_EQUAL(t1 >  t2, false);
    BOOST_CHECK_EQUAL(t2 >  t1, true);
    BOOST_CHECK_EQUAL(t1 <= t2, true);
    BOOST_CHECK_EQUAL(t2 <= t1, false);
    BOOST_CHECK_EQUAL(t1 >= t2, false);
    BOOST_CHECK_EQUAL(t2 >= t1, true);
}


BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_n_rhs_e) {
    tree<int> t1;
    t1.insert(2);
    t1.root().insert(3);

    tree<int> t2;

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), true);
    BOOST_CHECK_EQUAL(t2.empty(), true);

    CHECK_TREE(t1, data(), "");
}

BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_e_rhs_n) {
    tree<int> t1;

    tree<int> t2;
    t2.insert(2);
    t2.root().insert(3);

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t2.empty(), false);

    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
}

BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_n_rhs_n) {
    tree<int> t1;
    t1.insert(31);
    t1.root().insert(41);

    tree<int> t2;
    t2.insert(2);
    t2.root().insert(3);

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t2.empty(), false);

    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
}


BOOST_AUTO_TEST_CASE(tree_swap) {
    tree<int> t1;
    tree<int> t2;

    t1.insert(2);
    t2.insert(3);

    // method version
    t1.swap(t2);

    CHECK_TREE(t1, data(), "3");
    CHECK_TREE(t2, data(), "2");

    // function version
    swap(t1, t2);

    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "3");
}

BOOST_AUTO_TEST_SUITE_END()
