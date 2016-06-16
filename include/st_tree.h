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

#if !defined(__st_tree_h__)
#define __st_tree_h__ 1


#include <string>
#include <exception>


namespace st_tree {

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
template <typename Key, typename Compare = std::less<Key> >
struct keyed {};


// generic exception base class for tree package
struct exception: public std::exception {
    exception() throw(): std::exception(), _what() {}
    virtual ~exception() throw() {}
    exception(const std::string& w) throw(): std::exception(), _what(w) {}
    virtual const char* what() const throw() { return _what.c_str(); }
    protected:
    std::string _what;
};

// attempting operations undefined without a parent node
struct parent_exception: public exception {
    parent_exception() throw(): exception() {}
    virtual ~parent_exception() throw() {}
    parent_exception(const std::string& w) throw(): exception(w) {}
};

// attempting operations on a node unassociated with a tree
struct orphan_exception: public exception {
    orphan_exception() throw(): exception() {}
    virtual ~orphan_exception() throw() {}
    orphan_exception(const std::string& w) throw(): exception(w) {}
};

// attempting operation that would create a cycle in tree
struct cycle_exception: public exception {
    cycle_exception() throw(): exception() {}
    virtual ~cycle_exception() throw() {}
    cycle_exception(const std::string& w) throw(): exception(w) {}
};

// attempting operation that is undefined on an empty tree 
struct empty_exception: public exception {
    empty_exception() throw(): exception() {}
    virtual ~empty_exception() throw() {}
    empty_exception(const std::string& w) throw(): exception(w) {}
};

// data, node, key, etc was undefined or not present
struct missing_exception: public exception {
    missing_exception() throw(): exception() {}
    virtual ~missing_exception() throw() {}
    missing_exception(const std::string& w) throw(): exception(w) {}
};

} // namespace st_tree


#if !defined(__st_tree_detail_h__)
#include "st_tree_detail.h"
#endif

#if !defined(__st_tree_iterators_h__)
#include "st_tree_iterators.h"
#endif

#if !defined(__st_tree_nodes_h__)
#include "st_tree_nodes.h"
#endif


namespace st_tree {

template <typename Data, typename CSModel, typename Alloc>
struct tree {
    typedef tree<Data, CSModel, Alloc> tree_type;
    typedef Data data_type;
    typedef CSModel cs_model_type;
    typedef Alloc allocator_type;
    typedef size_t size_type;
    typedef st_tree::detail::difference_type difference_type;

    protected:
    typedef detail::node_type_dispatch<tree_type, CSModel> nt_dispatch;

    public:
    typedef typename nt_dispatch::node_type node_type;
    typedef typename Alloc::template rebind<node_type>::other node_allocator_type;
    typedef typename Alloc::template rebind<typename nt_dispatch::cs_value_type>::other cs_allocator_type;

    protected:
    typedef typename node_type::base_type node_base_type;

    public:
    typedef node_type value_type;
    typedef node_type* pointer;
    typedef node_type const* const_pointer;
    typedef node_type& reference;
    typedef node_type const& const_reference;

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

    tree(const tree& src) : _root(NULL) { *this = src; }

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
        if (empty()) throw empty_exception("root(): empty tree has no root node");
        return *_root;
    }

    const node_type& root() const {
        if (empty()) throw empty_exception("root(): empty tree has no root node");
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
        node_base_type::_excise(s);
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
        node_base_type::_thread(n);
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

    template <typename _Tree, typename _Node, typename _ChildContainer> friend struct detail::node_base;
    template <typename _Tree, typename _Data> friend struct detail::node_raw;
    template <typename _Tree, typename _Data, typename _Compare> friend struct detail::node_ordered;
    template <typename _Tree, typename _Data, typename _Key, typename _Compare> friend struct detail::node_keyed;

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


}  // namespace st_tree



#endif  // __st_tree_h__
