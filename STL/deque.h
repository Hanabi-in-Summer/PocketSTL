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

    public:
        reference operator*() const { return *cur; }
        pointer operator->() const { return &(operator*()); }
        difference_type operator-(const self& x) const{
            return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.last - x.cur);
        }
        
        self& operator++(){
            ++cur;
            if(cur == last){
                set_node(node + 1);
                cur = first;
            }
            return *this;
        }

        self operator++(int){
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            if(cur == first){
                set_node(node - 1);
                cur = last;
            }
            --last;
            return *this;
        }

        self operator--(int){
            auto tmp = *this;
            --*this;
            return tmp;
        }

        self& operator+=(difference_type n){
            difference_type offset = n + (cur - first);
            if(offset >= 0 && offset < difference_type(buffer_size())){
                cur += n;
            }
            else{
                difference_type node_offset =
                    offset > 0 ? offset / difference_type(buffer_size())
                               : -difference_type((-offset - 1) / buffer_size()) - 1;
                set_node(node + node_offset);
                cur = first + (offset - node_offset * difference_type(buffer_size()));
            }
            return *this;
        }

        self operator+(difference_type n) const{
            auto tmp = *this;
            return tmp += n;
        }

        self& operator-=(difference_type n){
            return *this += -n;
        }

        self operator-(difference_type n) const{
            auto tmp = *this;
            return tmp -= n;
        }

        reference operator[](difference_type n) const { return *(*this += n); }
        bool operator==(const self& x) const { return cur == x.cur; }
        bool operator!=(const self& x) const { return cur != x.cur; }
        bool operator<(const self& x) const { return (node == x.node) ? (cur < x.cur) : (node < x.node); }
        bool operator>(const self& x) const { return (node == x.node) ? (cur > x.cur) : (node > x.node); }
        bool operator<=(const self& x) const { return !operator<(x); }
        bool operator>=(const self& x) const { return !operator>(x); }

    private:
        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
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