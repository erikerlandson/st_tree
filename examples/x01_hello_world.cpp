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
using namespace std;

// use the ootree header and namespace
#include "ootree.h"
using namespace ootree;

int main(int argc, char** argv) {
    // Declare a tree of strings.
    // Default child node storage model is raw<>, which provides a vector<> - like interface.
    tree<const char*> t;
    // The basic iterator traverses nodes of the tree in breadth-first order.
    typedef tree<const char*>::iterator iterator;

    // Insert a string at root (ply 0).
    t.insert("Hello");

    // Insert strings at ply 1
    // For raw child storage model, push_back() and insert() are equivalent
    t.root().push_back(" ");
    t.root().push_back("world");

    // Insert strings at ply 2
    t.root()[0].push_back("!");
    t.root()[1].push_back("\n");

    // Output data in breadth first order to print a traditional message
    for (iterator j(t.begin());  j != t.end();  ++j)
        cout << j->data();

    return 0;
}
