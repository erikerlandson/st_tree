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

#if !defined(__st_tree_nodes_h__)
#define __st_tree_nodes_h__ 1


#if !defined(__st_tree_detail_h__)
#include "st_tree_detail.h"
#endif

#if !defined(__st_tree_iterators_h__)
#include "st_tree_iterators.h"
#endif


namespace st_tree {
namespace detail {

template <typename Tree, typename Node, typename ChildContainer>
struct node_base {
    typedef Tree tree_type;
    typedef Node node_type;
    typedef ChildContainer cs_type;
    typedef size_t size_type;
    typedef typename tree_type::data_type data_type;
    typedef typename Tree::allocator_type allocator_type;

    protected:
    typedef typename cs_type::iterator cs_iterator;
    typedef typename cs_type::const_iterator cs_const_iterator;
    
    public:
    typedef typename valmap_iterator_dispatch<cs_iterator, typename vmap_dispatch<node_type, typename cs_iterator::value_type>::vmap, typename cs_iterator::iterator_category>::adaptor_type iterator;
    typedef typename valmap_iterator_dispatch<cs_const_iterator, typename vmap_dispatch<node_type, typename cs_const_iterator::value_type>::vmap, typename cs_const_iterator::iterator_category>::adaptor_type const_iterator;

    iterator begin() { return iterator(_children.begin()); }
    iterator end() { return iterator(_children.end()); }
    const_iterator begin() const { return const_iterator(_children.begin()); }
    const_iterator end() const { return const_iterator(_children.end()); }

    typedef b1st_iterator<node_type, node_type, allocator_type> bf_iterator;
    typedef b1st_iterator<node_type, const node_type, allocator_type> const_bf_iterator;
    typedef d1st_post_iterator<node_type, node_type, allocator_type> df_post_iterator;
    typedef d1st_post_iterator<node_type, const node_type, allocator_type> const_df_post_iterator;
    typedef d1st_pre_iterator<node_type, node_type, allocator_type> df_pre_iterator;
    typedef d1st_pre_iterator<node_type, const node_type, allocator_type> const_df_pre_iterator;

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
        if (NULL == q->_tree) throw orphan_exception("tree(): orphan node has no associated tree");
        return *(q->_tree);
    }

    const tree_type& tree() const {
        const node_type* q = static_cast<const node_type*>(this);
        while (!q->is_root())  q = q->_parent;
        if (NULL == q->_tree) throw orphan_exception("tree(): orphan node has no associated tree");
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
        if (is_root()) throw parent_exception("parent(): node has no parent");
        return *(_parent);
    }
    const node_type& parent() const {
        if (is_root()) throw parent_exception("parent(): node has no parent");
        return *(_parent); 
    }

    size_type size() const { return _children.size(); }
    bool empty() const { return _children.empty(); }

    void clear() {
        _erase(begin(), end());
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

    friend struct st_tree::tree<data_type, typename Tree::cs_model_type, allocator_type>;
    friend struct b1st_iterator<node_type, node_type, allocator_type>;
    friend struct b1st_iterator<node_type, const node_type, allocator_type>;
    friend struct d1st_post_iterator<node_type, node_type, allocator_type>;
    friend struct d1st_post_iterator<node_type, const node_type, allocator_type>;
    friend struct d1st_pre_iterator<node_type, node_type, allocator_type>;
    friend struct d1st_pre_iterator<node_type, const node_type, allocator_type>;

    protected:
    tree_type* _tree;
    size_type _size;
    max_maintainer<size_type, allocator_type> _depth;
    node_type* _parent;
    data_type _data;
    cs_type _children;

    bool _default_constructed() const {
        return (NULL == _parent) && (NULL == _tree);
    }

    iterator _iterator() { return iterator(node_type::_cs_iterator(*static_cast<node_type*>(this))); }

    void _erase(const iterator& j) {
        node_type* n = &*j;
        _prune(n);
        _children.erase(j.base());
        this->tree()._delete_node(n);
    }

    void _erase(const iterator& F, const iterator& L) {
        vector<node_type*> d;
        for (iterator j(F);  j != L;  ++j) {
            node_type* n = &*j;
            _prune(n);
            d.push_back(n);
        }
        _children.erase(F.base(), L.base());
        tree_type& tree_ = this->tree();
        for (typename vector<node_type*>::iterator e(d.begin());  e != d.end();  ++e) tree_._delete_node(*e);
    }

    void _erase() {
        if (is_root()) _tree->erase();
        else parent().erase(_iterator());
    }

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


template <typename Tree, typename Data>
struct node_raw: public node_base<Tree, node_raw<Tree, Data>, vector<node_raw<Tree, Data>*, typename Tree::cs_allocator_type> > {
    typedef Tree tree_type;
    typedef node_raw<Tree, Data> node_type;
    typedef typename Tree::allocator_type allocator_type;
    typedef vector<node_type*, typename Tree::cs_allocator_type> cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef Data data_type;

    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef size_t size_type;
    typedef st_tree::detail::difference_type difference_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend struct st_tree::tree<data_type, typename Tree::cs_model_type, allocator_type>;
    friend struct node_base<Tree, node_type, cs_type>;

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

        // LHS is non-default, but RHS is default:
        if (rhs._default_constructed()) {
            // both are default-constructed, no-op:
            if (this->_default_constructed()) return *this;

            // Seems sane to define semantic as 'empty'
            // should also consider removing from tree?
            this->clear();
            this->_data = rhs._data;
            return *this;
        }

        // LHS is default-constructed (RHS is non-default)
        if (this->_default_constructed()) {
            // A workable semantic is a sort of free-standing node, who shares rhs tree
            // and is deep-copied, but is not actually a full-fledged member of a tree
            this->_tree = const_cast<tree_type*>(&(rhs.tree()));
        }

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw cycle_exception("op=(): operation introduces cycle");

        node_type* r = const_cast<node_type*>(&rhs);
        // important if rhs is child of "this", to prevent it from getting deallocated by clear()
        bool ancestor = this->is_ancestor(rhs);
        if (ancestor) base_type::_excise(r);

        // in the case of vector storage, I can just leave current node where it is
        this->clear();
        this->_data = rhs._data;
        // do the copying work for children only
        for (cs_const_iterator j(r->_children.begin());  j != r->_children.end();  ++j) {
            node_type* n = (*j)->_copy_data(this->tree());
            this->_children.push_back(n);
            base_type::_thread(n);
            this->_graft(n);
        }
        if (ancestor) this->tree()._delete_node(r);

        return *this;
    }

    void swap(node_type& b) {
        node_type& a = *this;

        if (&a == &b) return;

        // this would introduce cycles 
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw cycle_exception("swap(): operation introduces cycle");

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
        if (this == &src) throw cycle_exception("graft(): operation introduces cycle");
        if (src.is_ancestor(*this)) throw cycle_exception("graft(): operation introduces cycle");

        // remove src from its current location
        node_type* s = &src;
        base_type::_excise(s);

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

    void erase() { this->_erase(); }
    void erase(const iterator& j) { this->_erase(j); }
    void erase(const iterator& F, const iterator& L) { this->_erase(F, L); }

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
        if (n.is_root()) throw parent_exception("_cs_iterator(): node nas no parent");
        cs_iterator j(n.parent()._children.begin());
        cs_iterator jend(n.parent()._children.end());
        for (;  j != jend;  ++j) if (*j == &n) break;
        if (j == jend) throw missing_exception("_cs_iterator(): requested node does not exist");
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


template <typename Tree, typename Data, typename Compare>
struct node_ordered: public node_base<Tree, node_ordered<Tree, Data, Compare>, multiset<node_ordered<Tree, Data, Compare>*, ptr_less_data<Compare>, typename Tree::cs_allocator_type> > {
    typedef Tree tree_type;
    typedef typename Tree::allocator_type allocator_type;
    typedef node_ordered<Tree, Data, Compare> node_type;
    typedef multiset<node_type*, ptr_less_data<Compare>, typename Tree::cs_allocator_type> cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef Data data_type;

    typedef data_type key_type;
    typedef Compare key_compare;
    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef size_t size_type;
    typedef st_tree::detail::difference_type difference_type;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend struct st_tree::tree<data_type, typename Tree::cs_model_type, allocator_type>;
    friend struct node_base<Tree, node_type, cs_type>;

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

        // LHS is non-default, but RHS is default:
        if (rhs._default_constructed()) {
            // both are default-constructed, no-op:
            if (this->_default_constructed()) return *this;

            // Seems sane to define semantic as 'empty'
            // should also consider removing from tree?
            this->clear();
            this->_data = rhs._data;
            return *this;
        }

        // LHS is default-constructed (RHS is non-default)
        if (this->_default_constructed()) {
            // A workable semantic is a sort of free-standing node, who shares rhs tree
            // and is deep-copied, but is not actually a full-fledged member of a tree
            this->_tree = const_cast<tree_type*>(&(rhs.tree()));
        }

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw cycle_exception("op=(): operation introduces cycle");

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        node_type* t = this;
        node_type* r = const_cast<node_type*>(&rhs);
        bool ancestor = this->is_ancestor(rhs);
        if (ancestor) base_type::_excise(r);

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
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw cycle_exception("swap(): operation introduces cycle");

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
        if (this == &src) throw cycle_exception("graft(): operation introduces cycle");
        if (src.is_ancestor(*this)) throw cycle_exception("graft(): operation introduces cycle");

        // remove src from its current location
        node_type* s = &src;
        base_type::_excise(s);

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

    iterator lower_bound(const data_type& data) {
        node_type s;
        s._data = data;
        return iterator(this->_children.lower_bound(&s));
    }
    const_iterator lower_bound(const data_type& data) const {
        node_type s;
        s._data = data;
        return const_iterator(this->_children.lower_bound(&s));
    }
    iterator upper_bound(const data_type& data) {
        node_type s;
        s._data = data;
        return iterator(this->_children.upper_bound(&s));
    }
    const_iterator upper_bound(const data_type& data) const {
        node_type s;
        s._data = data;
        return const_iterator(this->_children.upper_bound(&s));
    }
    pair<iterator, iterator> equal_range(const data_type& data) {
        node_type s;
        s._data = data;
        return pair<iterator, iterator>(this->_children.equal_range(&s));
    }
    pair<const_iterator, const_iterator> equal_range(const data_type& data) const {
        node_type s;
        s._data = data;
        return pair<const_iterator, const_iterator>(this->_children.equal_range(&s));
    }


    size_type count(const data_type& data) const {
        node_type s;
        s._data = data;
        return this->_children.count(&s);
    }

    void erase() { this->_erase(); }
    void erase(const iterator& j) { this->_erase(j); }
    void erase(const iterator& F, const iterator& L) { this->_erase(F, L); }

    size_type erase(const data_type& data) {
        size_type c = count(data);
        if (c <= 0) return 0;
        pair<iterator, iterator> r = equal_range(data);
        erase(r.first, r.second);
        return c;
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
        if (n.is_root()) throw parent_exception("_cs_iterator(): node has no parent");
        pair<cs_iterator, cs_iterator> r(n.parent()._children.equal_range(&n));
        if (r.first == r.second) throw missing_exception("_cs_iterator(): requested node does not exist");
        for (cs_iterator j(r.first);  j != r.second;  ++j)
            if (*j == &n) return j;
        throw missing_exception("_cs_iterator(): requested node does not exist");
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


template <typename Tree, typename Data, typename Key, typename Compare>
struct node_keyed: public node_base<Tree, node_keyed<Tree, Data, Key, Compare>, map<const Key*, node_keyed<Tree, Data, Key, Compare>*, ptr_less<Compare>, typename Tree::cs_allocator_type> > {
    typedef Tree tree_type;
    typedef node_keyed<Tree, Data, Key, Compare> node_type;
    typedef typename Tree::allocator_type allocator_type;
    typedef map<const Key*, node_type*, ptr_less<Compare>, typename Tree::cs_allocator_type> cs_type;
    typedef node_base<Tree, node_type, cs_type> base_type;
    typedef Data data_type;

    typedef Key key_type;
    typedef Compare key_compare;
    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

    typedef size_t size_type;
    typedef st_tree::detail::difference_type difference_type;

    typedef pair<const key_type, data_type> kv_pair;

    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;

    friend struct st_tree::tree<data_type, typename Tree::cs_model_type, allocator_type>;
    friend struct node_base<Tree, node_type, cs_type>;

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

        // LHS is non-default, but RHS is default:
        if (rhs._default_constructed()) {
            // both are default-constructed, no-op:
            if (this->_default_constructed()) return *this;

            // Seems sane to define semantic as 'empty'
            // should also consider removing from tree?
            this->clear();
            this->_data = rhs._data;
            return *this;
        }

        // LHS is default-constructed (RHS is non-default)
        if (this->_default_constructed()) {
            // A workable semantic is a sort of free-standing node, who shares rhs tree
            // and is deep-copied, but is not actually a full-fledged member of a tree
            this->_tree = const_cast<tree_type*>(&(rhs.tree()));
        }

        // this would introduce cycles
        if (rhs.is_ancestor(*this)) throw cycle_exception("op=(): operation introduces cycle");

        // important to save these prior to clearing 'this'
        // note, rhs may be child of 'this', and get erased too, otherwise
        node_type* r = const_cast<node_type*>(&rhs);
        bool ancestor = this->is_ancestor(rhs);
        if (ancestor) base_type::_excise(r);

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
        if (this->end() == f) throw missing_exception("op[](): key undefined");
        return *f;
    }

    iterator find(const key_type& key) { return iterator(this->_children.find(&key)); }
    const_iterator find(const key_type& key) const { return const_iterator(this->_children.find(&key)); }

    iterator lower_bound(const key_type& key) {
        return iterator(this->_children.lower_bound(&key));
    }
    const_iterator lower_bound(const key_type& key) const {
        return const_iterator(this->_children.lower_bound(&key));
    }
    iterator upper_bound(const key_type& key) {
        return iterator(this->_children.upper_bound(&key));
    }
    const_iterator upper_bound(const key_type& key) const {
        return const_iterator(this->_children.upper_bound(&key));
    }
    pair<iterator, iterator> equal_range(const key_type& key) {
        return pair<iterator, iterator>(this->_children.equal_range(&key));
    }
    pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return pair<const_iterator, const_iterator>(this->_children.equal_range(&key));
    }

    size_type count(const key_type& key) const {
        return this->_children.count(&key);
    }

    void erase() { this->_erase(); }
    void erase(const iterator& j) { this->_erase(j); }
    void erase(const iterator& F, const iterator& L) { this->_erase(F, L); }

    size_type erase(const key_type& key) {
        size_type c = count(key);
        if (c <= 0) return 0;
        pair<iterator, iterator> r = equal_range(key);
        erase(r.first, r.second);
        return c;
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
        if (a.is_ancestor(b) || b.is_ancestor(a)) throw cycle_exception("swap(): operation introduces cycle");

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
        if (this == &src) throw cycle_exception("graft(): operation introduces cycle");
        if (src.is_ancestor(*this)) throw cycle_exception("graft(): operation introduces cycle");

        // remove src from its current location
        node_type* s = &src;
        base_type::_excise(s);

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
        if (n.is_root()) throw parent_exception("_cs_iterator(): node has no parent");
        cs_iterator j(n.parent()._children.find(&n._key));
        if (j == n.parent()._children.end()) throw missing_exception("_cs_iterator(): requested node does not exist");
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


} // namespace detail
} // namespace st_tree

#endif
