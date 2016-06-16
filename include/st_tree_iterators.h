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

#if !defined(__st_tree_iterators_h__)
#define __st_tree_iterators_h__ 1

#if !defined(__st_tree_detail_h__)
#include "st_tree_detail.h"
#endif

namespace st_tree {
namespace detail {

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
    b1st_iterator& operator=(const b1st_iterator& rhs) { _queue = rhs._queue; return *this; }

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
    typedef typename Alloc::template rebind<node_type*>::other node_ptr_allocator_type;
    deque<node_type*, node_ptr_allocator_type> _queue;
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
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; return *this; }
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
    d1st_post_iterator& operator=(const d1st_post_iterator& rhs) { _stack = rhs._stack; return *this; }

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
    typedef typename Alloc::template rebind<frame>::other frame_allocator_type;
    vector<frame, frame_allocator_type> _stack;
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
        frame& operator=(const frame& rhs) { first=rhs.first; second=rhs.second; visited=rhs.visited; return *this; }
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
    d1st_pre_iterator& operator=(const d1st_pre_iterator& rhs) { _stack = rhs._stack; return *this; }

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
    typedef typename Alloc::template rebind<frame>::other frame_allocator_type;
    vector<frame, frame_allocator_type> _stack;
};


} // namespace detail
} // namespace st_tree


#endif
