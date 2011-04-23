#include <boost/test/unit_test.hpp>

#include "ootree.h"
#include "ut_common.h"


BOOST_AUTO_TEST_SUITE(ut_keyed)


BOOST_AUTO_TEST_CASE(default_ctor) {
    tree<int, keyed<std::string> > t1;
    BOOST_CHECK(t1.empty());
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}

BOOST_AUTO_TEST_CASE(insert_root) {
    tree<int, keyed<std::string> > t1;
    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().is_root(), true);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(insert_subnodes) {
    tree<int, keyed<std::string> > t1;
    typedef tree<int, keyed<std::string> >::node_type::kv_pair kv_pair;

    t1.insert(7);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);

    t1.root().insert(kv_pair("0", 8));
    BOOST_CHECK_EQUAL(t1.size(), 2);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 1);

    t1.root().insert(kv_pair("1", 9));
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);

    BOOST_CHECK_EQUAL(t1.root().data(), 7);
    BOOST_CHECK_EQUAL(t1.root()["0"].data(), 8);
    BOOST_CHECK_EQUAL(t1.root()["1"].data(), 9);
}


BOOST_AUTO_TEST_CASE(clear) {
    tree<int, keyed<std::string> > t1;

    t1.insert(7);
    t1.root().insert("0", 8);
    t1.root().insert("1", 9);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);

    CHECK_TREE(t1, data(), "7 8 9");

    t1.clear();
    BOOST_CHECK_EQUAL(t1.size(), 0);
    BOOST_CHECK_EQUAL(t1.depth(), 0);
    BOOST_CHECK_EQUAL(t1.empty(), true);
    BOOST_CHECK_THROW(t1.root(), ootree::exception);
}


BOOST_AUTO_TEST_CASE(reinsert) {
    tree<int, keyed<std::string> > t1;
 
    t1.insert(7);
    t1.root().insert("0", 8);
    t1.root().insert("1", 9);
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 2);
    CHECK_TREE(t1, data(), "7 8 9");

    t1.insert(3);
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);
    BOOST_CHECK_EQUAL(t1.root().data(), 3);
    CHECK_TREE(t1, data(), "3");
}


BOOST_AUTO_TEST_CASE(erase) {
    tree<int, keyed<std::string> > t1;
 
    t1.insert(7);
    t1.root().insert("0", 8);
    CHECK_TREE(t1, data(), "7 8");

    t1.root().erase(t1.root().begin());
    BOOST_CHECK_EQUAL(t1.size(), 1);
    BOOST_CHECK_EQUAL(t1.depth(), 1);
    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t1.root().size(), 0);
    BOOST_CHECK_EQUAL(t1.root().data(), 7);
}


BOOST_AUTO_TEST_CASE(erase_noarg) {
    tree<int, keyed<std::string> > t1;
 
    t1.insert(2);
    t1.root().insert("0", 3);
    t1.root().insert("1", 5);
    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);

    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    t1.root()["1"].erase();
    CHECK_TREE(t1, data(), "2 3 7 11");

    t1.root()["0"]["1"].erase();
    CHECK_TREE(t1, data(), "2 3 7");

    t1.root().erase();
    BOOST_CHECK_EQUAL(t1.empty(), true);
    CHECK_TREE(t1, data(), "");
}


BOOST_AUTO_TEST_CASE(bf_iterator_empty) {
    tree<int, keyed<std::string> > t1;

    BOOST_CHECK_EQUAL(t1.bf_begin() == t1.bf_end(), true);
    BOOST_CHECK_EQUAL(t1.bf_begin() != t1.bf_end(), false);
}


BOOST_AUTO_TEST_CASE(bf_iterator) {
    tree<int, keyed<std::string> > t1;

    CHECK_TREE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");

    t1.root().insert("0", 3);
    t1.root().insert("1", 5);
    CHECK_TREE(t1, data(), "2 3 5");

    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1.root(), data(), "2 3 5 7 11 13 17");
}


BOOST_AUTO_TEST_CASE(df_post_iterator_empty) {
    tree<int, keyed<std::string> > t1;

    BOOST_CHECK_EQUAL(t1.df_post_begin() == t1.df_post_end(), true);
    BOOST_CHECK_EQUAL(t1.df_post_begin() != t1.df_post_end(), false);
}


BOOST_AUTO_TEST_CASE(df_post_iterator) {
    tree<int, keyed<std::string> > t1;

    CHECK_TREE_DF_POST(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_POST(t1, data(), "2");

    t1.root().insert("0",3);
    t1.root().insert("1",5);
    CHECK_TREE_DF_POST(t1, data(), "3 5 2");

    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);
    CHECK_TREE_DF_POST(t1, data(), "7 11 3 13 17 5 2");
    CHECK_TREE_DF_POST(t1.root(), data(), "7 11 3 13 17 5 2");
}


BOOST_AUTO_TEST_CASE(df_pre_iterator_empty) {
    tree<int, keyed<std::string> > t1;

    BOOST_CHECK_EQUAL(t1.df_pre_begin() == t1.df_pre_end(), true);
    BOOST_CHECK_EQUAL(t1.df_pre_begin() != t1.df_pre_end(), false);
}


BOOST_AUTO_TEST_CASE(df_pre_iterator) {
    tree<int, keyed<std::string> > t1;

    CHECK_TREE_DF_PRE(t1, data(), "");

    t1.insert(2);
    CHECK_TREE_DF_PRE(t1, data(), "2");

    t1.root().insert("0",3);
    t1.root().insert("1",5);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 5");

    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);
    CHECK_TREE_DF_PRE(t1, data(), "2 3 7 11 5 13 17");
    CHECK_TREE_DF_PRE(t1.root(), data(), "2 3 7 11 5 13 17");
}


BOOST_AUTO_TEST_CASE(node_ply) {
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, ply(), "0");

    t1.root().insert("0",3);
    t1.root().insert("1",5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, ply(), "0 1 1");

    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, ply(), "0 1 1 2 2 2 2");

    t1.root().insert("2",77);
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
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, depth(), "1");
    CHECK_TREE(t1, ply(), "0");

    t1.root().insert("0",3);
    t1.root().insert("1",5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, depth(), "2 1 1");
    CHECK_TREE(t1, ply(), "0 1 1");

    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, depth(), "3 2 2 1 1 1 1");
    CHECK_TREE(t1, ply(), "0 1 1 2 2 2 2");

    t1.root().insert("2",77);
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
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t1, subtree_size(), "1");

    t1.root().insert("0",3);
    t1.root().insert("1",5);
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t1, subtree_size(), "3 1 1");

    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);
    CHECK_TREE(t1, data(), "2 3 5 7 11 13 17");
    CHECK_TREE(t1, subtree_size(), "7 3 3 1 1 1 1");

    t1.root().insert("2",77);
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
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    BOOST_CHECK_THROW(t1.root().parent(), ootree::exception);
    t1.root().insert("0",3);
    BOOST_CHECK_EQUAL(t1.root()["0"].parent().data(), 2);
    const tree<int, keyed<std::string> >& t2 = t1;
    BOOST_CHECK_EQUAL(t2.root()["0"].parent().data(), 2);    
}


BOOST_AUTO_TEST_CASE(node_tree) {
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    tree<int, keyed<std::string> >& t2 = t1.root().tree();
    BOOST_CHECK_EQUAL(t1 == t2, true);
    t2 = t1.root()["0"].tree();
    BOOST_CHECK_EQUAL(t1 == t2, true);
    const tree<int, keyed<std::string> >& t3 = t1;
    const tree<int, keyed<std::string> >& t4 = t3.root().tree();
    BOOST_CHECK_EQUAL(t1 == t4, true);
}


BOOST_AUTO_TEST_CASE(erase_node) {
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);
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
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);
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
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);

    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, key(), " 0");

    tree<int, keyed<std::string> > t2;
    t2.insert(5);
    t2.root().insert("0",7);
    t2.root().insert("1",11);

    t1.root()["0"] = t2.root();
    BOOST_CHECK_EQUAL(t1.size(), 4);
    BOOST_CHECK_EQUAL(t1.depth(), 3);

    CHECK_TREE(t1, data(), "2 5 7 11");
    CHECK_TREE(t1, ply(), "0 1 2 2");
    CHECK_TREE(t1, subtree_size(), "4 3 1 1");
    CHECK_TREE(t1, key(), " 0 0 1");

    CHECK_TREE(t2, data(), "5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");
}


BOOST_AUTO_TEST_CASE(node_op_equal_root) {
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);

    tree<int, keyed<std::string> > t2;
    t2.insert(5);
    t2.root().insert("0",7);
    t2.root().insert("1",11);

    t1.root() = t2.root();
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);

    CHECK_TREE(t1, data(), "5 7 11");
    CHECK_TREE(t1, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
    CHECK_TREE(t1, key(), " 0 1");

    CHECK_TREE(t2, data(), "5 7 11");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t1, subtree_size(), "3 1 1");
}


BOOST_AUTO_TEST_CASE(node_op_equal_subtree) {
    tree<int, keyed<std::string> > t1;

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);

    BOOST_CHECK_THROW(t1.root()["0"] = t1.root(), ootree::exception);

    t1.root() = t1.root()["0"];
    CHECK_TREE(t1, data(), "3 7 11");
    CHECK_TREE(t1, key(), " 0 1");
    BOOST_CHECK_EQUAL(t1.size(), 3);
    BOOST_CHECK_EQUAL(t1.depth(), 2);

    t1.root()["1"].insert("0",13);
    t1.root()["1"].insert("1",17);
    t1.root()["1"] = t1.root()["1"]["1"];
    CHECK_TREE(t1, data(), "3 7 17");
    CHECK_TREE(t1, key(), " 0 1");
}

BOOST_AUTO_TEST_CASE(node_swap) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t1.root()["0"].insert("0",7);
    t1.root()["0"].insert("1",11);

    t2.insert(102);
    t2.root().insert("0",103);
    t2.root().insert("1",105);
    t2.root()["0"].insert("0",107);
    t2.root()["0"].insert("1",111);

    // swap a leaf
    swap(t1.root()["0"]["0"], t2.root()["0"]["0"]);
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
    swap(t1.root()["0"]["0"], t2.root()["0"]["0"]);

    // swap an internal
    swap(t1.root()["0"], t2.root()["0"]);
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
    swap(t1.root()["0"], t2.root()["0"]);

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
    swap(t1.root(), t2.root()["0"]);
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
    swap(t1.root(), t2.root()["0"]);

    // on same tree
    swap(t1.root()["0"], t1.root()["1"]);
    BOOST_CHECK_EQUAL(t1.size(), 5);
    BOOST_CHECK_EQUAL(t1.depth(), 3);
    CHECK_TREE(t1, data(), "2 5 3 7 11");
    CHECK_TREE(t1, ply(), "0 1 1 2 2");
    CHECK_TREE(t1, depth(), "3 1 2 1 1");
    CHECK_TREE(t1, subtree_size(), "5 1 3 1 1");

    // put them back
    swap(t1.root()["0"], t1.root()["1"]);

    // no-no
    BOOST_CHECK_THROW(swap(t1.root(), t1.root()["0"]), ootree::exception);
    BOOST_CHECK_THROW(swap(t1.root()["1"], t1.root()), ootree::exception);
}

BOOST_AUTO_TEST_CASE(graft) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.graft(t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2 3 5");
    CHECK_TREE(t2, depth(), "2 1 1");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.graft(t1.root()["1"]);
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
    t2.root().graft("0",t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    // graft tree -> node (non-empty)
    t1.insert(2);
    t2.insert(1);
    t2.root().graft("0",t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2");

    // node --> node
    t1.insert(2);
    t2.insert(1);
    t2.root().graft("0",t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(1);
    t2.root().graft("0",t1.root());
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "1 2 3 5");
    CHECK_TREE(t2, depth(), "3 2 1 1");
    CHECK_TREE(t2, ply(), "0 1 2 2");
    CHECK_TREE(t2, subtree_size(), "4 3 1 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(1);
    t2.root().graft("0",t1.root()["1"]);
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
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(t1.root());
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t2, data(), "2 3 5");
    CHECK_TREE(t2, depth(), "2 1 1");
    CHECK_TREE(t2, ply(), "0 1 1");
    CHECK_TREE(t2, subtree_size(), "3 1 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(t1.root()["1"]);
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
    t2.root().insert("0",t1);
    CHECK_TREE(t1, data(), "");
    CHECK_TREE(t2, data(), "2");

    // insert tree -> node (non-empty)
    t1.insert(2);
    t2.insert(1);
    t2.root().insert("0",t1);
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "1 2");

    // node --> node
    t1.insert(2);
    t2.insert(1);
    t2.root().insert("0",t1.root());
    CHECK_TREE(t1, data(), "2");
    CHECK_TREE(t2, data(), "1 2");
    CHECK_TREE(t2, depth(), "2 1");
    CHECK_TREE(t2, ply(), "0 1");
    CHECK_TREE(t2, subtree_size(), "2 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(1);
    t2.root().insert("0",t1.root());
    CHECK_TREE(t1, data(), "2 3 5");
    CHECK_TREE(t2, data(), "1 2 3 5");
    CHECK_TREE(t2, depth(), "3 2 1 1");
    CHECK_TREE(t2, ply(), "0 1 2 2");
    CHECK_TREE(t2, subtree_size(), "4 3 1 1");

    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.insert(1);
    t2.root().insert("0",t1.root()["1"]);
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
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t1.insert(2);
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t1.root()["0"].insert("0",7);
    t1.root()["1"].insert("0",13);
    t1.root()["0"].insert("1",11);
    t1.root()["1"].insert("1",17);

    t2.insert(2);
    t2.root().insert("0",3);
    t2.root().insert("1",5);
    t2.root()["0"].insert("0",7);
    t2.root()["1"].insert("0",13);
    t2.root()["0"].insert("1",11);
    t2.root()["1"].insert("1",17);

    BOOST_CHECK_EQUAL(t1.root() == t1.root(), true);
    BOOST_CHECK_EQUAL(t1.root()["0"] == t1.root()["0"], true);
    BOOST_CHECK_EQUAL(t1.root()["0"]["0"] == t1.root()["0"]["0"], true);

    BOOST_CHECK_EQUAL(t1.root() == t2.root(), true);
    BOOST_CHECK_EQUAL(t1.root()["0"] == t2.root()["0"], true);
    BOOST_CHECK_EQUAL(t1.root()["0"]["0"] == t2.root()["0"]["0"], true);

    BOOST_CHECK_EQUAL(t1.root()["0"] == t2.root()["1"], false);
    BOOST_CHECK_EQUAL(t1.root()["0"]["0"] == t2.root()["0"]["1"], false);    
}


BOOST_AUTO_TEST_CASE(node_op_lessthan) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t1.insert(2);
    t2.insert(2);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);

    t2.root().data() = 2;
    t1.root().insert("0",3);
    t1.root().insert("1",5);
    t2.root().insert("0",3);
    t2.root().insert("1",5);

    BOOST_CHECK_EQUAL(t1.root() < t2.root(), false);

    t2.root()["1"].data() = 7;
    BOOST_CHECK_EQUAL(t1.root() < t2.root(), true);
}


BOOST_AUTO_TEST_CASE(node_derived_comp_ops) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, false);

    t1.insert(2);
    BOOST_CHECK_EQUAL(t1 == t2, true);

    t2.root().data() = 3;
    BOOST_CHECK_EQUAL(t1 == t2, false);
}


BOOST_AUTO_TEST_CASE(tree_op_lessthan) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t1.insert(2);
    t1.root().insert("0",3);

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), true);
    BOOST_CHECK_EQUAL(t2.empty(), true);

    CHECK_TREE(t1, data(), "");
}

BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_e_rhs_n) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t2.insert(2);
    t2.root().insert("0",3);

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t2.empty(), false);

    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
}

BOOST_AUTO_TEST_CASE(tree_op_equal_lhs_n_rhs_n) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
    t1.insert(31);
    t1.root().insert("0",41);

    t2.insert(2);
    t2.root().insert("0",3);

    t1 = t2;

    BOOST_CHECK_EQUAL(t1.empty(), false);
    BOOST_CHECK_EQUAL(t2.empty(), false);

    CHECK_TREE(t1, data(), "2 3");
    CHECK_TREE(t1, ply(), "0 1");
    CHECK_TREE(t1, subtree_size(), "2 1");
}


BOOST_AUTO_TEST_CASE(tree_swap) {
    tree<int, keyed<std::string> > t1;
    tree<int, keyed<std::string> > t2;
    
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

BOOST_AUTO_TEST_CASE(count) {
    tree<int, keyed<std::string> > t1;
    t1.insert(2);
    t1.root().insert("5",5);
    t1.root().insert("3",3);
    t1.root().insert("5",5);
    CHECK_TREE(t1, data(), "2 3 5");
    BOOST_CHECK_EQUAL(t1.root().count("3"), (unsigned)1);
    BOOST_CHECK_EQUAL(t1.root().count("5"), (unsigned)1);
    BOOST_CHECK_EQUAL(t1.root().count("7"), (unsigned)0);
}

BOOST_AUTO_TEST_CASE(find) {
    tree<int, keyed<std::string> > t1;
    typedef tree<int, keyed<std::string> >::node_type::iterator iterator;
    typedef tree<int, keyed<std::string> >::node_type::const_iterator const_iterator;
    t1.insert(2);
    t1.root().insert("5",5);
    t1.root().insert("3",3);
    t1.root().insert("5",5);
    CHECK_TREE(t1, data(), "2 3 5");

    iterator j1 = t1.root().find("3");
    BOOST_CHECK(j1 != t1.root().end()  &&  j1->data() == 3);
    j1 = t1.root().find("5");
    BOOST_CHECK(j1 != t1.root().end()  &&  j1->data() == 5);
    j1 = t1.root().find("7");
    BOOST_CHECK(j1 == t1.root().end());

    tree<int, keyed<std::string> > const& tc = t1;
    const_iterator jc = tc.root().find("3");
    BOOST_CHECK(jc != tc.root().end()  &&  jc->data() == 3);
    jc = tc.root().find("5");
    BOOST_CHECK(jc != tc.root().end()  &&  jc->data() == 5);
    jc = tc.root().find("7");
    BOOST_CHECK(jc == tc.root().end());
}

BOOST_AUTO_TEST_CASE(lower_upper_equal) {
    tree<int, keyed<std::string> > t1;
    typedef tree<int, keyed<std::string> >::node_type::iterator iterator;
    typedef tree<int, keyed<std::string> >::node_type::const_iterator const_iterator;

    t1.insert(2);
    t1.root().insert("5", 5);
    t1.root().insert("3", 3);
    t1.root().insert("7", 7);

    CHECK_TREE(t1, data(), "2 3 5 7");

    iterator l1 = t1.root().lower_bound("5");
    iterator u1 = t1.root().upper_bound("5");
    pair<iterator, iterator> r1 = t1.root().equal_range("5");
    BOOST_CHECK_EQUAL(l1->data(), 5);
    BOOST_CHECK_EQUAL(u1->data(), 7);
    BOOST_CHECK(r1.first == l1);
    BOOST_CHECK(r1.second == u1);
    ++l1;
    BOOST_CHECK_EQUAL(l1->data(), 7);

    const tree<int, keyed<std::string> >& tc = t1;
    const_iterator lc = tc.root().lower_bound("5");
    const_iterator uc = tc.root().upper_bound("5");
    pair<const_iterator, const_iterator> rc = tc.root().equal_range("5");
    BOOST_CHECK_EQUAL(lc->data(), 5);
    BOOST_CHECK_EQUAL(uc->data(), 7);
    BOOST_CHECK(rc.first == lc);
    BOOST_CHECK(rc.second == uc);
    ++lc;
    BOOST_CHECK_EQUAL(lc->data(), 7);    
}


BOOST_AUTO_TEST_SUITE_END()
