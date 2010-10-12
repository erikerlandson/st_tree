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
#include <limits>
#include "stdio.h"

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


template <typename Unsigned>
struct max_maintainer {
    max_maintainer(): _hist(), _max(0) {}
    virtual ~max_maintainer() {}

    max_maintainer(const max_maintainer& src) { *this = src; }
    max_maintainer& operator=(const max_maintainer& rhs) {
        if (this == &rhs) return *this;
        _max = rhs._max;
        _hist = rhs._hist;
        return *this;
    }

    Unsigned max() const { return _max; }

    void insert(const Unsigned& n) {
        if (n >= _hist.size()) _hist.resize(1+n, 0);
        _hist[n] += 1;
        if (n > _max) _max = n;
    }

    void erase(const Unsigned& n) {
        if (n > _max) return;
        _hist[n] -= 1;
        if (_hist[_max] > 0) return;
        while (true) {
            if (_max == 0) break;
            _max -= 1;
            if (_hist[_max] > 0) break;
        }
    }

    protected:
    Unsigned _max;
    vector<Unsigned> _hist;
};


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
    operator base() const { return _base; }
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
    typedef bad_node_storage_spec cat;
};

template <typename Data>
struct cscat_dispatch<Data, raw> {
    typedef cscat<raw, arg_unused, arg_unused> cat;
};

template <typename Data>
struct cscat_dispatch<Data, cscat<raw, arg_default, arg_default> > {
    typedef cscat<raw, arg_unused, arg_unused> cat;
};

template <typename Data> 
struct cscat_dispatch<Data, ordered_unique> {
    typedef cscat<ordered_unique, less<Data>, arg_unused> cat;
};

template <typename Data>
struct cscat_dispatch<Data, cscat<ordered_unique, arg_default, arg_default> > {
    typedef cscat<ordered_unique, less<Data>, arg_unused> cat;
};

template <typename Data, typename Comp>
struct cscat_dispatch<Data, cscat<ordered_unique, Comp, arg_default> > {
    typedef cscat<ordered_unique, Comp, arg_unused> cat;
};

template <typename Data> 
struct cscat_dispatch<Data, ordered> {
    typedef cscat<ordered, less<Data>, arg_unused> cat;
};

template <typename Data> 
struct cscat_dispatch<Data, cscat<ordered, arg_default, arg_default> > {
    typedef cscat<ordered, less<Data>, arg_unused> cat;
};

template <typename Data, typename Comp>
struct cscat_dispatch<Data, cscat<ordered, Comp, arg_default> > {
    typedef cscat<ordered, Comp, arg_unused> cat;
};

template <typename Data, typename Key>
struct cscat_dispatch<Data, cscat<keyed_unique, Key, arg_default> > {
    typedef cscat<keyed_unique, Key, less<Key> > cat;
};

template <typename Data, typename Key, typename KeyComp>
struct cscat_dispatch<Data, cscat<keyed_unique, Key, KeyComp> > {
    typedef cscat<keyed_unique, Key, KeyComp > cat;
};

template <typename Data, typename Key>
struct cscat_dispatch<Data, cscat<keyed, Key, arg_default> > {
    typedef cscat<keyed, Key, less<Key> > cat;
};

template <typename Data, typename Key, typename KeyComp>
struct cscat_dispatch<Data, cscat<keyed, Key, KeyComp> > {
    typedef cscat<keyed, Key, KeyComp > cat;
};


template <typename Compare>
struct ptr_less_data {
    ptr_less_data() : _comp() {}
    virtual ~ptr_less_data() {}

    template <typename Pointer>
    bool operator()(const Pointer& a, const Pointer& b) const { return _comp(a->data(), b->data()); }

    Compare _comp;
};


template <typename Tree, typename Node, typename ChildContainer>
struct node_base {
    typedef node_base<Tree, Node, ChildContainer> this_type;
    typedef Tree tree_type;
    typedef Node node_type;
    typedef ChildContainer cs_type;
    typedef typename tree_type::size_type size_type;
    typedef typename tree_type::data_type data_type;

    node_base() : _tree(NULL), _ply(0), _size(0), _parent(), _this(), _data(), _children() {}
    virtual ~node_base() {}

    size_type ply() const { return _ply; }
    size_type depth() const { return _tree->depth() - _ply; }
    size_type subtree_size() const { return _size; }

    bool is_root() const { return ply() == 0; }

    node_type& parent() {
        if (ply() == 0) throw exception();
        return *(_parent.lock());
    }
    const node_type& parent() const {
        if (ply() == 0) throw exception();
        return *(_parent.lock()); 
    }

    data_type& data() { return _data; }
    const data_type& data() const { return _data; }

    protected:
    typedef typename cs_type::iterator cs_iterator;
    
    public:
    typedef valmap_iterator_adaptor<cs_iterator, dref_vmap<typename cs_iterator::value_type> > iterator;

    iterator begin() { return iterator(_children.begin()); }
    iterator end() { return iterator(_children.end()); }

    size_type size() const { return _children.size(); }
    bool empty() const { return _children.empty(); }

    void erase(const iterator& j) {
        _children.erase(cs_iterator(j));
    }

    friend class tree_type::tree_type;
    protected:
    tree_type* _tree;
    size_type _ply;
    size_type _size;
    weak_ptr<node_type> _parent;
    weak_ptr<node_type> _this;
    data_type _data;
    cs_type _children;

    void graft_subtree(shared_ptr<node_type>& n) {
        // set new parent for this subtree as current node
        shared_ptr<node_type> q = _this.lock();
        n->_parent = q;
 
        // percolate the new subtree size up the chain of parents
        while (true) {
            q->_size += n->_size;
            if (q->ply() == 0) {
                _tree->_size += n->_size;
                break;
            }
            q = q->_parent.lock();
        }

        // percolate new ply values and tree ptr down the subtree, recursively
        deque<shared_ptr<node_type> > nq;
        nq.push_back(n);
        while (!nq.empty()) {
            q = nq.front();
            nq.pop_front();
            q->_tree = _tree;
            q->_ply = 1 + q->_parent.lock()->_ply;
            _tree->_depth.insert(1 + q->_ply);
            for (cs_iterator j(q->_children.begin());  j != q->_children.end();  ++j) nq.push_back(*j);
        }
    }
};


template <typename Tree, typename Data>
struct node_raw: public node_base<Tree, node_raw<Tree, Data>, vector<shared_ptr<node_raw<Tree, Data> > > > {
    typedef node_raw<Tree, Data> this_type;
    typedef this_type node_type;
    typedef Tree tree_type;
    typedef vector<shared_ptr<node_type> > cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef typename Tree::size_type size_type;
    typedef Data data_type;

    typedef typename base_type::iterator iterator;

    friend class tree_type::tree_type;

    node_raw() : base_type() {}
    virtual ~node_raw() {}

    node_type& operator[](size_type n) { return *(this->_children[n]); }
    const node_type& operator[](size_type n) const { return *(this->_children[n]); }

    void insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_this = n;
        n->_size = 1;
        this->_children.push_back(n);
        this->graft_subtree(n);
    }
};


struct node_type_dispatch_failed {};

template <typename Tree, typename CSCat>
struct node_type_dispatch {
    // catch-all should be compile error
    typedef node_type_dispatch_failed node_type;
};


template <typename Tree>
struct node_type_dispatch<Tree, cscat<raw, arg_unused, arg_unused> > {
    typedef node_raw<Tree, typename Tree::data_type> node_type;
    // raw storage class uses vector
    typedef vector<shared_ptr<node_type> > node_container;
    // why do I need this?  because of friend declarations, that's why.
    typedef node_base<Tree, node_type, node_container> base_type;
};


template <typename Data, typename CSCat=raw>
struct tree {
    typedef tree<Data, CSCat> this_type;
    typedef this_type tree_type;
    typedef unsigned long size_type;
    typedef Data data_type;

    typedef typename cscat_dispatch<Data, CSCat>::cat cscat;
    typedef node_type_dispatch<tree_type, cscat> nt_dispatch;
    typedef typename nt_dispatch::node_type node_type;
    typedef typename nt_dispatch::base_type base_type;

/*
    typedef b1st_iterator<node_type> bf_iterator;
    typedef d1st_post_iterator<node_type> df_post_iterator;
    typedef d1st_pre_iterator<node_type> df_pre_iterator;
*/

    tree() : _size(0), _root() {}
    virtual ~tree() { clear(); }

/*
    tree(const tree& src) : _size(0), _root() { *this = src; }

    tree& operator=(const tree& src) {
        if (&src == this) return *this;
        tree_deep_copy<this_type, dt_spec, ns_spec> tdc;
        tdc(*this, src);
        return *this;
    }
*/

    size_type size() const { return _size; }
    bool empty() const { return 0 == size(); }

    size_type depth() const { return _depth.max(); }

    node_type& root() {
        if (0 == size()) throw exception();
        return *_root;
    }

    const node_type& root() const {
        if (0 == size()) throw exception();
        return *_root;
    }

    void insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_tree = this;
        n->_this = n;
        _root = n;
        _size = 1;
        if (_depth.max() < 1) _depth.insert(1);
    }

    // there is only one node to erase from the tree: the root
    void erase() {
/*
        if (_root == NULL) return;
        if (_root->is_internal()) internal(_root)->clear();
        _root->detach();
        _root.reset();
        _size = 0;
*/
    }
    void clear() { erase(); }

/*
    void swap(this_type& src) {
        if (this == &src) return;
        std::swap(_root, src._root);
        std::swap(_size, src._size);
    }


/*
    bf_iterator bf_begin() { return bf_iterator(_root); }
    bf_iterator bf_end() { return bf_iterator(); }

    df_post_iterator df_post_begin() { return df_post_iterator(_root); }
    df_post_iterator df_post_end() { return df_post_iterator(); }

    df_pre_iterator df_pre_begin() { return df_pre_iterator(_root); }
    df_pre_iterator df_pre_end() { return df_pre_iterator(); }
*/

    // compile will not let me use just node_type here
    friend class node_type_dispatch<tree_type, cscat>::node_type;
    friend class node_type_dispatch<tree_type, cscat>::base_type;

    protected:
    shared_ptr<node_type> _root;
    size_type _size;
    max_maintainer<size_type> _depth;
};


};  // namespace ootree

#endif  // __ootree_h__
