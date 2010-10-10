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

// use the mtree header and namespace
#include "ootree.h"
using namespace ootree;

#include <set>

int main(int argc, char** argv) {
    int* t = new int(4);
    const int* t2 = new int(5);
    dref_vmap<int*> tv;
    cout << tv(t) << "\n";

    {
    vector<shared_ptr<int> > v;
    typedef typeof(v.begin()) iterator;
    v.push_back(shared_ptr<int>(new int(1)));
    v.push_back(shared_ptr<int>(new int(2)));
    v.push_back(shared_ptr<int>(new int(3)));
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";

    *(vm_iterator(v.begin())) = 42;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";
    }
    
    {
    vector<int* > v;
    typedef typeof(v.begin()) iterator;
    v.push_back(new int(1));
    v.push_back(new int(2));
    v.push_back(new int(3));
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";

    *(vm_iterator(v.begin())) = 42;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";
    }


    {
    vector<pair<int, int>* > v;
    typedef typeof(v.begin()) iterator;
    v.push_back(new pair<int,int>(1,2));
    v.push_back(new pair<int,int>(2,3));
    v.push_back(new pair<int,int>(3,4));
    typedef valmap_iterator_adaptor<iterator, dref_vmap<iterator::value_type > > vm_iterator;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << j->first << "\n";

    (vm_iterator(v.begin()))->first = 42;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << j->first << "\n";

    }


    {
    vector<pair<int, int>* > v;
    typedef typeof(v.begin()) iterator;
    v.push_back(new pair<int,int>(1,2));
    v.push_back(new pair<int,int>(2,3));
    v.push_back(new pair<int,int>(3,4));
    typedef valmap_iterator_adaptor<iterator, dref_second_vmap<iterator::value_type > > vm_iterator;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";

    *(vm_iterator(v.begin())) = 42;
    for (vm_iterator j(v.begin()); j != vm_iterator(v.end()); ++j)
        cout << *j << "\n";

    }

    return 0;
}
