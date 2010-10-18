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

    node_base() : _tree(NULL), _ply(0), _size(1), _parent(), _this(), _data(), _children() {}
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

    void clear() {
        erase(begin(), end());
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

    void _prune(shared_ptr<node_type>& n) {
        // percolate the new subtree size up the chain of parents
        shared_ptr<node_type> q = _this.lock();
        while (true) {
            q->_size -= n->_size;
            if (q->ply() == 0) {
                _tree->_size -= n->_size;
                break;
            }
            q = q->_parent.lock();
        }

        // erase subtree depth info from parent tree
        deque<shared_ptr<node_type> > nq;
        nq.push_back(n);
        while (!nq.empty()) {
            q = nq.front();
            nq.pop_front();
            _tree->_depth.erase(1 + q->_ply);
            for (cs_iterator j(q->_children.begin());  j != q->_children.end();  ++j) nq.push_back(*j);
        }
    }

    void _graft(shared_ptr<node_type>& n) {
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

    void _thread(shared_ptr<node_type>& n) {
        n->_this = n;
        n->_size = 1;
        for (cs_iterator j(n->_children.begin());  j !=  n->_children.end();  ++j) {
            (*j)->_parent = n;
            _thread(*j);
            n->_size += (*j)->_size;
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
        // in the case of vector storage, I can just leave current node where it is
        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(rhs._children.begin());  j != rhs._children.end();  ++j) {
            shared_ptr<node_type> n((*j)->_copy_data());
            this->_children.push_back(n);
            this->_thread(n);
            this->_graft(n);
        }
        return *this;
    }

    node_type& operator[](size_type n) { return *(this->_children[n]); }
    const node_type& operator[](size_type n) const { return *(this->_children[n]); }

    node_type& operator/(size_type n) { return *(this->_children[n]); }
    const node_type& operator/(size_type n) const { return *(this->_children[n]); }

    void insert(const data_type& data) {
        shared_ptr<node_type> n(new node_type);
        n->_data = data;
        n->_this = n;
        n->_size = 1;
        this->_children.push_back(n);
        this->_graft(n);
    }

    protected:
    typedef typename base_type::cs_iterator cs_iterator;
    typedef typename base_type::cs_const_iterator cs_const_iterator;

    cs_iterator _cs_iterator() {
        if (this->is_root()) throw exception();
        cs_iterator j(this->parent()._children.begin());
        cs_iterator jend(this->parent()._children.end());
        for (;  j != jend;  ++j) if (*j == this) break;
        if (j == jend) throw exception();
        return j;
    }

    iterator _iterator() { return iterator(_cs_iterator()); }

    shared_ptr<node_type> _copy_data() {
        shared_ptr<node_type> n(new node_type);
        n->_data = this->_data;
        for (cs_iterator j(this->_children.begin()); j != this->_children.end(); ++j)
            n->_children.push_back((*j)->_copy_data());
        return n;
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

    typedef b1st_iterator<node_type> bf_iterator;
    typedef d1st_post_iterator<node_type> df_post_iterator;
    typedef d1st_pre_iterator<node_type> df_pre_iterator;

    tree() : _size(0), _root(), _depth() {}
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
            _size = 1;
            _depth.insert(1);
        }

        *_root = src.root();

        return *this;
    }


    size_type size() const { return _size; }
    bool empty() const { return 0 == size(); }

    size_type depth() const { return _depth.max(); }

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
        _size = 1;
        _depth.insert(1);
    }

    // there is only one node to erase from the tree: the root
    void erase() { clear(); }

    void clear() {
        if (empty()) return;
        _root.reset();
        _depth.clear();
        _size = 0;
    }

    void swap(this_type& src) {
        if (this == &src) return;
        _root.swap(src._root);
        std::swap(_size, src._size);
        _depth.swap(src._depth);
    }

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
    size_type _size;
    max_maintainer<size_type> _depth;
};


};  // namespace ootree


namespace std {

template <typename Data, typename CSCat>
void swap(ootree::tree<Data, CSCat>& a, ootree::tree<Data, CSCat>& b) {
    a.swap(b);
}

};  // namespace ootree

#endif  // __ootree_h__
