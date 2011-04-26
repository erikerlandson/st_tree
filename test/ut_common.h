#include <iostream>
#include <sstream>

#include "st_tree.h"

using namespace st_tree;
using std::string;
using std::pair;

template <typename Tree>
typename Tree::const_bf_iterator bf_begin(const Tree& t) {
    return t.bf_begin();
}
template <typename Tree>
typename Tree::const_bf_iterator bf_end(const Tree& t) {
    return t.bf_end();
}
template <typename Tree>
typename Tree::const_bf_iterator& bf_j(const Tree& t) {
    static typename Tree::const_bf_iterator j;
    return j;
}

template <typename Tree>
typename Tree::const_df_pre_iterator df_pre_begin(const Tree& t) {
    return t.df_pre_begin();
}
template <typename Tree>
typename Tree::const_df_pre_iterator df_pre_end(const Tree& t) {
    return t.df_pre_end();
}
template <typename Tree>
typename Tree::const_df_pre_iterator& df_pre_j(const Tree& t) {
    static typename Tree::const_df_pre_iterator j;
    return j;
}

template <typename Tree>
typename Tree::const_df_post_iterator df_post_begin(const Tree& t) {
    return t.df_post_begin();
}
template <typename Tree>
typename Tree::const_df_post_iterator df_post_end(const Tree& t) {
    return t.df_post_end();
}
template <typename Tree>
typename Tree::const_df_post_iterator& df_post_j(const Tree& t) {
    static typename Tree::const_df_post_iterator j;
    return j;
}


// These were more elegant with typedef(), but does ansi support typedef?  Nooooo.
#define CHECK_TREE(t, meth, ref) { \
    std::stringstream tst; \
    for (bf_j(t) = bf_begin(t);  bf_j(t) != bf_end(t);  ++bf_j(t)) { \
        if (bf_j(t) != bf_begin(t)) tst << " ";  \
        tst << bf_j(t)->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}


#define CHECK_TREE_DF_POST(t, meth, ref) { \
    std::stringstream tst;                                          \
    for (df_post_j(t) = df_post_begin(t);  df_post_j(t) != df_post_end(t);  ++df_post_j(t)) { \
        if (df_post_j(t) != df_post_begin(t)) tst << " "; \
        tst << df_post_j(t)->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}


#define CHECK_TREE_DF_PRE(t, meth, ref) { \
    std::stringstream tst; \
    for (df_pre_j(t) = df_pre_begin(t);  df_pre_j(t) != df_pre_end(t);  ++df_pre_j(t)) { \
        if (df_pre_j(t) != df_pre_begin(t)) tst << " "; \
        tst << df_pre_j(t)->meth; \
    } \
    BOOST_CHECK_MESSAGE(tst.str() == ref, "Checking " << #meth << ": Expected \"" << ref << "\" received \"" << tst.str() << "\"\n"); \
}

