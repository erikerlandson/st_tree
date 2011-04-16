#include <boost/test/unit_test.hpp>

#include "ootree.h"
#include "ut_common.h"


BOOST_AUTO_TEST_SUITE(ut_ordered)


BOOST_AUTO_TEST_CASE(default_ctor) {
    tree<int, ordered<> > t1;
    BOOST_CHECK(t1.empty());
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}

BOOST_AUTO_TEST_CASE(insert_root) {
    tree<int, ordered<> > t1;
    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().is_root(), true);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(insert_subnodes) {
    tree<int, ordered<> > t1;

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

    CHECK_TREE(t1, data(), "7 8 9");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
}


BOOST_AUTO_TEST_CASE(clear) {
    tree<int, ordered<> > t1;

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
    tree<int, ordered<> > t1;

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
    tree<int, ordered<> > t1;

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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);

    n3.insert(7);
    node_type& n11 = *n3.insert(11);
    n5.insert(13);
    n5.insert(17);
    
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");

    n5.erase();
    CHECK_TREE(t1, data(), "2 3 7 11");

    n11.erase();
    CHECK_TREE(t1, data(), "2 3 7");

    t1.root().erase();
    BOOST_CHECK_EQUAL(t1.empty(), true);
    CHECK_TREE(t1, data(), "");
}




BOOST_AUTO_TEST_CASE(bf_iterator_empty) {
    tree<int, ordered<> > t1;
    BOOST_CHECK_EQUAL(t1.bf_begin() == t1.bf_end(), true);
    BOOST_CHECK_EQUAL(t1.bf_begin() != t1.bf_end(), false);
}


BOOST_AUTO_TEST_CASE(bf_iterator) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;
    CHECK_TREE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");

    n3.insert(7);
    n5.insert(13);
    n3.insert(11);
    n5.insert(17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1.root(), data(), "2 3 5 7 11 13 17");
}


BOOST_AUTO_TEST_CASE(df_post_iterator_empty) {
    tree<int, ordered<> > t1;
    BOOST_CHECK_EQUAL(t1.df_post_begin() == t1.df_post_end(), true);
    BOOST_CHECK_EQUAL(t1.df_post_begin() != t1.df_post_end(), false);
}


BOOST_AUTO_TEST_CASE(df_post_iterator) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    CHECK_TREE_DF_POST(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_POST(t1, data(), "2");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE_DF_POST(t1, data(), "3 5 2");

    n3.insert(7);
    n5.insert(13);
    n3.insert(11);
    n5.insert(17);
    CHECK_TREE_DF_POST(t1, data(), "7 11 3 13 17 5 2");
    CHECK_TREE_DF_POST(t1.root(), data(), "7 11 3 13 17 5 2");
}


BOOST_AUTO_TEST_CASE(df_pre_iterator_empty) {
    tree<int, ordered<> > t1;
    BOOST_CHECK_EQUAL(t1.df_pre_begin() == t1.df_pre_end(), true);
    BOOST_CHECK_EQUAL(t1.df_pre_begin() != t1.df_pre_end(), false);
}


BOOST_AUTO_TEST_CASE(df_pre_iterator) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    CHECK_TREE_DF_PRE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_PRE(t1, data(), "2");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 5");

    n3.insert(7);
    n5.insert(13);
    n3.insert(11);
    n5.insert(17);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 7 11 5 13 17");
    CHECK_TREE_DF_PRE(t1.root(), data(), "2 3 7 11 5 13 17");
}


BOOST_AUTO_TEST_CASE(node_ply) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, ply(), "0 1 1");

    n3.insert(7);
    n3.insert(11);
    n5.insert(13);
    n5.insert(17);
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, depth(), "1");
    CHECK_TREE(t1, ply(), "0");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, ply(), "0 1 1");

    n3.insert(7);
    n3.insert(11);
    n5.insert(13);
    n5.insert(17);
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, subtree_size(), "1");

    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, subtree_size(), "3 1 1");

    n3.insert(7);
    n3.insert(11);
    n5.insert(13);
    n5.insert(17);
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


BOOST_AUTO_TEST_CASE(node_parent) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);

    node_type& n3 = *t1.root().insert(3);
    BOOST_CHECK_EQUAL(n3.parent().data(), 2);

    const node_type& n3c = n3;
    BOOST_CHECK_EQUAL(n3c.parent().data(), 2);    
}


BOOST_AUTO_TEST_CASE(node_tree) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);

    tree<int, ordered<> >& t2 = t1.root().tree();
    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2 = n3.tree();
    BOOST_CHECK_EQUAL(t1 == t2, true);

    const node_type& n5c = n5;
    const tree<int, ordered<> >& t3 = t1;
    const tree<int, ordered<> >& t4 = n5c.tree();
    BOOST_CHECK_EQUAL(t3 == t4, true);
    BOOST_CHECK_EQUAL(t1 == t4, true);
}



BOOST_AUTO_TEST_CASE(erase_node) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    n3.insert(7);
    n5.insert(13);
    n3.insert(11);
    n5.insert(17);
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    n3.insert(7);
    n5.insert(13);
    n3.insert(11);
    n5.insert(17);

    BOOST_CHECK_EQUAL(t1.size(), 7);
    BOOST_CHECK_EQUAL(t1.depth(), 3);

    t1.root().clear();
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);

    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");
    CHECK_TREE(t1, subtree_size(), "1");
}


BOOST_AUTO_TEST_CASE(node_assign_op) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);

    tree<int, ordered<> > t2;
    t2.insert(5);
    node_type& n7 = *t2.root().insert(7);
    node_type& n11 = *t2.root().insert(11);

    BOOST_CHECK_EQUAL(t1.root().data(), 2);

    n3 = t2.root();
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;
    t1.insert(2);
    t1.root().insert(3);

    tree<int, ordered<> > t2;
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    n3.insert(7);
    node_type& n11 = *n3.insert(11);
    n5.insert(13);
    n5.insert(17);

    BOOST_CHECK_THROW(n3 = t1.root(), ootree::exception);

    t1.root() = n3;
    CHECK_TREE(t1, data(), "3 7 11");
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);

    typeof(t1.root().begin()) j11(t1.root().begin());
    ++j11;
    j11->insert(13);
    node_type& n17 = *(j11->insert(17));
    *j11 = n17;
    CHECK_TREE(t1, data(), "3 7 17");
}


BOOST_AUTO_TEST_CASE(node_swap) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

    BOOST_TEST_CHECKPOINT("here 1");
    t1.insert(2);
    node_type& n2 = t1.root();
    node_type& n3 = *t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    node_type& n7 = *n3.insert(7);
    n3.insert(11);

    BOOST_TEST_CHECKPOINT("here 2");
    t2.insert(102);
    node_type& n102 = t2.root();
    node_type& n103 = *t2.root().insert(103);
    node_type& n105 = *t2.root().insert(105);
    node_type& n107 = *n103.insert(107);
    n103.insert(111);

    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

    // swap a leaf
    BOOST_TEST_CHECKPOINT("here 3");
    swap(n7, n107);

    BOOST_TEST_CHECKPOINT("here 4");
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 3 5 11 107");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");

    BOOST_TEST_CHECKPOINT("here 5");
    BOOST_CHECK_EQUAL(t2.size(), 5);
    BOOST_CHECK_EQUAL(t2.depth(), 3);
    CHECK_TREE(t2, data(), "102 103 105 7 111");
    CHECK_TREE(t2, ply(), "0 1 1 2 2");
    CHECK_TREE(t2, depth(), "3 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "5 3 1 1 1");

    BOOST_TEST_CHECKPOINT("here 6");

    // put it back
    swap(n7, n107);
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

    // swap an internal
    swap(n3, n103);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 5 103 107 111");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 1 2 1 1");
    CHECK_TREE(t1, subtree_size(), "5 1 3 1 1");
    BOOST_CHECK_EQUAL(t2.size(), 5);
    BOOST_CHECK_EQUAL(t2.depth(), 3);
    CHECK_TREE(t2, data(), "102 3 105 7 11");
    CHECK_TREE(t2, ply(), "0 1 1 2 2");
    CHECK_TREE(t2, depth(), "3 2 1 1 1");
    CHECK_TREE(t2, subtree_size(), "5 3 1 1 1");

    // put it back
    swap(n3, n103);
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

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
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

    // swap different plies
    swap(n2, n103);
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
    swap(n2, n103);
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

    // on same tree
    swap(n3, n5);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 2 1 1 1");
    CHECK_TREE(t1, subtree_size(), "5 3 1 1 1");

    // put them back
    swap(n3, n5);
    CHECK_TREE(t1, data(), "2 3 5 7 11");
    CHECK_TREE(t1, depth(), "3 2 1 1 1")
    CHECK_TREE(t2, data(), "102 103 105 107 111");
    CHECK_TREE(t2, depth(), "3 2 1 1 1")

    // no-no
    BOOST_CHECK_THROW(swap(n2, n3), ootree::exception);
    BOOST_CHECK_THROW(swap(n3, n2), ootree::exception);
}


BOOST_AUTO_TEST_CASE(graft) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

    // start with the node/tree overloadings
    // graft node -> tree
    t1.insert(2);
    t2.insert(1);
    t2.graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");
    CHECK_TREE(t2, depth(), "1");
    CHECK_TREE(t2, ply(), "0");
    CHECK_TREE(t2, subtree_size(), "1");

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t2.graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2 3 5");
    CHECK_TREE(t2, depth(), "2 1 1");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");

    t1.insert(2);
    t1.root().insert(3);
    node_type& n5 = *t1.root().insert(5);
    t2.graft(n5);
    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, depth(), "2 1");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
    CHECK_TREE(t2, data(), "5");
    CHECK_TREE(t2, depth(), "1");
    CHECK_TREE(t2, ply(), "0");
    CHECK_TREE(t2, subtree_size(), "1");

    // graft tree -> tree (empty)
    t1.clear();
    t2.graft(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "");

    // graft tree -> tree (non-empty)
    t1.insert(2);
    t2.graft(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    // graft tree -> node (empty)
    t2.root().graft(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    // graft tree -> node (non-empty)
    t1.insert(2);
    t2.insert(1);
    t2.root().graft(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2");

    // node --> node
    t1.insert(2);
    t2.insert(1);
    t2.root().graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t2.insert(1);
    t2.root().graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2 3 5");
    CHECK_TREE(t2, depth(), "3 2 1 1");
    CHECK_TREE(t2, ply(), "0 1 2 2");
    CHECK_TREE(t2, subtree_size(), "4 3 1 1");

    t1.insert(2);
    t1.root().insert(3);
    node_type& n5b = *t1.root().insert(5);
    t2.insert(1);
    t2.root().graft(n5b);
    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, depth(), "2 1");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
    CHECK_TREE(t2, data(), "1 5");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");
}


BOOST_AUTO_TEST_CASE(insert_node) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

    // start with the node/tree overloadings
    // insert node -> tree
    t1.insert(2);
    t2.insert(1);
    t2.insert(t1.root());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "2");
    CHECK_TREE(t2, depth(), "1");
    CHECK_TREE(t2, ply(), "0");
    CHECK_TREE(t2, subtree_size(), "1");

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t2.insert(t1.root());
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t2, data(), "2 3 5");
    CHECK_TREE(t2, depth(), "2 1 1");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");

    t1.insert(2);
    t1.root().insert(3);
    node_type& n5a = *t1.root().insert(5);
    t2.insert(n5a);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
    CHECK_TREE(t2, data(), "5");
    CHECK_TREE(t2, depth(), "1");
    CHECK_TREE(t2, ply(), "0");
    CHECK_TREE(t2, subtree_size(), "1");

    // insert tree -> tree (empty)
    t1.clear();
    t2.insert(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "");

    // insert tree -> tree (non-empty)
    t1.insert(2);
    t2.insert(t1);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "2");

    // insert tree -> node (empty)
    t1.clear();
    t2.root().insert(t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    // insert tree -> node (non-empty)
    t1.insert(2);
    t2.insert(1);
    t2.root().insert(t1);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "1 2");

    // node --> node
    t1.insert(2);
    t2.insert(1);
    t2.root().insert(t1.root());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "1 2");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");

    t1.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t2.insert(1);
    t2.root().insert(t1.root());
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t2, data(), "1 2 3 5");
    CHECK_TREE(t2, depth(), "3 2 1 1");
    CHECK_TREE(t2, ply(), "0 1 2 2");
    CHECK_TREE(t2, subtree_size(), "4 3 1 1");

    t1.insert(2);
    t1.root().insert(3);
    node_type& n5b = *t1.root().insert(5);
    t2.insert(1);
    t2.root().insert(n5b);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
    CHECK_TREE(t2, data(), "1 5");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");
}


BOOST_AUTO_TEST_CASE(node_op_equality) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n3a = *t1.root().insert(3);
    node_type& n5a = *t1.root().insert(5);
    node_type& n7a = *n3a.insert(7);
    n5a.insert(13);
    n3a.insert(11);
    n5a.insert(17);

    tree<int, ordered<> > t2;

    t2.insert(2);
    node_type& n3b = *t2.root().insert(3);
    node_type& n5b = *t2.root().insert(5);
    node_type& n7b = *n3b.insert(7);
    n5b.insert(13);
    node_type& n11b = *n3b.insert(11);
    n5b.insert(17);

    BOOST_CHECK_EQUAL(t1.root() == t1.root(), true);
    BOOST_CHECK_EQUAL(n3a == n3a, true);
    BOOST_CHECK_EQUAL(n7a == n7a, true);

    BOOST_CHECK_EQUAL(t1.root() == t2.root(), true);
    BOOST_CHECK_EQUAL(n3a == n3b, true);
    BOOST_CHECK_EQUAL(n7a == n7b, true);

    BOOST_CHECK_EQUAL(n3a == n5b, false);
    BOOST_CHECK_EQUAL(n7a == n11b, false);    
}


BOOST_AUTO_TEST_CASE(node_op_lessthan) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    t2.insert(2);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    t2.clear();
    t2.insert(3);
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);

    t2.clear();
    t2.insert(2);
    t1.root().insert(3);
    t1.root().insert(5);
    t2.root().insert(3);
    node_type& n5b = *t2.root().insert(5);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    n5b.erase();
    t2.root().insert(7);
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);
}


BOOST_AUTO_TEST_CASE(node_derived_comp_ops) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

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

    t2.erase();
    t2.insert(3);
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
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, false);

    t1.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.clear();
    t2.insert(3);
    BOOST_CHECK_EQUAL(t1 == t2, false);
}


BOOST_AUTO_TEST_CASE(tree_op_lessthan) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;
    BOOST_CHECK_EQUAL(t1 < t2, false);

    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 < t2, true);
    BOOST_CHECK_EQUAL(t2 < t1, false);

    t1.insert(2);
    BOOST_CHECK_EQUAL(t1 < t2, false);

    t2.clear();
    t2.insert(3);
    BOOST_CHECK_EQUAL(t1 < t2, true);
    BOOST_CHECK_EQUAL(t2 < t1, false);

    BOOST_CHECK_EQUAL(t1 < t1, false);
}


BOOST_AUTO_TEST_CASE(tree_derived_comp_ops) {
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

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

    t2.clear();
    t2.insert(3);
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;
    t1.insert(2);
    t1.root().insert(3);

    tree<int, ordered<> > t2;

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), true);
    BOOST_CHECK_EQUAL(t2.empty(), true);

    CHECK_TREE(t1, data(), "");
}

BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_e_rhs_n) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    tree<int, ordered<> > t2;
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
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;
    t1.insert(31);
    t1.root().insert(41);

    tree<int, ordered<> > t2;
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
    tree<int, ordered<> > t1;
    tree<int, ordered<> > t2;
    typedef tree<int, ordered<> >::node_type node_type;

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


BOOST_AUTO_TEST_CASE(ordering_behavior) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type node_type;

    t1.insert(2);
    node_type& n5 = *t1.root().insert(5);
    t1.root().insert(3);
    t1.root().insert(7);
    n5.insert(17);
    n5.insert(13);
    n5.insert(11);

    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, ply(), "0 1 1 1 2 2 2");
    CHECK_TREE(t1, depth(), "3 1 2 1 1 1 1");
    CHECK_TREE(t1, subtree_size(), "7 1 4 1 1 1 1")
}

BOOST_AUTO_TEST_CASE(count) {
    tree<int, ordered<> > t1;
    t1.insert(2);
    t1.root().insert(5);
    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5 5");
    BOOST_CHECK_EQUAL(t1.root().count(3), (unsigned)1);
    BOOST_CHECK_EQUAL(t1.root().count(5), (unsigned)2);
    BOOST_CHECK_EQUAL(t1.root().count(7), (unsigned)0);
}

BOOST_AUTO_TEST_CASE(find) {
    tree<int, ordered<> > t1;
    typedef tree<int, ordered<> >::node_type::iterator iterator;
    typedef tree<int, ordered<> >::node_type::const_iterator const_iterator;
    t1.insert(2);
    t1.root().insert(5);
    t1.root().insert(3);
    t1.root().insert(5);
    CHECK_TREE(t1, data(), "2 3 5 5");

    iterator j1 = t1.root().find(3);
    BOOST_CHECK(j1 != t1.root().end()  &&  j1->data() == 3);
    j1 = t1.root().find(5);
    BOOST_CHECK(j1 != t1.root().end()  &&  j1->data() == 5);
    j1 = t1.root().find(7);
    BOOST_CHECK(j1 == t1.root().end());

    tree<int, ordered<> > const& tc = t1;
    const_iterator jc = tc.root().find(3);
    BOOST_CHECK(jc != tc.root().end()  &&  jc->data() == 3);
    jc = tc.root().find(5);
    BOOST_CHECK(jc != tc.root().end()  &&  jc->data() == 5);
    jc = tc.root().find(7);
    BOOST_CHECK(jc == tc.root().end());
}

BOOST_AUTO_TEST_SUITE_END()
