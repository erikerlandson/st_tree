/******
st_tree: A highly configurable C++ template tree class, using STL style interfaces.

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

#if !defined(__st_tree_detail_h__)
#define __st_tree_detail_h__ 1

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <functional>
#include <algorithm>
#include <iterator>

namespace st_tree {

// forward declarations
template <typename Data, typename CSModel=raw<>, typename Alloc=std::allocator<Data> > struct tree;

namespace detail {

using std::vector;
using std::deque;
using std::multiset;
using std::map;
using std::less;
using std::pair;

// ptrdiff_t no longer working
typedef vector<int>::difference_type difference_type;

// forward declarations
template <typename Tree, typename Data> struct node_raw;
template <typename Tree, typename Data, typename Compare> struct node_ordered;
template <typename Tree, typename Data, typename Key, typename Compare> struct node_keyed;

template <typename Unsigned, typename Alloc>
struct max_maintainer {
    typedef size_t size_type;

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
    typedef typename Alloc::template rebind<Unsigned>::other unsigned_allocator;
    Unsigned _max;
    vector<Unsigned, unsigned_allocator> _hist;
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
    typedef Iterator base_iterator_type;
    typedef ValMap valmap;

    base_iterator_type _base;
    valmap _vmap;

    public:
    typedef std::forward_iterator_tag iterator_category;
    typedef typename ValMap::value_type value_type;
    typedef typename base_iterator_type::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

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

    // casting
    base_iterator_type base() const { return this->_base; }
    valmap_iterator_adaptor_forward(const base_iterator_type& src): _base(src), _vmap() {}
    valmap_iterator_adaptor_forward(const base_iterator_type& src, const valmap& vmap): _base(src), _vmap(vmap) {}
    valmap_iterator_adaptor_forward& operator=(const base_iterator_type& rhs) {
        _base = rhs;
        return *this;
    }

    // pre-increment:
    valmap_iterator_adaptor_forward& operator++() {
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
struct valmap_iterator_adaptor_random {
    typedef Iterator base_iterator_type;
    typedef ValMap valmap_type;

    typedef std::random_access_iterator_tag iterator_category;
    typedef typename ValMap::value_type value_type;
    typedef typename base_iterator_type::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    protected:
    base_iterator_type _base;
    valmap_type _vmap;

    public:
    valmap_iterator_adaptor_random() : _base(), _vmap() {}
    virtual ~valmap_iterator_adaptor_random() {}
    
    valmap_iterator_adaptor_random(const valmap_iterator_adaptor_random& src): _base(src._base), _vmap(src._vmap) {}
    valmap_iterator_adaptor_random& operator=(const valmap_iterator_adaptor_random& src) {
        if (this == &src) return *this;
        _base = src._base;
        _vmap = src._vmap;
        return *this;
    }

    base_iterator_type base() const { return this->_base; }
    valmap_iterator_adaptor_random(const base_iterator_type& src) : _base(src), _vmap() {}
    valmap_iterator_adaptor_random& operator=(const base_iterator_type& src) {
        _base = src;
        return *this;
    }

    // access methods
    reference operator*() const { return _vmap(*_base); }

    pointer operator->() const { return &(_vmap(*_base)); }

    reference operator[](const difference_type& n) {
        return this->_vmap(*(this->_base+n));
    }


    // pre-increment:
    valmap_iterator_adaptor_random& operator++() {
        ++_base;
        return *this;
    }

    // post-increment:
    valmap_iterator_adaptor_random operator++(int) {
        valmap_iterator_adaptor_random r(*this);
        ++(*this);
        return r;
    }

    // pre-dec:
    valmap_iterator_adaptor_random& operator--() {
        --(this->_base);
        return *this;
    }

    // post-dec:
    valmap_iterator_adaptor_random operator--(int) {
        valmap_iterator_adaptor_random r(*this);
        --(*this);
        return r;
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


    bool operator==(const valmap_iterator_adaptor_random& rhs) const {
        if (_base != rhs._base) return false;
        if (!(_vmap == rhs._vmap)) return false;
        return true;
    }
    bool operator!=(const valmap_iterator_adaptor_random& rhs) const { return !(*this == rhs); }

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


// random access iterators
template <typename Iterator, typename ValMap>
struct valmap_iterator_dispatch<Iterator, ValMap, std::random_access_iterator_tag> {
    typedef valmap_iterator_adaptor_random<Iterator, ValMap> adaptor_type;
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

template <typename Node, typename Value>
struct vmap_dispatch {
    typedef dref_vmap<Value> vmap;
};

template <typename Tree, typename Data, typename Key, typename Compare, typename Value>
struct vmap_dispatch<node_keyed<Tree, Data, Key, Compare>, Value> {
    typedef dref_second_vmap<Value> vmap;
};


struct node_type_dispatch_failed {};

template <typename Tree, typename CSModel>
struct node_type_dispatch {
    // catch-all should be compile error
    typedef node_type_dispatch_failed node_type;
    typedef node_type_dispatch_failed cs_value_type;
};


template <typename Tree, typename Unused>
struct node_type_dispatch<Tree, raw<Unused> > {
    typedef node_raw<Tree, typename Tree::data_type> node_type;
    typedef node_type* cs_value_type;
};

template <typename Tree, typename Compare>
struct node_type_dispatch<Tree, ordered<Compare> > {
    typedef node_ordered<Tree, typename Tree::data_type, Compare> node_type;
    typedef node_type* cs_value_type;
};


template <typename Tree>
struct node_type_dispatch<Tree, ordered<arg_default> > {
    typedef node_ordered<Tree, typename Tree::data_type, less<typename Tree::data_type> > node_type;
    typedef node_type* cs_value_type;
};


template <typename Tree, typename Key, typename Compare>
struct node_type_dispatch<Tree, keyed<Key, Compare> > {
    typedef node_keyed<Tree, typename Tree::data_type, Key, Compare> node_type;
    typedef std::pair<const Key* const, node_type*> cs_value_type;
};


} // namespace detail
} // namespace st_tree


namespace std {

template <typename Data, typename CSModel, typename Alloc>
void swap(st_tree::tree<Data, CSModel, Alloc>& a, st_tree::tree<Data, CSModel, Alloc>& b) {
    a.swap(b);
}

template <typename Tree, typename Data>
void swap(st_tree::detail::node_raw<Tree, Data>& a, st_tree::detail::node_raw<Tree, Data>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Compare>
void swap(st_tree::detail::node_ordered<Tree, Data, Compare>& a, st_tree::detail::node_ordered<Tree, Data, Compare>& b) {
    a.swap(b);
}

template <typename Tree, typename Data, typename Key, typename Compare>
void swap(st_tree::detail::node_keyed<Tree, Data, Key, Compare>& a, st_tree::detail::node_keyed<Tree, Data, Key, Compare>& b) {
    a.swap(b);
}

}  // namespace std

#endif
