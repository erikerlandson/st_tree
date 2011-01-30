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
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iostream>
#include "stdio.h"

namespace ootree {
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::dynamic_pointer_cast;
using std::tr1::const_pointer_cast;
using std::vector;
using std::deque;
using std::set;
using std::multiset;
using std::map;
using std::less;
using std::pair;
using std::cerr;


// Node storage classes:
// use this if you want raw (vector) unordered child node storage
struct raw {};
// provides ordered child node storage
struct ordered {};
// child nodes are indexed by external key
struct keyed {};


// some data types to flag template argument behaviors
struct arg_default {};
struct arg_unused {};
struct arg_void {};


struct exception {
    exception() {}
    virtual ~exception() {}
// stub.  This will need some work.
};


template <typename Unsigned>
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
    vector<Unsigned> _hist;
};


template <typename X>
struct dref_vmap {
    // X is a type that is de-referenceable: supports the unary "*" dereference operator 
    typedef typeof(*(X())) drX;

    // These both return a non-const reference -- this enables me to handle
    // (multi)set iterators in a useful way below: those iterators are always const,
    // however I need non-const behavior for many ops.  It is only the data field that
    // must remain const, because it is the true sorting key
    drX& operator()(X& x) const { return *x; }
    drX& operator()(const X& x) const { return const_cast<drX&>(*x); }

    bool operator==(const dref_vmap& rhs) const { return true; }
    bool operator!=(const dref_vmap& rhs) const { return false; }
};


template <typename X>
struct dref_second_vmap {
    // X is a type that is de-referenceable: supports the unary "*" dereference operator 
    typedef typeof(*(X())) drX;
    typedef typeof(drX().second) R;

    // Set these both to return non-const reference (see dref_vmap comment above)
    R& operator()(X& x) const { return x->second; }
    R& operator()(const X& x) const { return x->second; }

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

    typedef Iterator base_iterator;

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


template <typename Node>
struct b1st_iterator {
    typedef Node node_type;
    typedef typename node_type::iterator::base_iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef node_type value_type;
    typedef typename iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    b1st_iterator() : _queue() {}
    virtual ~b1st_iterator() {}

    b1st_iterator(const b1st_iterator& rhs) : _queue(rhs._queue) {}
    b1st_iterator& operator=(const b1st_iterator& rhs) { _queue = rhs._queue; }

    b1st_iterator(const shared_ptr<node_type>& root) {
        if (root == NULL) return;
        _queue.push_back(root);
    }

    reference operator*() const { return *(_queue.front()); }
    pointer operator->() const { return _queue.front().get(); }

    // pre-increment iterator
    b1st_iterator operator++() {
        // if we are already past the end of elements in tree, then this is a no-op
        if (_queue.empty()) return *this;
        
        // take current node off front of the queue
        shared_ptr<node_type> f(_queue.front());
        _queue.pop_front();

        if (f->empty()) return *this;
        for (iterator j(f->begin());  j != iterator(f->end());  ++j)
            _queue.push_back(*j);

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
    deque<shared_ptr<node_type> > _queue;
};


template <typename Node>
struct d1st_post_iterator {
    typedef Node node_type;
    typedef typename node_type::iterator::base_iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef node_type value_type;
    typedef typename iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    struct frame {
        frame(): first(), second(), visited() {}
        virtual ~frame() {}
        frame(const frame& rhs): first(rhs.first), second(rhs.second), visited(rhs.visited) {}
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; }
        frame(const shared_ptr<node_type>& first_, const iterator& second_, const bool& visited_) {
            first = first_;  second = second_;  visited = visited_;
        }
        bool operator==(const frame& rhs) const {
            if (first != rhs.first) return false;
            if (second != rhs.second) return false;
            if (visited != rhs.visited) return false;
            return true;
        }
        bool operator!=(const frame& rhs) const { return !(*this == rhs); }
        shared_ptr<node_type> first;
        iterator second;
        bool visited;
    };

    d1st_post_iterator() : _stack() {}
    virtual ~d1st_post_iterator() {}

    d1st_post_iterator(const d1st_post_iterator& rhs) : _stack(rhs._stack) { }
    d1st_post_iterator& operator=(const d1st_post_iterator& rhs) { _stack = rhs._stack; }

    d1st_post_iterator(const shared_ptr<node_type>& root) {
        if (root == NULL) return;
        _stack.push_back(frame(root, iterator(root->begin()), false));
        while (true) {
            if (_stack.back().first->empty()) {
                _stack.back().visited = true;
                break;
            }
            iterator b(_stack.back().first->begin());
            _stack.push_back(frame(*b, iterator((*b)->begin()), false));
        }
    }

    reference operator*() const { return *(_stack.back().first); }
    pointer operator->() const { return _stack.back().first.get(); }

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
        _stack.push_back(frame(*(_stack.back().second), iterator((*_stack.back().second)->begin()), false));
        while (true) {
            if (_stack.back().first->empty()) {
                _stack.back().visited = true;
                break;
            }
            iterator b(_stack.back().first->begin());
            _stack.push_back(frame(*b, iterator((*b)->begin()), false));                
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
    vector<frame> _stack;
};


template <typename Node>
struct d1st_pre_iterator {
    typedef Node node_type;
    typedef typename node_type::iterator::base_iterator iterator;

    typedef std::forward_iterator_tag iterator_category;
    typedef node_type value_type;
    typedef typename iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    struct frame {
        frame(): first(), second(), visited() {}
        virtual ~frame() {}
        frame(const frame& rhs): first(rhs.first), second(rhs.second), visited(rhs.visited) {}
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; }
        frame(const shared_ptr<node_type>& first_, const iterator& second_, const bool& visited_) {
            first = first_;  second = second_;  visited = visited_;
        }
        bool operator==(const frame& rhs) const {
            if (first != rhs.first) return false;
            if (second != rhs.second) return false;
            if (visited != rhs.visited) return false;
            return true;
        }
        bool operator!=(const frame& rhs) const { return !(*this == rhs); }
        shared_ptr<node_type> first;
        iterator second;
        bool visited;
    };

    d1st_pre_iterator() : _stack() {}
    virtual ~d1st_pre_iterator() {}

    d1st_pre_iterator(const d1st_pre_iterator& rhs) : _stack(rhs._stack) { }
    d1st_pre_iterator& operator=(const d1st_pre_iterator& rhs) { _stack = rhs._stack; }

    d1st_pre_iterator(const shared_ptr<node_type>& root) {
        if (root == NULL) return;
        _stack.push_back(frame(root, iterator(root->begin()), false));
    }

    reference operator*() const { return *(_stack.back().first); }
    pointer operator->() const { return _stack.back().first.get(); }

    // pre-increment
    d1st_pre_iterator operator++() {
        // if we are already past the end of elements in tree, then this is a no-op
        if (_stack.empty()) return *this;

        // check the case where this is node we have just visited pre-order
        if (!_stack.back().visited) {
            _stack.back().visited = true;
            if (!_stack.back().first->empty()) {
                _stack.push_back(frame(*(_stack.back().second), iterator((*(_stack.back().second))->begin()), false));
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
        _stack.push_back(frame(*(_stack.back().second), iterator((*(_stack.back().second))->begin()), false));

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
    vector<frame> _stack;
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
struct cscat_dispatch<Data, cscat<keyed, Key, arg_default> > {
    typedef cscat<keyed, Key, less<Key> > cat;
};

template <typename Data, typename Key, typename KeyComp>
struct cscat_dispatch<Data, cscat<keyed, Key, KeyComp> > {
    typedef cscat<keyed, Key, KeyComp > cat;
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


struct dereferenceable_lessthan {
    template <typename D>
    bool operator()(const D& a, const D& b) const { return *a < *b; }
};


template <typename Tree, typename Node, typename ChildContainer>
struct node_base {
    typedef node_base<Tree, Node, ChildContainer> this_type;
    typedef Tree tree_type;
    typedef Node node_type;
    typedef ChildContainer cs_type;
    typedef typename tree_type::size_type size_type;
    typedef typename tree_type::data_type data_type;

    node_base() : _tree(NULL), _size(1), _parent(), _this(), _data(), _children() {}
    virtual ~node_base() {}

    size_type ply() const {
        size_type p = 0;
        shared_ptr<node_type> q(_this.lock());
        while (!q->is_root()) {
            q = q->_parent.lock();
            p += 1;
        }
        return p;
    }

    tree_type& tree() {
        shared_ptr<node_type> q(_this.lock());
        while (!q->is_root()) {
            q = q->_parent.lock();
        }
        return *(q->_tree);
    }

    const tree_type& tree() const { 
        shared_ptr<const node_type> q(_this.lock());
        while (!q->is_root()) {
            q = q->_parent.lock();
        }
        return *(q->_tree);
    }

    size_type depth() const { return _depth.max(); }
    size_type subtree_size() const { return _size; }

    bool is_root() const { return _tree != NULL; }

    bool is_ancestor(const node_type& n) const {
        shared_ptr<node_type> a(_this.lock());
        shared_ptr<node_type> q(n._this.lock());
        while (true) {
            if (q->is_root()) return false;
            q = q->_parent.lock();
            if (q == a) return true;
        }
        return false;
    }

    node_type& parent() {
        if (is_root()) throw exception();
        return *(_parent.lock());
    }
    const node_type& parent() const {
        if (is_root()) throw exception();
        return *(_parent.lock()); 
    }

    protected:
    typedef typename cs_type::iterator cs_iterator;
    typedef typename cs_type::const_iterator cs_const_iterator;
    
    public:
    typedef valmap_iterator_adaptor<cs_iterator, dref_vmap<typename cs_iterator::value_type> > iterator;
    typedef valmap_iterator_adaptor<cs_const_iterator, dref_vmap<typename cs_const_iterator::value_type> > const_iterator;

    iterator begin() { return iterator(_children.begin()); }
    iterator end() { return iterator(_children.end()); }

    size_type size() const { return _children.size(); }
    bool empty() const { return _children.empty(); }

    void erase(const iterator& j) {
        cs_iterator csj(j);
        shared_ptr<node_type> n((*csj)->_this.lock());
        _prune(n);
        _children.erase(csj);
    }

    void erase(const iterator& F, const iterator& L) {
        cs_iterator csF(F);
        cs_iterator csL(L);
        for (cs_iterator j(csF);  j != csL;  ++j) {
            shared_ptr<node_type> n((*j)->_this.lock());
            _prune(n);
        }
        _children.erase(csF, csL);
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
        for (cs_const_iterator jL(_children.begin()), jR(rhs._children.begin());  jL != _children.end();  ++jL,++jR)
            if (**jL != **jR) return false;
        return true;
    }
    bool operator!=(const node_base& rhs) const { return !(*this == rhs); }

    bool operator<(const node_base& rhs) const {
        if (this == &rhs) return false;
        if (_data != rhs._data) return (_data < rhs._data);
        dereferenceable_lessthan lt;
        return std::lexicographical_compare(_children.begin(), _children.end(), rhs._children.begin(), rhs._children.end(), lt);
    }
    bool operator>(const node_base& rhs) const { return rhs < *this; }
    bool operator<=(const node_base& rhs) const { return !(rhs < *this); }
    bool operator>=(const node_base& rhs) const { return !(*this < rhs); }

    friend class tree_type::tree_type;
    protected:
    tree_type* _tree;
    size_type _size;
    max_maintainer<size_type> _depth;
    weak_ptr<node_type> _parent;
    weak_ptr<node_type> _this;
    data_type _data;
    cs_type _children;

    iterator _iterator() { return iterator(node_type::_cs_iterator(*static_cast<node_type*>(this))); }

    void _prune(shared_ptr<node_type>& n) {
        // percolate the new subtree size up the chain of parents
        shared_ptr<node_type> q = _this.lock();
        size_type dd = 1;
        while (true) {
            q->_size -= n->_size;
            q->_depth.erase(n->_depth, dd);
            if (q->is_root()) {
                break;
            }
            q = q->_parent.lock();
            dd += 1;
        }
    }

    void _graft(shared_ptr<node_type>& n) {
        // set new parent for this subtree as current node
        shared_ptr<node_type> q = _this.lock();
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
            q = q->_parent.lock();
            dd += 1;
        }
    }

    static void _thread(shared_ptr<node_type>& n) {
        n->_size = 1;
        for (iterator j(n->begin());  j != n->end();  ++j) {
            j->_parent = n;
            shared_ptr<node_type> c = j->_this.lock();
            _thread(c);
            n->_size += j->_size;
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
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type>;

    node_raw() : base_type() {}
    virtual ~node_raw() {}

    node_raw(const node_raw& src) { *this = src; }
    node_raw& operator=(const node_raw& rhs) {
        if (this == &rhs) return *this;
        
        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        // important if rhs is child of "this", to prevent it from getting deallocated by clear()
        shared_ptr<node_type> r(rhs._this.lock());

        // in the case of vector storage, I can just leave current node where it is
        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            shared_ptr<node_type> n((*j)->_copy_data());
            this->_children.push_back(n);
            base_type::_thread(n);
            this->_graft(n);
        }
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

        shared_ptr<node_type>& qa = (ira) ? ta->_root : *(node_type::_cs_iterator(a));
        shared_ptr<node_type>& qb = (irb) ? tb->_root : *(node_type::_cs_iterator(b));
        shared_ptr<node_type> ra = qa;
        shared_ptr<node_type> rb = qb;

        shared_ptr<node_type> pa; if (!a.is_root()) pa = a._parent.lock();
        shared_ptr<node_type> pb; if (!b.is_root()) pb = b._parent.lock();

        if (ira) ta->_prune(ra);   else pa->_prune(ra);
        if (irb) tb->_prune(rb);   else pb->_prune(rb);

        qa = rb;
        qb = ra;

        if (ira) ta->_graft(rb);   else pa->_graft(rb);
        if (irb) tb->_graft(ra);   else pb->_graft(ra);
    }


    void graft(node_type& b) {
        node_type& a = *this;

        // this would introduce cycles 
        if (&a == &b) throw exception();
        if (b.is_ancestor(a)) throw exception();

        // remove b from its current location
        shared_ptr<node_type> rb = b._this.lock();
        b.erase();

        // graft b to current location
        a._children.push_back(rb);
        a._graft(rb);
    }

    void graft(tree_type& b) {
        if (b.empty()) return;
        graft(b.root());
    }

    // data can be non-const or const for this class
    data_type& data() { return this->_data; }
    const data_type& data() const { return this->_data; }

    node_type& operator[](size_type n) { return *(this->_children[n]); }
    const node_type& operator[](size_type n) const { return *(this->_children[n]); }

    iterator insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_this = n;
        n->_size = 1;
        n->_depth.insert(1);
        this->_children.push_back(n);
        this->_graft(n);
        return iterator(this->_children.begin()+(this->_children.size()-1));
    }

    iterator insert(const node_type& b) {
        shared_ptr<node_type> n(b._copy_data());
        base_type::_thread(n);
        this->_children.push_back(n);
        this->_graft(n);
        return iterator(this->_children.begin()+(this->_children.size()-1));
    }
    iterator insert(const tree_type& b) {
        if (b.empty()) return this->end();
        return insert(b.root());
    }

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        cs_iterator j(n.parent()._children.begin());
        cs_iterator jend(n.parent()._children.end());
        for (;  j != jend;  ++j) if (j->get() == &n) break;
        if (j == jend) throw exception();
        return j;
    }

    shared_ptr<node_type> _copy_data() const {
        shared_ptr<node_type> n(new node_type);
        n->_this = n;
        n->_data = this->_data;
        n->_depth = this->_depth;
        n->_this = n;
        for (cs_const_iterator j(this->_children.begin()); j != this->_children.end(); ++j)
            n->_children.push_back((*j)->_copy_data());
        return n;
    }
};


template <typename Tree, typename Data, typename Compare>
struct node_ordered: public node_base<Tree, node_ordered<Tree, Data, Compare>, multiset<shared_ptr<node_ordered<Tree, Data, Compare> >, ptr_less_data<Compare> > > {
    typedef node_ordered<Tree, Data, Compare> this_type;
    typedef this_type node_type;
    typedef Tree tree_type;
    typedef multiset<shared_ptr<node_type>, ptr_less_data<Compare> > cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef typename Tree::size_type size_type;
    typedef Data data_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type>;

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    public:
    node_ordered() : base_type() {}
    virtual ~node_ordered() {}

    node_ordered(const node_ordered& src) { *this = src; }
    node_ordered& operator=(const node_ordered& rhs) {
        if (this == &rhs) return *this;

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        shared_ptr<node_type> t(this->_this.lock());
        shared_ptr<node_type> r(rhs._this.lock());

        shared_ptr<node_type> p;
        if (!this->is_root()) {
            p = this->_parent.lock();
            cs_iterator tt = node_type::_cs_iterator(*this);
            p->_children.erase(tt);
        }

        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            shared_ptr<node_type> n((*j)->_copy_data());
            this->_children.insert(n);
            base_type::_thread(n);
            this->_graft(n);
        }

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

        shared_ptr<node_type> ra = (ira) ? ta->_root : const_pointer_cast<node_type>(*(ja = node_type::_cs_iterator(a)));
        shared_ptr<node_type> rb = (irb) ? tb->_root : const_pointer_cast<node_type>(*(jb = node_type::_cs_iterator(b)));

        shared_ptr<node_type> pa; if (!ira) pa = a._parent.lock();
        shared_ptr<node_type> pb; if (!irb) pb = b._parent.lock();

        if (ira) ta->_prune(ra);   else { pa->_children.erase(ja);  pa->_prune(ra); }
        if (irb) tb->_prune(rb);   else { pb->_children.erase(jb);  pb->_prune(rb); }

        if (ira) { ta->_root = rb;  ta->_graft(rb); }   else { pa->_children.insert(rb);  pa->_graft(rb); }
        if (irb) { tb->_root = ra;  tb->_graft(ra); }   else { pb->_children.insert(ra);  pb->_graft(ra); }
    }


    void graft(node_type& b) {
        node_type& a = *this;

        // this would introduce cycles 
        if (&a == &b) throw exception();
        if (b.is_ancestor(a)) throw exception();

        // remove b from its current location
        shared_ptr<node_type> rb = b._this.lock();
        b.erase();

        // graft b to current location
        a._children.insert(rb);
        a._graft(rb);
    }

    void graft(tree_type& b) {
        if (b.empty()) return;
        graft(b.root());
    }

    // data needs to be immutable for this class, since it's the sort key, so
    // only const access allowed
    const data_type& data() const { return this->_data; }

    iterator insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_this = n;
        n->_size = 1;
        n->_depth.insert(1);
        cs_iterator r = this->_children.insert(n);
        this->_graft(n);
        return iterator(r);
    }

    void insert(const node_type& b) {
        shared_ptr<node_type> n(b._copy_data());
        base_type::_thread(n);
        this->_children.insert(n);
        this->_graft(n);
    }
    void insert(const tree_type& b) {
        if (b.empty()) return;
        insert(b.root());
    }


    protected:
    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        pair<cs_iterator, cs_iterator> r(n.parent()._children.equal_range(n._this.lock()));
        if (r.first == r.second) throw exception();
        for (cs_iterator j(r.first);  j != r.second;  ++j)
            if (*j == n._this.lock()) return j;
        throw exception();
        // to satisfy compiler:
        return r.first;
    }

    shared_ptr<node_type> _copy_data() const {
        shared_ptr<node_type> n(new node_type);
        n->_this = n;
        n->_data = this->_data;
        n->_depth = this->_depth;
        for (cs_const_iterator j(this->_children.begin());  j != this->_children.end();  ++j) {
            shared_ptr<node_type> c((*j)->_copy_data());
            n->_children.insert(c);
        }
        return n;
    }
};


template <typename Tree, typename Data, typename Key, typename Compare>
struct node_keyed: public node_base<Tree, node_keyed<Tree, Data, Key, Compare>, map<const Key*, shared_ptr<node_keyed<Tree, Data, Key, Compare> >, ptr_less<Compare> > > {
    typedef node_keyed<Tree, Data, Key, Compare> this_type;
    typedef this_type node_type;
    typedef Tree tree_type;
    typedef map<const Key*, shared_ptr<node_type>, ptr_less<Compare> > cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef typename Tree::size_type size_type;
    typedef Data data_type;
    typedef Key key_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend class tree_type::tree_type;
    friend class node_base<Tree, node_type, cs_type>;

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    public:
    node_keyed() : base_type() {}
    virtual ~node_keyed() {}

#if 0
    node_ordered(const node_ordered& src) { *this = src; }
    node_ordered& operator=(const node_ordered& rhs) {
        if (this == &rhs) return *this;

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw exception();

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        shared_ptr<node_type> t(this->_this.lock());
        shared_ptr<node_type> r(rhs._this.lock());

        shared_ptr<node_type> p;
        if (!this->is_root()) {
            p = this->_parent.lock();
            cs_iterator tt = node_type::_cs_iterator(*this);
            p->_children.erase(tt);
        }

        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            shared_ptr<node_type> n((*j)->_copy_data());
            this->_children.insert(n);
            base_type::_thread(n);
            this->_graft(n);
        }

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

        shared_ptr<node_type> ra = (ira) ? ta->_root : const_pointer_cast<node_type>(*(ja = node_type::_cs_iterator(a)));
        shared_ptr<node_type> rb = (irb) ? tb->_root : const_pointer_cast<node_type>(*(jb = node_type::_cs_iterator(b)));

        shared_ptr<node_type> pa; if (!ira) pa = a._parent.lock();
        shared_ptr<node_type> pb; if (!irb) pb = b._parent.lock();

        if (ira) ta->_prune(ra);   else { pa->_children.erase(ja);  pa->_prune(ra); }
        if (irb) tb->_prune(rb);   else { pb->_children.erase(jb);  pb->_prune(rb); }

        if (ira) { ta->_root = rb;  ta->_graft(rb); }   else { pa->_children.insert(rb);  pa->_graft(rb); }
        if (irb) { tb->_root = ra;  tb->_graft(ra); }   else { pb->_children.insert(ra);  pb->_graft(ra); }
    }


    void graft(node_type& b) {
        node_type& a = *this;

        // this would introduce cycles 
        if (&a == &b) throw exception();
        if (b.is_ancestor(a)) throw exception();

        // remove b from its current location
        shared_ptr<node_type> rb = b._this.lock();
        b.erase();

        // graft b to current location
        a._children.insert(rb);
        a._graft(rb);
    }

    void graft(tree_type& b) {
        if (b.empty()) return;
        graft(b.root());
    }

    data_type& data() { return this->_data; }
    const data_type& data() const { return this->_data; }

    iterator insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_this = n;
        n->_size = 1;
        n->_depth.insert(1);
        cs_iterator r = this->_children.insert(n);
        this->_graft(n);
        return iterator(r);
    }

    void insert(const node_type& b) {
        shared_ptr<node_type> n(b._copy_data());
        base_type::_thread(n);
        this->_children.insert(n);
        this->_graft(n);
    }
    void insert(const tree_type& b) {
        if (b.empty()) return;
        insert(b.root());
    }


    protected:
    static cs_iterator _cs_iterator(node_type& n) {
        if (n.is_root()) throw exception();
        pair<cs_iterator, cs_iterator> r(n.parent()._children.equal_range(n._this.lock()));
        if (r.first == r.second) throw exception();
        for (cs_iterator j(r.first);  j != r.second;  ++j)
            if (*j == n._this.lock()) return j;
        throw exception();
        // to satisfy compiler:
        return r.first;
    }

    shared_ptr<node_type> _copy_data() const {
        shared_ptr<node_type> n(new node_type);
        n->_this = n;
        n->_data = this->_data;
        n->_depth = this->_depth;
        for (cs_const_iterator j(this->_children.begin());  j != this->_children.end();  ++j) {
            shared_ptr<node_type> c((*j)->_copy_data());
            n->_children.insert(c);
        }
        return n;
    }
#endif
};


struct node_type_dispatch_failed {};

template <typename Tree, typename CSCat>
struct node_type_dispatch {
    // catch-all should be compile error
    typedef node_type_dispatch_failed node_type;
    typedef node_type_dispatch_failed base_type;
};


template <typename Tree>
struct node_type_dispatch<Tree, cscat<raw, arg_unused, arg_unused> > {
    typedef node_raw<Tree, typename Tree::data_type> node_type;
    // why do I need this?  because of friend declarations, that's why.
    typedef typename node_type::base_type base_type;
};

template <typename Tree, typename Compare>
struct node_type_dispatch<Tree, cscat<ordered, Compare, arg_unused> > {
    typedef node_ordered<Tree, typename Tree::data_type, Compare> node_type;
    typedef typename node_type::base_type base_type;
};


template <typename Tree, typename Key, typename Compare>
struct node_type_dispatch<Tree, cscat<keyed, Key, Compare> > {
    typedef node_keyed<Tree, typename Tree::data_type, Key, Compare> node_type;
    typedef typename node_type::base_type base_type;
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
    typedef typename nt_dispatch::base_type node_base_type;

    typedef b1st_iterator<node_type> bf_iterator;
    typedef d1st_post_iterator<node_type> df_post_iterator;
    typedef d1st_pre_iterator<node_type> df_pre_iterator;

    tree() : _root() {}
    virtual ~tree() { clear(); }


    tree(const tree& src) { *this = src; }

    tree& operator=(const tree& src) {
        if (&src == this) return *this;

        if (src.empty()) {
            clear();
            return *this;
        }

        if (empty()) {
            _root.reset(new node_type);
            _root->_tree = this;
            _root->_this = _root;
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
        _root.reset(new node_type);
        _root->_data = data;
        _root->_tree = this;
        _root->_this = _root;
        _root->_depth.insert(1);
    }

    // there is only one node to erase from the tree: the root
    void erase() { clear(); }

    void clear() {
        if (empty()) return;
        _root.reset();
    }

    void swap(this_type& src) {
        if (this == &src) return;
        _root.swap(src._root);
    }

    void graft(node_type& b) {
        shared_ptr<node_type> rb = b._this.lock();
        b.erase();
        _root = rb;
        _graft(rb);
    }

    void graft(tree_type& b) {
        if (b.empty()) erase();
        else           graft(b.root());
    }

    void insert(const node_type& b) {
        shared_ptr<node_type> n(b._copy_data());
        node_type::base_type::_thread(n);
        _root = n;
        _graft(n);
    }

    void insert(tree_type& b) {
        if (b.empty()) erase();
        else           insert(b.root());
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

    bf_iterator bf_begin() { return bf_iterator(_root); }
    bf_iterator bf_end() { return bf_iterator(); }

    df_post_iterator df_post_begin() { return df_post_iterator(_root); }
    df_post_iterator df_post_end() { return df_post_iterator(); }

    df_pre_iterator df_pre_begin() { return df_pre_iterator(_root); }
    df_pre_iterator df_pre_end() { return df_pre_iterator(); }

    // compile will not let me use just node_type here
    friend class node_type_dispatch<tree_type, cscat>::node_type;
    friend class node_type_dispatch<tree_type, cscat>::base_type;

    protected:
    shared_ptr<node_type> _root;

    void _prune(shared_ptr<node_type>& n) {
    }

    void _graft(shared_ptr<node_type>& n) {
        n->_parent.reset();
        n->_tree = this;
    }
};


};  // namespace ootree


namespace std {

template <typename Data, typename CSCat>
void swap(ootree::tree<Data, CSCat>& a, ootree::tree<Data, CSCat>& b) {
    a.swap(b);
}

template <typename Tree, typename Data>
void swap(ootree::node_raw<Tree, Data>&a, ootree::node_raw<Tree, Data>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Compare>
void swap(ootree::node_ordered<Tree, Data, Compare>&a, ootree::node_ordered<Tree, Data, Compare>& b) {
    a.swap(b);
}

};  // namespace ootree

#endif  // __ootree_h__
