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
    // Declare a tree of integers, which stores node children keyed by a string label.
    // The keyed child storage model has a 'map-like' interface.
    tree<int, keyed<string> > t;
    typedef tree<int, keyed<string> >::iterator iterator;
    typedef tree<int, keyed<string> >::node_type::kv_pair kv_pair;

    // There is no key for root, since root node is always unique
    // The key takes its default value.
    t.insert(0);

    // Insert at ply 1
    t.root().insert("C", 3);
    t.root().insert("A", 1);
    t.root().insert(kv_pair("B", 2)); // standard map<> key/val pair insertion

    // Insert at ply 2
    t.root()["C"].insert("F", 6);
    t.root()["C"].insert("E", 5);
    t.root()["C"].insert("G", 7);
    t.root()["C"].insert("E", 99);   // duplicate insertions are ignored, as with map<>
    t.root()["C"].insert("D", 4);

    // Output data in breadth first order.
    // Child nodes are stored in order of key.
    for (iterator j(t.begin());  j != t.end();  ++j)
        cout << "\"" << j->key() << "\" --> " << j->data() << "\n";

    return 0;
}
