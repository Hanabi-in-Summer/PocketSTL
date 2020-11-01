#ifndef _POCKET_DEQUE_H_
#define _POCKET_DEQUE_H_

/*
** deque
** 
*/

#include <cstddef>
#include <stdexcept>
#include "allocator.h"
#include "uninitialized.h"

namespace pocket_stl{
    
    template <class T, class Ref, class Ptr>
    class __deque_iterator{
    public:
        using iterator_category     = random_access_iterator_tag;
        using iterator              = __deque_iterator<T, T&, T*>;
        using const_iterator        = __deque_iterator<T, const T&, const T*>;
        using self                  = __deque_iterator;
        using value_type            = T;
        using pointer               = Ptr;
        using reference             = Ref;
        using size_type             = size_t;
        using difference_type       = ptrdiff_t;
        using map_pointer           = T**;

        static size_type buffer_size() { return __deque_buffer_size(sizeof(T)); }
        static size_type __deque_buffer_size(size_t size) { return size < 512 ? size_type(512 / size) : size_type(1); }
    
    private:
        template <class, class> friend class deque;
        T* cur;
        T* first;
        T* last;
        map_pointer node;
    public:
        __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), map_pointer(nullptr) {}
        __deque_iterator(T* cur_arg, map_pointer node_arg) 
            : cur(cur_arg), first(*node_arg), last(*node_arg + buffer_size()), node(node_arg) {}
        __deque_iterator(const iterator& rhs)
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
        __deque_iterator(iterator && rhs)
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node){
            rhs.cur = rhs.first = rhs.last = rhs.node = nullptr;
        }
        __deque_iterator(const const_iterator& rhs)
            : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
        self& operator=(const iterator& rhs){
            if(&rhs != this){
                cur = rhs.cur;
                first = rhs.first;
                last = rhs.last;
                node = rhs.node;
            }
            return *this;
        }
    };

    template <class T, class Alloc = pocket_stl::allocator<T>>
    class deque{
    public:
        using allocator_type = Alloc;
        using value_type = typename allocator_type::value_type;
        using reference = value_type&;
        using const_reference = const value_type&;

        using difference_type = typename allocator_type::difference_type;
        using size_type = typename allocator_type::size_type;
    };

} // namespace

#endif