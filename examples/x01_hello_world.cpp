/******
ootree: An easy to use and highly configurable C++ template tree class, 
using STL container style interfaces.

Copyright (c) 2010 Erik Erlandson

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
    map<int, int> test;
    pair<map<int,int>::iterator, bool> r;
    r = test.insert(map<int,int>::value_type(1,2));
    cerr << "b= " << r.second << "  v= " << test[1] << "\n";

    r = test.insert(map<int,int>::value_type(1,2));
    cerr << "b= " << r.second << "  v= " << test[1] << "\n";

    r = test.insert(map<int,int>::value_type(1,3));
    cerr << "b= " << r.second << "  v= " << test[1] << "\n";
    return 0;

    // declare a tree of strings 
    tree<const char*> t;
    typedef tree<const char*>::bf_iterator bf_iterator;

    // insert a string at root (ply 0)
    t.insert("Hello");

    // insert strings at ply 1
    t.root().insert(" ");
    t.root().insert("world");

    // insert strings at ply 2
    t.root()[0].insert("!");
    t.root()[1].insert("\n");

    // output data in breadth first order to print a traditional message
    for (bf_iterator j(t.bf_begin());  j != t.bf_end();  ++j)
        cout << j->data();

    return 0;
}
