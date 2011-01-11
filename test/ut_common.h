#include <iostream>
#include <sstream>

#include "ootree.h"

using namespace ootree;
using namespace std;


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

