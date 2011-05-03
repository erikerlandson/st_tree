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
    // construct a basic tree with two plys
    tree<string> t;
    t.insert("A");
    t.root().insert("B");  // children of "A"
    t.root().insert("C");
    t.root().insert("D");

    // another tree
    tree<string> t2;
    t2.insert("T");
    t2.root().insert("U");     // children of "T"
    t2.root().insert("V");
    t2.root()[0].insert("W");  // children of "U"
    t2.root()[0].insert("X");
    t2.root()[1].insert("Y");  // children of "V"
    t2.root()[1].insert("Z");

    // Demonstrate the differences between node assignment, insert and graft:
    // Assignment replaces destination with deep copy of source (and subtree)
    t.root()[0] = t2.root()[1];  // node "B"  <--  "V","Y","Z" subtree

    // insert() inserts a deep copy of source into node's children
    t.root()[1].insert(t2.root()[1]);  // node "C" gets child subtree "V","Y","Z"

    // graft() *removes* source and inserts to destination's children
    t.root()[2].graft(t2.root()[1]);   // subtree "V","Y","Z" is moved from t2 to child of "D" on (t)
                                       // tree (t2) will no longer have subtree "V","Y","Z"

    // Display tree (t) with changes
    cout << "Tree (t)\n";
    serialize_indented(t, cout, 2);

    // Observe that (t2) subtree "V","Y","Z" is gone after graft()
    cout << "\nTree (t2)\n";
    serialize_indented(t2, cout, 2);

    return 0;
}
