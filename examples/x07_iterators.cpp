/******
st_tree: An easy to use and highly configurable C++ template tree class, 
using STL container style interfaces.

Copyright (c) 2010-2011 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
******/

#include <string>
#include <iostream>
using std::cout;
using std::string;

// use the st_tree header and namespace
#include "st_tree.h"
using namespace st_tree;

#include "x_common.h"

int main(int argc, char** argv) {
    // construct a basic tree
    tree<string> t;
    t.insert("T");
    t.root().insert("U");     // children of "T"
    t.root().insert("V");
    t.root()[0].insert("W");  // children of "U"
    t.root()[0].insert("X");
    t.root()[1].insert("Y");  // children of "V"
    t.root()[1].insert("Z");


    // The basic tree<>::iterator is breadth first traversal
    cout << "tree<>::iterator\n";
    serialize_indented(t.begin(), t.end(), cout);

    // tree<>::bf_iterator is also breadth-first
    cout << "\ntree<>::bf_iterator\n";
    serialize_indented(t.bf_begin(), t.bf_end(), cout);

    // tree<>::df_pre_iterator is depth-first, pre order traversal
    cout << "\ntree<>::df_pre_iterator\n";
    serialize_indented(t.df_pre_begin(), t.df_pre_end(), cout);

    // tree<>::df_post_iterator id depth-first post order traversal
    cout << "\ntree<>::df_post_iterator\n";
    serialize_indented(t.df_post_begin(), t.df_post_end(), cout);


    // the basic tree<>::node_type::iterator is iterator over that node's children
    // so, this only iterates over children of "U": "W" and "X"
    cout << "\ntree<>::node_type::iterator\n";
    serialize_indented(t.root()[0].begin(), t.root()[0].end(), cout, 0);

    // tree<>::node_type::bf_iterator traverses a node's subtree, breadth first
    cout << "\ntree<>::node_type::bf_iterator\n";
    serialize_indented(t.root()[0].bf_begin(), t.root()[0].bf_end(), cout);

    // tree<>::node_type::df_pre_iterator traverses a node's subtree, depth first pre-order
    cout << "\ntree<>::node_type::df_pre_iterator\n";
    serialize_indented(t.root()[0].df_pre_begin(), t.root()[0].df_pre_end(), cout);

    // tree<>::node_type::df_post_iterator traverses a node's subtree, depth first post-order
    cout << "\ntree<>::node_type::df_post_iterator\n";
    serialize_indented(t.root()[0].df_post_begin(), t.root()[0].df_post_end(), cout);

    return 0;
}
