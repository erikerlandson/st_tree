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


template <typename X>
struct dref_vmap {
    // X is a type that is de-referenceable: supports the unary "*" dereference operator 
    typedef typeof(*(X())) drX;
    drX& operator()(X& x) const { return *x; }
    bool operator==(const dref_vmap& rhs) const { return true; }
    bool operator!=(const dref_vmap& rhs) const { return false; }
};

template <typename X>
struct dref_second_vmap {
    // X is a type that is de-referenceable: supports the unary "*" dereference operator 
    typedef typeof(*(X())) drX;
    typedef typeof(drX().first) R;
    R& operator()(X& x) const { return x->second; }
    bool operator==(const dref_second_vmap& rhs) const { return true; }
    bool operator!=(const dref_second_vmap& rhs) const { return false; }
};



template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor {
    protected:
    typedef Iterator base;
    typedef ValMap valmap;

    base _base;
    valmap _vmap;

    public:
    typedef std::forward_iterator_tag iterator_category;
    typedef typeof(_vmap(*base())) value_type;
    typedef typename base::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    // default ctor/dtor
    valmap_iterator_adaptor(): _base(), _vmap() {}
    virtual ~valmap_iterator_adaptor() {}

    // copy/assign
    valmap_iterator_adaptor(const valmap_iterator_adaptor& src): _base(src._base), _vmap(src._vmap) {}
    valmap_iterator_adaptor& operator=(const valmap_iterator_adaptor& rhs) {
        if (this == &rhs) return *this;
        _base = rhs._base;
        _vmap = rhs._vmap;
        return *this;
    }

    // construct from specific valmap obj
    valmap_iterator_adaptor(const valmap& vmap): _base(), _vmap(vmap) {}

    // casting
    valmap_iterator_adaptor(const base& src): _base(src), _vmap() {}
    valmap_iterator_adaptor(const base& src, const valmap& vmap): _base(src), _vmap(vmap) {}
    valmap_iterator_adaptor& operator=(const base& rhs) {
        _base = rhs;
        return *this;
    }

    // pre-increment:
    valmap_iterator_adaptor operator++() {
        ++_base;  
        return *this;
    }

    // post-increment:
    valmap_iterator_adaptor operator++(int) {
        valmap_iterator_adaptor r(*this);
        ++(*this);
        return r;
    }

    // access methods
    reference operator*() const { return _vmap(*_base); }
    pointer operator->() const { return &(_vmap(*_base)); }

    bool operator==(const valmap_iterator_adaptor& rhs) const {
        if (_base != rhs._base) return false;
        if (!(_vmap == rhs._vmap)) return false;
        return true;
    }
    bool operator!=(const valmap_iterator_adaptor& rhs) const { return !(*this == rhs); }
};


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
struct cscat {
    typedef NodeStorage ns_type;
    typedef Arg2 arg2_type;
    typedef Arg3 arg3_type;
};



struct bad_node_storage_spec {};

template <typename Data, typename BadSpec> 
struct cscat_dispatch {
    // In this case catch-all should be compile error:
    typedef bad_node_storage_spec spec;
};

template <typename Data>
struct cscat_dispatch<Data, raw> {
    typedef cscat<raw, arg_unused, arg_unused> spec;
};

template <typename Data>
struct cscat_dispatch<Data, cscat<raw, arg_default, arg_default> > {
    typedef cscat<raw, arg_unused, arg_unused> spec;
};

template <typename Data> 
struct cscat_dispatch<Data, ordered_unique> {
    typedef cscat<ordered_unique, less<Data>, arg_unused> spec;
};

template <typename Data>
struct cscat_dispatch<Data, cscat<ordered_unique, arg_default, arg_default> > {
    typedef cscat<ordered_unique, less<Data>, arg_unused> spec;
};

template <typename Data, typename Comp>
struct cscat_dispatch<Data, cscat<ordered_unique, Comp, arg_default> > {
    typedef cscat<ordered_unique, Comp, arg_unused> spec;
};

template <typename Data> 
struct cscat_dispatch<Data, ordered> {
    typedef cscat<ordered, less<Data>, arg_unused> spec;
};

template <typename Data> 
struct cscat_dispatch<Data, cscat<ordered, arg_default, arg_default> > {
    typedef cscat<ordered, less<Data>, arg_unused> spec;
};

template <typename Data, typename Comp>
struct cscat_dispatch<Data, cscat<ordered, Comp, arg_default> > {
    typedef cscat<ordered, Comp, arg_unused> spec;
};

template <typename Data, typename Key>
struct cscat_dispatch<Data, cscat<keyed_unique, Key, arg_default> > {
    typedef cscat<keyed_unique, Key, less<Key> > spec;
};

template <typename Data, typename Key, typename KeyComp>
struct cscat_dispatch<Data, cscat<keyed_unique, Key, KeyComp> > {
    typedef cscat<keyed_unique, Key, KeyComp > spec;
};

template <typename Data, typename Key>
struct cscat_dispatch<Data, cscat<keyed, Key, arg_default> > {
    typedef cscat<keyed, Key, less<Key> > spec;
};

template <typename Data, typename Key, typename KeyComp>
struct cscat_dispatch<Data, cscat<keyed, Key, KeyComp> > {
    typedef cscat<keyed, Key, KeyComp > spec;
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


template <typename Data, typename ChildContainer>
struct node_base {
    
};

/*
template <typename Tree, typename NodeBase, typename Data>
    struct node_raw: public node_internal_base<Tree, NodeBase, vector<shared_ptr<node_raw<Tree, NodeBase, InternalData> > > > {
    typedef node_internal_raw_noleaf<Tree, NodeBase, InternalData> this_type;
    typedef Tree tree_type;
    typedef vector<shared_ptr<this_type> > ns_type;
    typedef node_internal_base<Tree, NodeBase, ns_type> base_type;
    typedef typename Tree::size_type size_type;
    typedef this_type internal_type;
    typedef internal_type leaf_type;
    typedef InternalData data_type;

    // this variation has no leaf type, so node_type is type of this class itself
    typedef this_type node_type;

    // NodeContainer is expected to be a vector<>, or something that provides same interface 
    typedef typename ns_type::iterator iterator;
    typedef typename ns_type::const_iterator const_iterator;

    friend class tree_type::tree_type;

    node_internal_raw_noleaf() : base_type() {}
    virtual ~node_internal_raw_noleaf() {}

    shared_ptr<node_type> operator[](size_type n) { return this->_children[n]; }
    shared_ptr<const node_type> operator[](size_type n) const { return this->_children[n]; }

    void insert(const data_type& data) {
        shared_ptr<internal_type> n(new internal_type);
        n->_data = data;
        n->_tree = this->_tree;
        n->set_parent(this->_this, n);
        this->_children.push_back(n);
        this->_tree->_size += 1;
    }
    void insert_leaf(const data_type& data) { insert(data); }
};
*/

template <typename CSCat>
struct node_type_dispatch {
    
};


template <typename Data, typename CSCat>
struct tree {

};


};  // namespace ootree

#endif  // __ootree_h__
