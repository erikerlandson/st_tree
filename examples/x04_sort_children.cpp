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

#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

// use the ootree header and namespace
#include "ootree.h"
using namespace ootree;


struct data_lt {
    template <typename N>
    bool operator()(const N& a, const N& b) const {
        return a.data() < b.data();
    }
};

template <typename X, typename Y>
inline void test(X a, X b, Y c) {
        //__glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<X>)
    for (X j=a;  j < b;  ++j) cout << j->data() << "\n";
}

int main(int argc, char** argv) {
    // Declare a tree of strings.
    tree<string> t;
    // The basic iterator traverses nodes of the tree in breadth-first order.
    typedef tree<string>::iterator iterator;
    typedef tree<string>::node_type node_type;
    typedef node_type::iterator child_iterator;

    // Insert a string at root (ply 0).
    t.insert("A");

    // Insert strings at ply 1
    // For raw child storage model, push_back() and insert() are equivalent
    t.root().push_back("D");
    t.root().push_back("C");
    t.root().push_back("B");

    // Insert strings at ply 2
    // You can use front() and back() with this storage model
    t.root().front().push_back("Z");
    t.root().front().push_back("Y");
    t.root()[1].push_back("R");
    t.root()[1].push_back("Q");
    t.root()[1].push_back("P");
    t.root().back().push_back("X");
    t.root().back().push_back("W");

    // Child node iterators are random-access for the raw<> storage model,
    // so that means we can do things like call sort() on them

    // currently this is not compiling -- I suspect a compiler limit on
    // size/complexity of type expression matching, but I'm not sure yet
    //sort(t.root()[1].begin(), t.root()[1].end(), data_lt());
    // why does this match correctly, but invocation of sort() above doesn't match?
    test(t.root()[1].begin(), t.root()[1].end(), data_lt());

    // Output data in breadth first order
    // Observe that only children of t.root()[1] ("C") are sorted.
    for (iterator j(t.begin());  j != t.end();  ++j)
        cout << j->data() << " ";
    cout << "\n";

    return 0;
}
