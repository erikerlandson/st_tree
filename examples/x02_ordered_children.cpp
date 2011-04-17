/******
ootree: An easy to use and highly configurable C++ template tree class, 
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

#include <iostream>
#include <string>
using namespace std;

// use the ootree header and namespace
#include "ootree.h"
using namespace ootree;

int main(int argc, char** argv) {
    // Declare a tree of strings, which stores node children using the ordered storage model.
    // The ordered child storage model ordered<> provides a multiset<> - like interface.
    tree<string, ordered<> > t;
    typedef tree<string, ordered<> >::iterator iterator;
    // Nodes have iterators, and generally model containers for child nodes
    typedef tree<string, ordered<> >::node_type::iterator node_iterator;

    // Insert a string at root (ply 0).
    t.insert("A");

    // Insert strings at ply 1
    node_iterator c = t.root().insert("C");
    t.root().insert("B");
    t.root().insert("D");

    // Insert strings at ply 2
    c->insert("F");
    c->insert("E");
    c->insert("G");
    c->insert("E");   // duplicates are allowed

    // Output data in breadth first order.
    // Observe that child nodes are stored in order
    for (iterator j(t.begin());  j != t.end();  ++j)
        cout << j->data();
    cout << "\n";

    return 0;
}
