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


#include <vector>
#include <deque>
#include <set>
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include "stdio.h"

namespace ootree {
using std::vector;
using std::deque;
using std::multiset;
using std::map;
using std::less;
using std::pair;
using std::cerr;


// some data types to flag template argument behaviors
struct arg_default {};
struct arg_unused {};

// Node storage classes:
// use this if you want raw (vector) child node storage
template <typename Unused = arg_unused>
struct raw {};
// provides ordered child node storage
template <typename Compare = arg_default>
struct ordered {};
// child nodes are indexed by external key
template <typename Key, typename Compare = less<Key> >
struct keyed {};




struct exception {
    exception() {}
    virtual ~exception() {}
// stub.  This will need some work.
};


template <typename Unsigned, typename Alloc>
struct max_maintainer {
    typedef typename vector<Unsigned>::size_type size_type;

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
        while ((_max > 0) && (_hist[_max] <= 0)) _max -= 1;
    }

    void insert(const max_maintainer& src, const Unsigned& d) {
        if (src._hist.size() <= 0) return;
        if ((src._max + d) > _max) _max = src._max + d;
        if (_max >= _hist.size()) _hist.resize(1+_max, 0);
        for (size_type s = 0;  s <= src._max;  ++s) _hist[s + d] += src._hist[s];
    }

    void erase(const max_maintainer& src, const Unsigned& d) {
        if (src._hist.size() <= 0) return;
        size_type n = src._max;
        if (d > _max) return;
        if ((_max-d) < n) n = _max-d;
        for (size_type s = 0;  s <= n;  ++s) _hist[s + d] -= src._hist[s];
        while ((_max > 0) && (_hist[_max] <= 0)) _max -= 1;
    }

    void clear() {
        _hist.clear();
        _max = 0;
    }

    void swap(max_maintainer& src) {
        if (this == &src) return;
        std::swap(_max, src._max);
        _hist.swap(src._hist);
    }

    protected:
    Unsigned _max;
    vector<Unsigned, Alloc> _hist;
};


// This is my poor-man substitute for typeof(*dr)
template <typename DR> struct dr_value {};

template <typename V>
struct dr_value<V*> {
    typedef V value_type;
};

template <typename DR>
struct dref_vmap {
    // DR is a type that is de-referenceable: supports the unary "*" dereference operator 
    typedef typename dr_value<DR>::value_type value_type;

    // These both return a non-const reference -- this enables me to handle
    // (multi)set iterators in a useful way below: those iterators are always const,
    // however I need non-const behavior for many ops.  It is only the data field that
    // must remain const, because it is the true sorting key
    value_type& operator()(DR& x) const { return *x; }
    value_type& operator()(const DR& x) const { return const_cast<value_type&>(*x); }

    bool operator==(const dref_vmap& rhs) const { return true; }
    bool operator!=(const dref_vmap& rhs) const { return false; }
};


template <typename P>
struct dref_second_vmap {
    // P is assumed to be of type pair<>, or at least define 'second' and 'second_type'
    typedef typename dr_value<typename P::second_type>::value_type value_type;

    // Set these both to return non-const reference (see dref_vmap comment above)
    value_type& operator()(P& x) const { return *(x.second); }
    value_type& operator()(const P& x) const { return const_cast<value_type&>(*(x.second)); }

    bool operator==(const dref_second_vmap& rhs) const { return true; }
    bool operator!=(const dref_second_vmap& rhs) const { return false; }
};


template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor_forward {
    protected:
    typedef Iterator base;
    typedef ValMap valmap;

    base _base;
    valmap _vmap;

    public:
    typedef std::forward_iterator_tag iterator_category;
    typedef typename ValMap::value_type value_type;
    typedef typename base::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    typedef Iterator base_iterator;

    // default ctor/dtor
    valmap_iterator_adaptor_forward(): _base(), _vmap() {}
    virtual ~valmap_iterator_adaptor_forward() {}

    // copy/assign
    valmap_iterator_adaptor_forward(const valmap_iterator_adaptor_forward& src): _base(src._base), _vmap(src._vmap) {}
    valmap_iterator_adaptor_forward& operator=(const valmap_iterator_adaptor_forward& rhs) {
        if (this == &rhs) return *this;
        _base = rhs._base;
        _vmap = rhs._vmap;
        return *this;
    }

    // construct from specific valmap obj
    valmap_iterator_adaptor_forward(const valmap& vmap): _base(), _vmap(vmap) {}

    // casting
    operator base() const { return _base; }
    valmap_iterator_adaptor_forward(const base& src): _base(src), _vmap() {}
    valmap_iterator_adaptor_forward(const base& src, const valmap& vmap): _base(src), _vmap(vmap) {}
    valmap_iterator_adaptor_forward& operator=(const base& rhs) {
        _base = rhs;
        return *this;
    }

    // pre-increment:
    valmap_iterator_adaptor_forward operator++() {
        ++_base;
        return *this;
    }

    // post-increment:
    valmap_iterator_adaptor_forward operator++(int) {
        valmap_iterator_adaptor_forward r(*this);
        ++(*this);
        return r;
    }

    // access methods
    reference operator*() const { return _vmap(*_base); }
    pointer operator->() const { return &(_vmap(*_base)); }

    bool operator==(const valmap_iterator_adaptor_forward& rhs) const {
        if (_base != rhs._base) return false;
        if (!(_vmap == rhs._vmap)) return false;
        return true;
    }
    bool operator!=(const valmap_iterator_adaptor_forward& rhs) const { return !(*this == rhs); }
};


template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor_bidirectional : public valmap_iterator_adaptor_forward<Iterator, ValMap> {
    typedef valmap_iterator_adaptor_forward<Iterator, ValMap> base_type;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::reference reference;

    typedef Iterator base_iterator;

    valmap_iterator_adaptor_bidirectional() : base_type() {}
    virtual ~valmap_iterator_adaptor_bidirectional() {}
    
    valmap_iterator_adaptor_bidirectional(const valmap_iterator_adaptor_bidirectional& src) : base_type(src) {}
    valmap_iterator_adaptor_bidirectional& operator=(const valmap_iterator_adaptor_bidirectional& src) {
        if (this == &src) return *this;
        base_type::operator=(src);
        return *this;
    }

    operator base_iterator() const { return this->_base; }
    valmap_iterator_adaptor_bidirectional(const base_iterator& src) : base_type(src) {}
    valmap_iterator_adaptor_bidirectional& operator=(const base_iterator& src) {
        base_type::operator=(src);
        return *this;
    }

    // pre-dec:
    valmap_iterator_adaptor_bidirectional operator--() {
        --(this->_base);
        return *this;
    }

    // post-dec:
    valmap_iterator_adaptor_bidirectional operator--(int) {
        valmap_iterator_adaptor_bidirectional r(*this);
        --(*this);
        return r;
    }
};

template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor_random : public valmap_iterator_adaptor_bidirectional<Iterator, ValMap> {
    typedef valmap_iterator_adaptor_bidirectional<Iterator, ValMap> base_type;

    typedef std::random_access_iterator_tag iterator_category;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::pointer pointer;
    typedef typename base_type::reference reference;

    typedef Iterator base_iterator;

    valmap_iterator_adaptor_random() : base_type() {}
    virtual ~valmap_iterator_adaptor_random() {}
    
    valmap_iterator_adaptor_random(const valmap_iterator_adaptor_random& src) : base_type(src) {}
    valmap_iterator_adaptor_random& operator=(const valmap_iterator_adaptor_random& src) {
        if (this == &src) return *this;
        base_type::operator=(src);
        return *this;
    }

    operator base_iterator() const { return this->_base; }
    valmap_iterator_adaptor_random(const base_iterator& src) : base_type(src) {}
    valmap_iterator_adaptor_random& operator=(const base_iterator& src) {
        base_type::operator=(src);
        return *this;
    }

    valmap_iterator_adaptor_random& operator+=(const difference_type& n) {
        this->_base += n;
        return *this;
    }
    valmap_iterator_adaptor_random& operator-=(const difference_type& n) {
        this->_base -= n;
        return *this;
    }

    valmap_iterator_adaptor_random operator+(const difference_type& n) const {
        return valmap_iterator_adaptor_random(this->_base + n);
    }
    valmap_iterator_adaptor_random operator-(const difference_type& n) const {
        return valmap_iterator_adaptor_random(this->_base - n);
    }

    difference_type operator-(const valmap_iterator_adaptor_random& s) const {
        return this->_base - s._base;
    }

    reference operator[](const difference_type& n) {
        return this->_vmap(*(this->_base+n));
    }

    bool operator<(const valmap_iterator_adaptor_random& rhs) const {
        return this->_base < rhs._base;
    }
    bool operator<=(const valmap_iterator_adaptor_random& rhs) const {
        return this->_base <= rhs._base;
    }
    bool operator>(const valmap_iterator_adaptor_random& rhs) const {
        return this->_base > rhs._base;
    }
    bool operator>=(const valmap_iterator_adaptor_random& rhs) const {
        return this->_base >= rhs._base;
    }
};

template <typename Iterator, typename ValMap, typename DiffType>
valmap_iterator_adaptor_random<Iterator, ValMap> operator+(const DiffType& n, const valmap_iterator_adaptor_random<Iterator, ValMap>& j) {
    return j + n;
}



// default will be forward iterator
template <typename Iterator, typename ValMap, typename Category>
struct valmap_iterator_dispatch {
    typedef valmap_iterator_adaptor_forward<Iterator, ValMap> adaptor_type;
};

// bidirectional iterators
template <typename Iterator, typename ValMap>
struct valmap_iterator_dispatch<Iterator, ValMap, std::bidirectional_iterator_tag> {
    typedef valmap_iterator_adaptor_bidirectional<Iterator, ValMap> adaptor_type;
};

// random access iterators
template <typename Iterator, typename ValMap>
struct valmap_iterator_dispatch<Iterator, ValMap, std::random_access_iterator_tag> {
    typedef valmap_iterator_adaptor_random<Iterator, ValMap> adaptor_type;
};

template <typename Iterator, typename ValMap>
struct valmap_iterator_adaptor : public valmap_iterator_dispatch<Iterator, ValMap, typename Iterator::iterator_category>::adaptor_type {
    typedef typename valmap_iterator_dispatch<Iterator, ValMap, typename Iterator::iterator_category>::adaptor_type adaptor_type;

    typedef typename adaptor_type::iterator_category iterator_category;
    typedef typename adaptor_type::value_type value_type;
    typedef typename adaptor_type::difference_type difference_type;
    typedef typename adaptor_type::pointer pointer;
    typedef typename adaptor_type::reference reference;

    typedef Iterator base_iterator;

    valmap_iterator_adaptor() : adaptor_type() {}
    virtual ~valmap_iterator_adaptor() {}
    
    valmap_iterator_adaptor(const valmap_iterator_adaptor& src) : adaptor_type(src) {}
    valmap_iterator_adaptor& operator=(const valmap_iterator_adaptor& src) {
        if (this == &src) return *this;
        adaptor_type::operator=(src);
        return *this;
    }

    operator base_iterator() const { return this->_base; }
    valmap_iterator_adaptor(const base_iterator& src) : adaptor_type(src) {}
    valmap_iterator_adaptor& operator=(const base_iterator& src) {
        adaptor_type::operator=(src);
        return *this;
    }
};


template <typename Node, typename Value, typename Alloc>
struct b1st_iterator {
    typedef Node node_type;
    typedef typename Node::iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef typename iterator::difference_type difference_type;
    typedef Value* pointer;
    typedef Value& reference;

    b1st_iterator() : _queue() {}
    virtual ~b1st_iterator() {}

    b1st_iterator(const b1st_iterator& rhs) : _queue(rhs._queue) {}
    b1st_iterator& operator=(const b1st_iterator& rhs) { _queue = rhs._queue; }

    b1st_iterator(value_type* root) {
        if (root == NULL) return;
        _queue.push_back(const_cast<node_type*>(root));
    }

    reference operator*() const { return *(_queue.front()); }
    pointer operator->() const { return _queue.front(); }

    // pre-increment iterator
    b1st_iterator operator++() {
        // if we are already past the end of elements in tree, then this is a no-op
        if (_queue.empty()) return *this;
        
        // take current node off front of the queue
        node_type* f(_queue.front());
        _queue.pop_front();

        if (f->empty()) return *this;
        for (iterator j(f->begin());  j != iterator(f->end());  ++j)
            _queue.push_back(&*j);

        return *this;
    }

    // post-increment iterator
    b1st_iterator operator++(int) {
        b1st_iterator r(*this);
        ++(*this);
        return r;
    }

    bool operator==(const b1st_iterator& rhs) const { return _queue == rhs._queue; }
    bool operator!=(const b1st_iterator& rhs) const { return _queue != rhs._queue; }

    protected:
    deque<node_type*, Alloc> _queue;
};


template <typename Node, typename Value, typename Alloc>
struct d1st_post_iterator {
    typedef Node node_type;
    typedef typename node_type::iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef typename iterator::difference_type difference_type;
    typedef Value* pointer;
    typedef Value& reference;

    struct frame {
        frame(): first(), second(), visited() {}
        virtual ~frame() {}
        frame(const frame& rhs): first(rhs.first), second(rhs.second), visited(rhs.visited) {}
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; }
        frame(node_type* first_, const iterator& second_, const bool& visited_) {
            first = first_;  second = second_;  visited = visited_;
        }
        bool operator==(const frame& rhs) const {
            if (first != rhs.first) return false;
            if (second != rhs.second) return false;
            if (visited != rhs.visited) return false;
            return true;
        }
        bool operator!=(const frame& rhs) const { return !(*this == rhs); }
        node_type* first;
        iterator second;
        bool visited;
    };

    d1st_post_iterator() : _stack() {}
    virtual ~d1st_post_iterator() {}

    d1st_post_iterator(const d1st_post_iterator& rhs) : _stack(rhs._stack) { }
    d1st_post_iterator& operator=(const d1st_post_iterator& rhs) { _stack = rhs._stack; }

    d1st_post_iterator(value_type* root) {
        if (root == NULL) return;
        _stack.push_back(frame(const_cast<node_type*>(root), iterator(const_cast<node_type*>(root)->begin()), false));
        while (true) {
            if (_stack.back().first->empty()) {
                _stack.back().visited = true;
                break;
            }
            iterator b(_stack.back().first->begin());
            _stack.push_back(frame(&*b, iterator((b)->begin()), false));
        }
    }

    reference operator*() const { return *(_stack.back().first); }
    pointer operator->() const { return _stack.back().first; }

    // pre-increment
    d1st_post_iterator operator++() {
        // if we are already past the end of elements in tree, then this is a no-op
        if (_stack.empty()) return *this;

        // check whether we are ready to pop the current frame
        if (_stack.back().visited) _stack.pop_back();

        // empty stack flags that we just incremented past last element in tree, so leave in that state
        if (_stack.empty()) return *this;

        // we just popped up stack, so move to next child at this ply:
        ++(_stack.back().second);

        // if we have visited all the children, set the visited flag for this node
        if (_stack.back().second == iterator(_stack.back().first->end())) {
            _stack.back().visited = true;
            return *this;
        }

        // we found a next child at current ply: push down its first children to the bottom
        _stack.push_back(frame(&*(_stack.back().second), iterator((_stack.back().second)->begin()), false));
        while (true) {
            if (_stack.back().first->empty()) {
                _stack.back().visited = true;
                break;
            }
            iterator b(_stack.back().first->begin());
            _stack.push_back(frame(&*b, iterator((b)->begin()), false));                
        }

        return *this;
    }

    // post-increment
    d1st_post_iterator operator++(int) {
        d1st_post_iterator r(*this);
        ++(*this);
        return r;
    }

    bool operator==(const d1st_post_iterator& rhs) const { return _stack == rhs._stack; }
    bool operator!=(const d1st_post_iterator& rhs) const { return _stack != rhs._stack; }

    protected:
    vector<frame, Alloc> _stack;
};


template <typename Node, typename Value, typename Alloc>
struct d1st_pre_iterator {
    typedef Node node_type;
    typedef typename node_type::iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef typename iterator::difference_type difference_type;
    typedef Value* pointer;
    typedef Value& reference;

    struct frame {
        frame(): first(), second(), visited() {}
        virtual ~frame() {}
        frame(const frame& rhs): first(rhs.first), second(rhs.second), visited(rhs.visited) {}
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; }
        frame(node_type* first_, const iterator& second_, const bool& visited_) {
            first = first_;  second = second_;  visited = visited_;
        }
        bool operator==(const frame& rhs) const {
            if (first != rhs.first) return false;
            if (second != rhs.second) return false;
            if (visited != rhs.visited) return false;
            return true;
        }
        bool operator!=(const frame& rhs) const { return !(*this == rhs); }
        node_type* first;
        iterator second;
        bool visited;
    };

    d1st_pre_iterator() : _stack() {}
    virtual ~d1st_pre_iterator() {}

    d1st_pre_iterator(const d1st_pre_iterator& rhs) : _stack(rhs._stack) { }
    d1st_pre_iterator& operator=(const d1st_pre_iterator& rhs) { _stack = rhs._stack; }

    d1st_pre_iterator(value_type* root) {
        if (root == NULL) return;
        _stack.push_back(frame(const_cast<node_type*>(root), iterator(const_cast<node_type*>(root)->begin()), false));
    }

    reference operator*() const { return *(_stack.back().first); }
    pointer operator->() const { return _stack.back().first; }

    // pre-increment
    d1st_pre_iterator operator++() {
        // if we are already past the end of elements in tree, then this is a no-op
        if (_stack.empty()) return *this;

        // check the case where this is node we have just visited pre-order
        if (!_stack.back().visited) {
            _stack.back().visited = true;
            if (!_stack.back().first->empty()) {
                _stack.push_back(frame(&*(_stack.back().second), iterator(((_stack.back().second))->begin()), false));
                return *this;
            }
        }

        // pop off any frames we're finished with
        while (true) {
            if (!_stack.back().first->empty()) {
                ++(_stack.back().second);
                // in this case, we're not finished at this frame:
                if (_stack.back().second != iterator(_stack.back().first->end())) break;
            }
            _stack.pop_back();
            // if we emptied the stack, we're at end of tree elements
            if (_stack.empty()) return *this;
        }

        // push the next child
        _stack.push_back(frame(&*(_stack.back().second), iterator(((_stack.back().second))->begin()), false));

        return *this;
    }

    // post-increment
    d1st_pre_iterator operator++(int) {
        d1st_pre_iterator r(*this);
        ++(*this);
        return r;
    }

    bool operator==(const d1st_pre_iterator& rhs) const { return _stack == rhs._stack; }
    bool operator!=(const d1st_pre_iterator& rhs) const { return _stack != rhs._stack; }

    protected:
    vector<frame, Alloc> _stack;
};


template <typename Compare>
struct ptr_less {
    ptr_less() : _comp() {}
    virtual ~ptr_less() {}

    template <typename Pointer>
    bool operator()(const Pointer& a, const Pointer& b) const { return _comp(*a, *b); }

    Compare _comp;
};

template <typename Compare>
struct ptr_less_data {
    ptr_less_data() : _comp() {}
    virtual ~ptr_less_data() {}

    template <typename Pointer>
    bool operator()(const Pointer& a, const Pointer& b) const { return _comp(a->data(), b->data()); }

    Compare _comp;
};


template <typename Container>
struct dereferenceable_lessthan {
    template <typename D>
    bool operator()(const D& a, const D& b) const { return *a < *b; }
};

template <typename Key, typename Data, typename Compare, typename Alloc>
struct dereferenceable_lessthan<map<Key, Data, Compare, Alloc> > {
    template <typename D>
    bool operator()(const D& a, const D& b) const {
        if (_lt((a.first), (b.first))) return true;
        if (_lt((b.first), (a.first))) return false;
        return *(a.second) < *(b.second);
    }
    Compare _lt;
};

// forward declaration
template <typename Tree, typename Data, typename Key, typename Compare, typename Alloc> struct node_keyed;

template <typename Node, typename Value>
struct vmap_dispatch {
    typedef dref_vmap<Value> vmap;
};

template <typename Tree, typename Data, typename Key, typename Compare, typename Alloc, typename Value>
struct vmap_dispatch<node_keyed<Tree, Data, Key, Compare, Alloc>, Value> {
    typedef dref_second_vmap<Value> vmap;
};


template <typename Tree, typename Node, typename ChildContainer, typename Alloc>
struct node_base {
    typedef Tree tree_type;
    typedef Node node_type;
    typedef ChildContainer cs_type;
    typedef typename tree_type::size_type size_type;
    typedef typename tree_type::data_type data_type;

    protected:
    typedef typename cs_type::iterator cs_iterator;
    typedef typename cs_type::const_iterator cs_const_iterator;
    
    public:
    typedef valmap_iterator_adaptor<cs_iterator, typename vmap_dispatch<node_type, typename cs_iterator::value_type>::vmap> iterator;
    typedef valmap_iterator_adaptor<cs_const_iterator, typename vmap_dispatch<node_type, typename cs_const_iterator::value_type>::vmap> const_iterator;

    iterator begin() { return iterator(_children.begin()); }
    iterator end() { return iterator(_children.end()); }
    const_iterator begin() const { return const_iterator(_children.begin()); }
    const_iterator end() const { return const_iterator(_children.end()); }

    typedef b1st_iterator<node_type, node_type, Alloc> bf_iterator;
    typedef b1st_iterator<node_type, const node_type, Alloc> const_bf_iterator;
    typedef d1st_post_iterator<node_type, node_type, Alloc> df_post_iterator;
    typedef d1st_post_iterator<node_type, const node_type, Alloc> const_df_post_iterator;
    typedef d1st_pre_iterator<node_type, node_type, Alloc> df_pre_iterator;
    typedef d1st_pre_iterator<node_type, const node_type, Alloc> const_df_pre_iterator;

    bf_iterator bf_begin() { return bf_iterator(static_cast<node_type*>(this)); }
    bf_iterator bf_end() { return bf_iterator(); }
    const_bf_iterator bf_begin() const { return const_bf_iterator(static_cast<const node_type*>(this)); }
    const_bf_iterator bf_end() const { return const_bf_iterator(); }

    df_post_iterator df_post_begin() { return df_post_iterator(static_cast<node_type*>(this)); }
    df_post_iterator df_post_end() { return df_post_iterator(); }
    const_df_post_iterator df_post_begin() const { return const_df_post_iterator(static_cast<const node_type*>(this)); }
    const_df_post_iterator df_post_end() const { return const_df_post_iterator(); }

    df_pre_iterator df_pre_begin() { return df_pre_iterator(static_cast<node_type*>(this)); }
    df_pre_iterator df_pre_end() { return df_pre_iterator(); }
    const_df_pre_iterator df_pre_begin() const { return const_df_pre_iterator(static_cast<const node_type*>(this)); }
    const_df_pre_iterator df_pre_end() const { return const_df_pre_iterator(); }

    node_base() : _tree(NULL), _size(1), _parent(NULL), _data(), _children(), _depth() {}
    virtual ~node_base() {
        // Saves work, and also prevents exception attempting to call tree() on default-constructed nodes
        if (_children.empty() || _default_constructed()) return;
        // Save off child pointers, take down the child container, and then deallocate children
        vector<node_type*> d;
        for (iterator j(begin());  j != end();  ++j)  d.push_back(&*j);
        _children.clear();
        tree_type& tree_ = this->tree();
        for (typename vector<node_type*>::iterator e(d.begin());  e != d.end();  ++e)  tree_._delete_node(*e);
    }

    size_type ply() const {
        size_type p = 0;
        const node_type* q = static_cast<const node_type*>(this);
        while (!q->is_root()) {
            q = q->_parent;
            p += 1;
        }
        return p;
    }

    tree_type& tree() {
        node_type* q = static_cast<node_type*>(this);
        while (!q->is_root())  q = q->_parent;
        if (NULL == q->_tree) throw exception();
        return *(q->_tree);
    }

    const tree_type& tree() const {
        const node_type* q = static_cast<const node_type*>(this);
        while (!q->is_root())  q = q->_parent;
        if (NULL == q->_tree) throw exception();
        return *(q->_tree);
    }

    size_type depth() const { return _depth.max(); }
    size_type subtree_size() const { return _size; }

    bool is_root() const { return NULL == _parent; }

    bool is_ancestor(const node_type& n) const {
        const node_type* a = static_cast<const node_type*>(this);
        const node_type* q = &n;
        while (true) {
            if (q->is_root()) return false;
            q = q->_parent;
            if (q == a) return true;
        }
        return false;
    }

    node_type& parent() {
        if (is_root()) throw exception();
        return *(_parent);
    }
    const node_type& parent() const {
        if (is_root()) throw exception();
        return *(_parent); 
    }

    size_type size() const { return _children.size(); }
    bool empty() const { return _children.empty(); }

    void erase(const iterator& j) {
        node_type* n = &*j;
        _prune(n);
        _children.erase(j);
        this->tree()._delete_node(n);
    }

    void erase(const iterator& F, const iterator& L) {
        vector<node_type*> d;
        for (iterator j(F);  j != L;  ++j) {
            node_type* n = &*j;
            _prune(n);
            d.push_back(n);
        }
        _children.erase(F, L);
        tree_type& tree_ = this->tree();
        for (typename vector<node_type*>::iterator e(d.begin());  e != d.end();  ++e) tree_._delete_node(*e);
    }

    void erase() {
        if (is_root()) _tree->erase();
        else parent().erase(_iterator());
    }

    void clear() {
        erase(begin(), end());
    }

    bool operator==(const node_base& rhs) const {
        if (this == &rhs) return true;
        if (_children.size() != rhs._children.size()) return false;
        if (_data != rhs._data) return false;
        for (const_iterator jL(begin()), jR(rhs.begin());  jL != end();  ++jL,++jR)
            if (*jL != *jR) return false;
        return true;
    }
    bool operator!=(const node_base& rhs) const { return !(*this == rhs); }

    bool operator<(const node_base& rhs) const {
        if (this == &rhs) return false;
        if (_data != rhs._data) return (_data < rhs._data);
        dereferenceable_lessthan<cs_type> lt;
        return std::lexicographical_compare(_children.begin(), _children.end(), rhs._children.begin(), rhs._children.end(), lt);
    }
    bool operator>(const node_base& rhs) const { return rhs < *this; }
    bool operator<=(const node_base& rhs) const { return !(rhs < *this); }
    bool operator>=(const node_base& rhs) const { return !(*this < rhs); }

    friend class tree_type::tree_type;
    friend class b1st_iterator<node_type, node_type, Alloc>;
    friend class b1st_iterator<node_type, const node_type, Alloc>;
    friend class d1st_post_iterator<node_type, node_type, Alloc>;
    friend class d1st_post_iterator<node_type, const node_type, Alloc>;
    friend class d1st_pre_iterator<node_type, node_type, Alloc>;
    friend class d1st_pre_iterator<node_type, const node_type, Alloc>;

    protected:
    tree_type* _tree;
    size_type _size;
    max_maintainer<size_type, Alloc> _depth;
    node_type* _parent;
    data_type _data;
    cs_type _children;

    bool _default_constructed() const {
        return (NULL == _parent) && (NULL == _tree);
    }

    iterator _iterator() { return iterator(node_type::_cs_iterator(*static_cast<node_type*>(this))); }

    void _prune(node_type* n) {
        // percolate the new subtree size up the chain of parents
        node_type* q = static_cast<node_type*>(this);
        size_type dd = 1;
        while (true) {
            q->_size -= n->_size;
            q->_depth.erase(n->_depth, dd);
            if (q->is_root()) {
                break;
            }
            q = q->_parent;
            dd += 1;
        }
    }

    void _graft(node_type* n) {
        // set new parent for this subtree as current node
        node_type* q = static_cast<node_type*>(this);
        n->_parent = q;
        n->_tree = NULL;
 
        // percolate the new subtree size up the chain of parents
        size_type dd = 1;
        while (true) {
            q->_depth.insert(n->_depth, dd);
            q->_size += n->_size;
            if (q->is_root()) {
                break;
            }
            q = q->_parent;
            dd += 1;
        }
    }

    static void _thread(node_type* n) {
        n->_size = 1;
        for (iterator j(n->begin());  j != n->end();  ++j) {
            j->_parent = n;
            node_type* c = &*j;
            _thread(c);
            n->_size += j->_size;
        }
    }

    static void _excise(node_type* n) {
        if (n->is_root()) {
            n->tree()._root = NULL;
            n->tree()._prune(n);
        } else {
            n->parent()._children.erase(node_type::_cs_iterator(*n));
            n->parent()._prune(n);
        }
    }
};


template <typename Tree, typename Data, typename Alloc>
struct node_raw: public node_base<Tree, node_raw<Tree, Data, Alloc>, vector<node_raw<Tree, Data, Alloc>*, Alloc>, Alloc> {
    typedef Tree tree_type;
    typedef node_raw<Tree, Data, Alloc> node_type;
    typedef vector<node_type*, Alloc> cs_type;
    typedef node_base<Tree, node_type, cs_type, Alloc> base_type;
    typedef Data data_type;

    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef typename Tree::size_type size_type;
    typedef typename Tree::difference_type difference_type;
    typedef Alloc allocator_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type, Alloc>;

    node_raw() : base_type() {}
    virtual ~node_raw() {}

    node_raw(const node_raw& src) : base_type() {
        // this is to do the right then when calling allocator construct() method
        if (src._default_constructed()) return;
        // otherwise, we'd want "normal" assignment logic
        *this = src; 
    }
    node_raw& operator=(const node_raw& rhs) {
        if (this == &rhs) return *this;

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        node_type* r = const_cast<node_type*>(&rhs);
        // important if rhs is child of "this", to prevent it from getting deallocated by clear()
        bool ancestor = is_ancestor(rhs);
        if (ancestor) _excise(r);

        // in the case of vector storage, I can just leave current node where it is
        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            node_type* n = (*j)->_copy_data(this->tree());
            this->_children.push_back(n);
            _thread(n);
            this->_graft(n);
        }
        if (ancestor) this->tree()._delete_node(r);

        return *this;
    }

    void swap(node_type& b) {
        node_type& a = *this;

        if (&a == &b) return;

        // this would introduce cycles 
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw exception();

        tree_type* ta = &a.tree();
        tree_type* tb = &b.tree();
        bool ira = a.is_root();
        bool irb = b.is_root();

        node_type*& qa = (ira) ? ta->_root : *(node_type::_cs_iterator(a));
        node_type*& qb = (irb) ? tb->_root : *(node_type::_cs_iterator(b));
        node_type* ra = qa;
        node_type* rb = qb;

        node_type* pa; if (!a.is_root()) pa = a._parent;
        node_type* pb; if (!b.is_root()) pb = b._parent;

        if (ira) ta->_prune(ra);   else pa->_prune(ra);
        if (irb) tb->_prune(rb);   else pb->_prune(rb);

        qa = rb;
        qb = ra;

        if (ira) ta->_graft(rb);   else pa->_graft(rb);
        if (irb) tb->_graft(ra);   else pb->_graft(ra);
    }


    void graft(node_type& src) {
        // this would introduce cycles 
        if (this == &src) throw exception();
        if (src.is_ancestor(*this)) throw exception();

        // remove src from its current location
        node_type* s = &src;
        _excise(s);

        // graft src to current location
        this->_children.push_back(s);
        this->_graft(s);
    }

    void graft(tree_type& src) {
        if (src.empty()) return;
        graft(src.root());
    }

    // data can be non-const or const for this class
    data_type& data() { return this->_data; }
    const data_type& data() const { return this->_data; }

    node_type& operator[](size_type n) { return *(this->_children[n]); }
    const node_type& operator[](size_type n) const { return *(this->_children[n]); }

    iterator insert(const data_type& data) {
        node_type* n = this->tree()._new_node();
        n->_data = data;
        n->_depth.insert(1);
        this->_children.push_back(n);
        this->_graft(n);
        return iterator(this->_children.begin()+(this->_children.size()-1));
    }

    iterator insert(const node_type& src) {
        node_type* n = src._copy_data(this->tree());
        base_type::_thread(n);
        this->_children.push_back(n);
        this->_graft(n);
        return iterator(this->_children.begin()+(this->_children.size()-1));
    }
    iterator insert(const tree_type& src) {
        if (src.empty()) return this->end();
        return insert(src.root());
    }

    void push_back(const data_type& data) { insert(data); }
    void push_back(const node_type& src) { insert(src); }
    void push_back(const tree_type& src) { insert(src); }

    void pop_back() {
        this->tree()._delete_node(this->_children.back());
        this->_children.pop_back();
    }

    node_type& back() { return *(this->_children.back()); }
    const node_type& back() const { return *(this->_children.back()); }
    node_type& front() { return *(this->_children.front()); }
    const node_type& front() const { return *(this->_children.front()); }

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        cs_iterator j(n.parent()._children.begin());
        cs_iterator jend(n.parent()._children.end());
        for (;  j != jend;  ++j) if (*j == &n) break;
        if (j == jend) throw exception();
        return j;
    }

    node_type* _copy_data(tree_type& tree_) const {
        node_type* n = tree_._new_node();
        n->_data = this->_data;
        n->_depth = this->_depth;
        for (cs_const_iterator j(this->_children.begin()); j != this->_children.end(); ++j)
            n->_children.push_back((*j)->_copy_data(tree_));
        return n;
    }
};


template <typename Tree, typename Data, typename Compare, typename Alloc>
struct node_ordered: public node_base<Tree, node_ordered<Tree, Data, Compare, Alloc>, multiset<node_ordered<Tree, Data, Compare, Alloc>*, ptr_less_data<Compare>, Alloc>, Alloc> {
    typedef node_ordered<Tree, Data, Compare, Alloc> node_type;
    typedef Tree tree_type;
    typedef multiset<node_type*, ptr_less_data<Compare>, Alloc> cs_type;
    typedef node_base<Tree, node_type, cs_type, Alloc> base_type;
    typedef Data data_type;

    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef typename Tree::size_type size_type;
    typedef typename Tree::difference_type difference_type;
    typedef Alloc allocator_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type, Alloc>;

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    public:
    node_ordered() : base_type() {}
    virtual ~node_ordered() {}

    node_ordered(const node_ordered& src) : base_type() {
        // this is to do the right then when calling allocator construct() method
        if (src._default_constructed()) return;
        // otherwise, we'd want "normal" assignment logic
        *this = src; 
    }
    node_ordered& operator=(const node_ordered& rhs) {
        if (this == &rhs) return *this;

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        node_type* t = this;
        node_type* r = const_cast<node_type*>(&rhs);
        bool ancestor = is_ancestor(rhs);
        if (ancestor) _excise(r);

        node_type* p;
        if (!this->is_root()) {
            p = this->_parent;
            cs_iterator tt = node_type::_cs_iterator(*this);
            p->_children.erase(tt);
        }

        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            node_type* n = (*j)->_copy_data(this->tree());
            this->_children.insert(n);
            base_type::_thread(n);
            this->_graft(n);
        }
        if (ancestor) this->tree()._delete_node(r);

        if (!this->is_root()) {
            p->_children.insert(t);
        }

        return *this;
    }


    void swap(node_type& b) {
        node_type& a = *this;

        if (&a == &b) return;

        // this would introduce cycles 
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw exception();

        bool ira = a.is_root();
        bool irb = b.is_root();

        tree_type* ta = (ira) ? &a.tree() : NULL;
        tree_type* tb = (irb) ? &b.tree() : NULL;

        cs_iterator ja, jb;

        node_type* ra = (ira) ? ta->_root : const_cast<node_type*>(*(ja = node_type::_cs_iterator(a)));
        node_type* rb = (irb) ? tb->_root : const_cast<node_type*>(*(jb = node_type::_cs_iterator(b)));

        node_type* pa; if (!ira) pa = a._parent;
        node_type* pb; if (!irb) pb = b._parent;

        if (ira) ta->_prune(ra);   else { pa->_children.erase(ja);  pa->_prune(ra); }
        if (irb) tb->_prune(rb);   else { pb->_children.erase(jb);  pb->_prune(rb); }

        if (ira) { ta->_root = rb;  ta->_graft(rb); }   else { pa->_children.insert(rb);  pa->_graft(rb); }
        if (irb) { tb->_root = ra;  tb->_graft(ra); }   else { pb->_children.insert(ra);  pb->_graft(ra); }
    }


    void graft(node_type& src) {
        // this would introduce cycles 
        if (this == &src) throw exception();
        if (src.is_ancestor(*this)) throw exception();

        // remove src from its current location
        node_type* s = &src;
        _excise(s);

        // graft src to current location
        this->_children.insert(s);
        this->_graft(s);
    }

    void graft(tree_type& src) {
        if (src.empty()) return;
        graft(src.root());
    }

    // data needs to be immutable for this class, since it's the sort key, so
    // only const access allowed
    const data_type& data() const { return this->_data; }

    iterator find(const data_type& data) {
        node_type s;
        s._data = data;
        return iterator(this->_children.find(&s));
    }
    const_iterator find(const data_type& data) const {
        node_type s;
        s._data = data;
        return const_iterator(this->_children.find(&s));
    }

    size_type count(const data_type& data) const {
        node_type s;
        s._data = data;
        return this->_children.count(&s);
    }

    iterator insert(const data_type& data) {
        node_type* n = this->tree()._new_node();
        n->_data = data;
        iterator r(this->_children.insert(n));
        // insertions always happen for multiset, hence no checking
        n->_depth.insert(1);
        this->_graft(n);
        return r;
    }

    iterator insert(const node_type& src) {
        node_type* n = src._copy_data(this->tree());
        iterator r(this->_children.insert(n));
        // insertions always happen for multiset, hence no checking
        base_type::_thread(n);
        this->_graft(n);
        return r;
    }
    iterator insert(const tree_type& src) {
        if (src.empty()) return this->end();
        return insert(src.root());
    }


    protected:
    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        pair<cs_iterator, cs_iterator> r(n.parent()._children.equal_range(&n));
        if (r.first == r.second) throw exception();
        for (cs_iterator j(r.first);  j != r.second;  ++j)
            if (*j == &n) return j;
        throw exception();
        // to satisfy compiler:
        return r.first;
    }

    node_type* _copy_data(tree_type& tree_) const {
        node_type* n = tree_._new_node();
        n->_data = this->_data;
        n->_depth = this->_depth;
        for (cs_const_iterator j(this->_children.begin());  j != this->_children.end();  ++j) {
            node_type* c((*j)->_copy_data(tree_));
            n->_children.insert(c);
        }
        return n;
    }
};


template <typename Tree, typename Data, typename Key, typename Compare, typename Alloc>
struct node_keyed: public node_base<Tree, node_keyed<Tree, Data, Key, Compare, Alloc>, map<const Key*, node_keyed<Tree, Data, Key, Compare, Alloc>*, ptr_less<Compare>, Alloc>, Alloc> {
    typedef node_keyed<Tree, Data, Key, Compare, Alloc> node_type;
    typedef Tree tree_type;
    typedef map<const Key*, node_type*, ptr_less<Compare>, Alloc> cs_type;
    typedef node_base<Tree, node_type, cs_type, Alloc> base_type;
    typedef Data data_type;
    typedef Key key_type;
    typedef pair<const key_type, data_type> kv_pair;

    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef typename Tree::size_type size_type;
    typedef typename Tree::difference_type difference_type;
    typedef Alloc allocator_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type, Alloc>;

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;
    typedef typename cs_type::value_type cs_value_type;
    key_type _key;

    public:
    node_keyed() : base_type(), _key() {}
    virtual ~node_keyed() {}

    node_keyed(const node_keyed& src) : base_type(), _key() { 
        // this is to do the right then when calling allocator construct() method
        if (src._default_constructed()) return;
        // otherwise, we'd want "normal" assignment logic
        *this = src; 
    }
    node_keyed& operator=(const node_keyed& rhs) {
        if (this == &rhs) return *this;

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        node_type* r = const_cast<node_type*>(&rhs);
        bool ancestor = is_ancestor(rhs);
        if (ancestor) _excise(r);

        // I'm going to define semantics of assignment as analogous to raw:
        // the key of the LHS node does not change
        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            node_type* n = (j->second)->_copy_data(this->tree());
            this->_children.insert(cs_value_type(&(n->_key), n));
            base_type::_thread(n);
            this->_graft(n);
        }
        if (ancestor) this->tree()._delete_node(r);

        return *this;
    }

    data_type& data() { return this->_data; }
    const data_type& data() const { return this->_data; }

    // keys are const access only
    const key_type& key() const { return this->_key; }

    node_type& operator[](const key_type& key) {
        iterator f(this->find(key));
        if (this->end() == f) f = this->insert(key, data_type()).first;
        return *f;
    }
    const node_type& operator[](const key_type& key) const {
        const_iterator f(this->find(key));
        if (this->end() == f) throw exception();
        return *f;
    }

    iterator find(const key_type& key) { return iterator(this->_children.find(&key)); }
    const_iterator find(const key_type& key) const { return const_iterator(this->_children.find(&key)); }

    size_type count(const key_type& key) const {
        return this->_children.count(&key);
    }

    pair<iterator, bool> insert(const key_type& key, const data_type& data) {
        node_type* n = this->tree()._new_node();
        n->_key = key;
        pair<cs_iterator, bool> r = this->_children.insert(cs_value_type(&(n->_key), n));
        pair<iterator, bool> rr(iterator(r.first), r.second);
        if (!r.second) {
            // if we did not insert, then bail now
            this->tree()._delete_node(n);
            return rr;
        }
        // do this work if we know we actually inserted 
        n->_data = data;
        n->_depth.insert(1);
        this->_graft(n);
        return rr;
    }

    pair<iterator, bool> insert(const kv_pair& kv) { return insert(kv.first, kv.second); }

    pair<iterator, bool> insert(const key_type& key, const node_type& src) {
        node_type* n = this->tree()._new_node();
        n->_key = key;
        pair<cs_iterator, bool> r = this->_children.insert(cs_value_type(&(n->_key), n));
        pair<iterator, bool> rr(iterator(r.first), r.second);
        if (!r.second) {
            // if there was no insertion, bail now
            this->tree()._delete_node(n);
            return rr;
        }
        // if we inserted, then graft the new node in and assign from src
        n->_depth.insert(1);
        this->_graft(n);
        *n = src;
        return rr;
    }

    pair<iterator, bool> insert(const key_type& key, const tree_type& src) {
        if (src.empty()) return pair<iterator, bool>(this->end(), false);
        return insert(key, src.root());
    }

    void swap(node_type& b) {
        node_type& a = *this;

        if (&a == &b) return;

        // this would introduce cycles 
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw exception();

        bool ira = a.is_root();
        bool irb = b.is_root();

        tree_type* ta = (ira) ? &a.tree() : NULL;
        tree_type* tb = (irb) ? &b.tree() : NULL;

        cs_iterator ja, jb;

        node_type* ra = (ira) ? ta->_root : const_cast<node_type*>((ja = _cs_iterator(a))->second);
        node_type* rb = (irb) ? tb->_root : const_cast<node_type*>((jb = _cs_iterator(b))->second);

        node_type* pa; if (!ira) pa = a._parent;
        node_type* pb; if (!irb) pb = b._parent;

        if (ira) ta->_prune(ra);   else { pa->_children.erase(ja);  pa->_prune(ra); }
        if (irb) tb->_prune(rb);   else { pb->_children.erase(jb);  pb->_prune(rb); }

        // keeping analogous to "raw" semantic where keys don't change
        std::swap(ra->_key, rb->_key);

        if (ira) { ta->_root = rb;  ta->_graft(rb); }   else { pa->_children.insert(cs_value_type(&(rb->_key), rb));  pa->_graft(rb); }
        if (irb) { tb->_root = ra;  tb->_graft(ra); }   else { pb->_children.insert(cs_value_type(&(ra->_key), ra));  pb->_graft(ra); }
    }


    void graft(const key_type& key, node_type& src) {
        // this would introduce cycles 
        if (this == &src) throw exception();
        if (src.is_ancestor(*this)) throw exception();

        // remove src from its current location
        node_type* s = &src;
        _excise(s);

        // graft src to current location
        s->_key = key;
        this->_children.insert(cs_value_type(&(s->_key), s));
        this->_graft(s);
    }

    void graft(const key_type& key, tree_type& src) {
        if (src.empty()) return;
        graft(key, src.root());
    }


    protected:
    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        cs_iterator j(n.parent()._children.find(&n._key));
        if (j == n.parent()._children.end()) throw exception();
        return j;
    }

    node_type* _copy_data(tree_type& tree_) const {
        node_type* n = tree_._new_node();
        n->_data = this->_data;
        n->_key = this->_key;
        n->_depth = this->_depth;
        for (cs_const_iterator j(this->_children.begin());  j != this->_children.end();  ++j) {
            node_type* c((j->second)->_copy_data(tree_));
            n->_children.insert(cs_value_type(&(c->_key), c));
        }
        return n;
    }
};


struct node_type_dispatch_failed {};

template <typename Tree, typename CSModel>
struct node_type_dispatch {
    // catch-all should be compile error
    typedef node_type_dispatch_failed node_type;
    typedef node_type_dispatch_failed base_type;
};


template <typename Tree, typename Unused>
struct node_type_dispatch<Tree, raw<Unused> > {
    typedef node_raw<Tree, typename Tree::data_type, typename Tree::allocator_type> node_type;
    // why do I need this?  because of friend declarations, that's why.
    typedef typename node_type::base_type base_type;
};

template <typename Tree, typename Compare>
struct node_type_dispatch<Tree, ordered<Compare> > {
    typedef node_ordered<Tree, typename Tree::data_type, Compare, typename Tree::allocator_type> node_type;
    typedef typename node_type::base_type base_type;
};


template <typename Tree>
struct node_type_dispatch<Tree, ordered<arg_default> > {
    typedef node_ordered<Tree, typename Tree::data_type, less<typename Tree::data_type>, typename Tree::allocator_type> node_type;
    typedef typename node_type::base_type base_type;
};


template <typename Tree, typename Key, typename Compare>
struct node_type_dispatch<Tree, keyed<Key, Compare> > {
    typedef node_keyed<Tree, typename Tree::data_type, Key, Compare, typename Tree::allocator_type> node_type;
    typedef typename node_type::base_type base_type;
};


template <typename Data, typename CSModel=raw<>, typename Alloc=std::allocator<Data> >
struct tree {
    typedef tree<Data, CSModel, Alloc> tree_type;
    typedef Data data_type;
    typedef CSModel cs_model_type;
    typedef Alloc allocator_type;
    typedef unsigned long size_type;
    typedef long difference_type;

    typedef node_type_dispatch<tree_type, CSModel> nt_dispatch;
    typedef typename nt_dispatch::node_type node_type;
    typedef typename nt_dispatch::base_type node_base_type;

    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    // check out this wild syntax!
    typedef typename Alloc::template rebind<node_type>::other node_allocator_type;

    typedef typename node_type::bf_iterator iterator;
    typedef typename node_type::const_bf_iterator const_iterator;
    typedef typename node_type::bf_iterator bf_iterator;
    typedef typename node_type::const_bf_iterator const_bf_iterator;
    typedef typename node_type::df_post_iterator df_post_iterator;
    typedef typename node_type::const_df_post_iterator const_df_post_iterator;
    typedef typename node_type::df_pre_iterator df_pre_iterator;
    typedef typename node_type::const_df_pre_iterator const_df_pre_iterator;

    tree() : _root(NULL), _node_allocator() {}
    virtual ~tree() { clear(); }


    tree(const tree& src) { *this = src; }

    tree& operator=(const tree& src) {
        if (&src == this) return *this;

        _node_allocator = src._node_allocator;

        if (src.empty()) {
            clear();
            return *this;
        }

        if (empty()) {
            _root = _new_node();
            _root->_tree = this;
            _root->_depth.insert(1);
        }

        *_root = src.root();

        return *this;
    }


    bool empty() const { return _root == NULL; }
    size_type size() const { return (empty()) ? 0 : root().subtree_size(); }
    size_type depth() const { return (empty()) ? 0 : root().depth(); }

    node_type& root() {
        if (empty()) throw exception();
        return *_root;
    }

    const node_type& root() const {
        if (empty()) throw exception();
        return *_root;
    }

    void insert(const data_type& data) {
        clear();
        _root = _new_node();
        _root->_data = data;
        _root->_tree = this;
        _root->_depth.insert(1);
    }

    // there is only one node to erase from the tree: the root
    void erase() { clear(); }

    void clear() {
        if (empty()) return;
        _delete_node(_root);
        _root = NULL;
    }

    void swap(tree_type& src) {
        if (this == &src) return;
        std::swap(_root, src._root);
    }

    void graft(node_type& src) {
        node_type* s = &src;
        node_type::_excise(s);
        clear();
        _root = s;
        _graft(s);
    }

    void graft(tree_type& src) {
        if (src.empty()) erase();
        else graft(src.root());
    }

    void insert(const node_type& src) {
        node_type* n = src._copy_data(*this);
        node_type::base_type::_thread(n);
        clear();
        _root = n;
        _graft(n);
    }

    void insert(const tree_type& src) {
        if (src.empty()) erase();
        else insert(src.root());
    }

    bool operator==(const tree& rhs) const {
        if (size() != rhs.size()) return false;
        if (size() == 0) return true;
        return root() == rhs.root();
    }
    bool operator!=(const tree& rhs) const { return !(*this == rhs); }

    bool operator<(const tree& rhs) const {
        if (empty()) return !rhs.empty();
        if (rhs.empty()) return false;
        return root() < rhs.root();
    }
    bool operator>(const tree& rhs) const { return rhs < *this; }
    bool operator<=(const tree& rhs) const { return !(rhs < *this); }
    bool operator>=(const tree& rhs) const { return !(*this < rhs); }

    iterator begin() { return iterator(_root); }
    iterator end() { return iterator(); }
    const_iterator begin() const { return const_iterator(_root); }
    const_iterator end() const { return const_iterator(); }

    bf_iterator bf_begin() { return bf_iterator(_root); }
    bf_iterator bf_end() { return bf_iterator(); }
    const_bf_iterator bf_begin() const { return const_bf_iterator(_root); }
    const_bf_iterator bf_end() const { return const_bf_iterator(); }

    df_post_iterator df_post_begin() { return df_post_iterator(_root); }
    df_post_iterator df_post_end() { return df_post_iterator(); }
    const_df_post_iterator df_post_begin() const { return const_df_post_iterator(_root); }
    const_df_post_iterator df_post_end() const { return const_df_post_iterator(); }

    df_pre_iterator df_pre_begin() { return df_pre_iterator(_root); }
    df_pre_iterator df_pre_end() { return df_pre_iterator(); }
    const_df_pre_iterator df_pre_begin() const { return const_df_pre_iterator(_root); }
    const_df_pre_iterator df_pre_end() const { return const_df_pre_iterator(); }

    // compile will not let me use just node_type here
    friend class node_type_dispatch<tree_type, CSModel>::node_type;
    friend class node_type_dispatch<tree_type, CSModel>::base_type;

    protected:
    node_type* _root;
    node_allocator_type _node_allocator;
    static const node_type _node_init_val;

    node_type* _new_node() {
        node_type* n = _node_allocator.allocate(1);
        _node_allocator.construct(n, _node_init_val);
        return n;
    }

    void _delete_node(node_type* n) {
        _node_allocator.destroy(n);
        _node_allocator.deallocate(n, 1);
    }

    void _prune(node_type* n) {
    }

    void _graft(node_type* n) {
        n->_parent = NULL;
        n->_tree = this;
    }
};


template <typename Data, typename CSModel, typename Alloc>
const typename tree<Data, CSModel, Alloc>::node_type tree<Data, CSModel, Alloc>::_node_init_val;


}  // namespace ootree


namespace std {

template <typename Data, typename CSModel, typename Alloc>
void swap(ootree::tree<Data, CSModel, Alloc>& a, ootree::tree<Data, CSModel, Alloc>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Alloc>
void swap(ootree::node_raw<Tree, Data, Alloc>& a, ootree::node_raw<Tree, Data, Alloc>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Compare, typename Alloc>
void swap(ootree::node_ordered<Tree, Data, Compare, Alloc>& a, ootree::node_ordered<Tree, Data, Compare, Alloc>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Key, typename Compare, typename Alloc>
void swap(ootree::node_keyed<Tree, Data, Key, Compare, Alloc>& a, ootree::node_keyed<Tree, Data, Key, Compare, Alloc>& b) {
    a.swap(b);
}

}  // namespace std

#endif  // __ootree_h__
