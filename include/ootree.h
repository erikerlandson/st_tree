/******
ootree: A highly configurable C++ template tree class, using STL style interfaces.

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

#if !defined(__ootree_h__)
#define __ootree_h__ 1


#include <tr1/memory>
#include <vector>
#include <deque>
#include <set>
#include <functional>
#include <algorithm>
#include <iterator>

namespace ootree {
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::dynamic_pointer_cast;
using std::vector;
using std::deque;
using std::set;
using std::multiset;
using std::less;
using std::pair;



// Node storage classes:
// use this if you want raw unordered child node storage, low overhead, linear-time deletion
struct raw {};
// provides ordered child node storage, non-unique
struct ordered {};
// this provides ordered child node storage, unique by ordering
struct ordered_unique {};
// child nodes are ordered by non-unique external key
struct keyed {};
// child nodes are ordered by unique external key
struct keyed_unique {};


// some data types to flag template argument behaviors
struct arg_default {};
struct arg_unused {};
struct arg_void {};


struct exception {
    exception() {}
    virtual ~exception() {}
// stub.  This will need some work.
};

// Node storage spec
template <typename NodeStorage, typename Arg2=arg_default, typename Arg3=arg_default> 
struct ns_spec {
    typedef NodeStorage ns_type;
    typedef Arg2 arg2_type;
    typedef Arg3 arg3_type;
};



struct bad_node_storage_spec {};

template <typename Data, typename BadSpec> 
struct ns_spec_dispatch {
    // In this case catch-all should be compile error:
    typedef bad_node_storage_spec spec;
};

template <typename Data>
struct ns_spec_dispatch<Data, raw> {
    typedef ns_spec<raw, arg_unused, arg_unused> spec;
};

template <typename Data>
struct ns_spec_dispatch<Data, ns_spec<raw, arg_default, arg_default> > {
    typedef ns_spec<raw, arg_unused, arg_unused> spec;
};

template <typename Data> 
struct ns_spec_dispatch<Data, ordered_unique> {
    typedef ns_spec<ordered_unique, less<Data>, arg_unused> spec;
};

template <typename Data>
struct ns_spec_dispatch<Data, ns_spec<ordered_unique, arg_default, arg_default> > {
    typedef ns_spec<ordered_unique, less<Data>, arg_unused> spec;
};

template <typename Data, typename Comp>
struct ns_spec_dispatch<Data, ns_spec<ordered_unique, Comp, arg_default> > {
    typedef ns_spec<ordered_unique, Comp, arg_unused> spec;
};

template <typename Data> 
struct ns_spec_dispatch<Data, ordered> {
    typedef ns_spec<ordered, less<Data>, arg_unused> spec;
};

template <typename Data> 
struct ns_spec_dispatch<Data, ns_spec<ordered, arg_default, arg_default> > {
    typedef ns_spec<ordered, less<Data>, arg_unused> spec;
};

template <typename Data, typename Comp>
struct ns_spec_dispatch<Data, ns_spec<ordered, Comp, arg_default> > {
    typedef ns_spec<ordered, Comp, arg_unused> spec;
};

template <typename Data, typename Key>
struct ns_spec_dispatch<Data, ns_spec<keyed_unique, Key, arg_default> > {
    typedef ns_spec<keyed_unique, Key, less<Key> > spec;
};

template <typename Data, typename Key, typename KeyComp>
struct ns_spec_dispatch<Data, ns_spec<keyed_unique, Key, KeyComp> > {
    typedef ns_spec<keyed_unique, Key, KeyComp > spec;
};

template <typename Data, typename Key>
struct ns_spec_dispatch<Data, ns_spec<keyed, Key, arg_default> > {
    typedef ns_spec<keyed, Key, less<Key> > spec;
};

template <typename Data, typename Key, typename KeyComp>
struct ns_spec_dispatch<Data, ns_spec<keyed, Key, KeyComp> > {
    typedef ns_spec<keyed, Key, KeyComp > spec;
};


template <typename Compare> 
struct ptr_less_data {
    ptr_less_data() : _comp() {}
    virtual ~ptr_less_data() {}

    template <typename Pointer>
    bool operator()(const Pointer& a, const Pointer& b) const { return _comp(a->data(), b->data()); }

    Compare _comp;
};


// Any node containing an external key inherits from this
// to use a storage class w/ external key, both internal and
// leaf nodes will need this, and it will need to be
// in base class
template <typename Key, typename Tree>
struct node_key {
    typedef Tree tree_type;
    typedef Key key_type;

    node_key() : _key() {}
    virtual ~node_key() {}

    const key_type& key() const { return _key; }

    protected:
    key_type _key;
};



};  // namespace ootree

#endif  // __ootree_h__
