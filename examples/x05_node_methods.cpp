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

const char* indent(unsigned n) {
    static char const spaces[] = "                                             ";
    static const unsigned ns = sizeof(spaces)/sizeof(*spaces);
    return spaces + (ns-1-n);
}

int main(int argc, char** argv) {
    // Declare a tree of strings.
    tree<string> t;
    // An iterator to traverse nodes in depth first pre-order
    typedef tree<string>::df_pre_iterator iterator;
    typedef tree<string>::node_type node_type;

    // Insert a string at root (ply 0).
    t.insert("Animals");

    // Insert strings at ply 1
    // For raw child storage model, push_back() and insert() are equivalent
    t.root().push_back("Canines");
    t.root().push_back("Felines");
    t.root().push_back("Equines");

    // Insert strings at ply 2
    t.root()[0].push_back("Dog");
    t.root()[0].push_back("Wolf");
    t.root()[0].push_back("Fox");

    t.root()[1].push_back("Cat");
    t.root()[1].push_back("Lion");
    t.root()[1].push_back("Panther");

    t.root()[2].push_back("Horse");
    t.root()[2].push_back("Zebra");


    // Demonstrate some of the basic tree-related node methods.
    // Output the tree nodes in depth first pre-order.
    for (iterator j(t.df_pre_begin());  j != t.df_pre_end();  ++j) {
        // data() gives the data 'payload' of the node
        string data = j->data();

        // ply() gives the ply, or layer, of the node
        unsigned ply = j->ply();

        // padding for nice output
        unsigned w = 16;
        unsigned padL = ply * 4;
        unsigned padR = w - data.size() - padL;
        cout << indent(padL);
        cout << data;
        cout << indent(padR);

        // is_root() is true if the node has no parent
        // parent() returns the node's parent
        cout << "   parent= " << ((j->is_root()) ? "       " : j->parent().data());

        // ply() gives the ply, or layer, of the node
        cout << "   ply= " << j->ply();

        // depth() is the depth of the node's subtree
        cout << "   depth= " << j->depth();

        // subtree_size() is the size of the node's subtree
        cout << "   subtree_size= " << j->subtree_size();
        cout << "\n";
    }

    return 0;
}
