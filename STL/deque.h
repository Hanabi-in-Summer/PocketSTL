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
    #define DEQUE_INIT_MAP_SIZE 8
    
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
    
    private:
        template <class, class> friend class deque;
        T* cur;
        T* first;
        T* last;
        map_pointer node;
        static size_type buffer_size() { return __deque_buffer_size(sizeof(T)); }
        static size_type __deque_buffer_size(size_t size) { return size < 512 ? size_type(512 / size) : size_type(1); }
    
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
        using allocator_type            = Alloc;
        using value_type                = typename allocator_type::value_type;
        using reference                 = value_type&;
        using const_reference           = const value_type&;
        using pointer                   = typename allocator_type::pointer;
        using const_pointer             = typename allocator_type::const_pointer;
        using iterator                  = __deque_iterator<T, reference, pointer>;
        using const_iterator            = __deque_iterator<T, const_reference, const_pointer>;

        using difference_type           = typename allocator_type::difference_type;
        using size_type                 = typename allocator_type::size_type;

        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

    private:
        using map_pointer               = T**;
        using map_allocator_type        = typename Alloc::template rebind<T*>::other;

        compressed_pair<iterator, allocator_type> __start_and_data_alloc;
        compressed_pair<iterator, map_allocator_type> __finish_and_map_alloc;
        map_pointer __map;
        size_type __map_size;

        compressed_pair<iterator, allocator_type>&          __data_allocator() noexcept { return __start_and_data_alloc; }
        const compressed_pair<iterator, allocator_type>&    __data_allocator() const noexcept { return __start_and_data_alloc; }
        compressed_pair<iterator, map_allocator_type>&          __map_allocator() noexcept { return __finish_and_map_alloc; }
        const compressed_pair<iterator, map_allocator_type>&    __map_allocator() const noexcept { return __finish_and_map_alloc; }
        iterator&           __start() noexcept { return __start_and_data_alloc.data; }
        const iterator&     __start() const noexcept { return __start_and_data_alloc.data; }
        iterator&           __finish() noexcept { return __finish_and_map_alloc.data; }
        const iterator&     __finish() const noexcept { return __finish_and_map_alloc.data; }

    public:
        /***************ctor 、 copy_ctor 、 move_ctor 、 dtor 、 operator=*****************/
        explicit deque() { allocate_and_fill(0, value_type()); }
        explicit deque(size_type n) { allocate_and_fill(n, value_type()); }
        deque(size_type n, const value_type& val) { allocate_and_fill(n, val); }
        template <class InputIterator, class std::enable_if<
                                           !std::is_integral<InputIterator>::value>::type>
        deque(InputIterator first, InputIterator last) { allocate_and_copy(first, last); } 
        deque(const deque& x);
        deque(deque&& x);
        deque(std::initializer_list<value_type> il);


    private:
        /***********************辅助工具*****************************/
        static size_type buffer_size() { return iterator::buffer_size(); }
        void allocate_and_fill(size_type n, const value_type& val);
        template <class InputItearator>
        void allocate_and_copy(InputItearator first, InputItearator last);
        void create_map_and_nodes(size_type num_elementes);

    };

    /*-------------------------------部分函数定义------------------------------------*/


    // -------------------- 辅助工具
    template <class T, class Alloc>
    void
    deque<T, Alloc>::allocate_and_fill(size_type n, const value_type& val){
        create_map_and_nodes(n);
        if (n == 0) return;
        map_pointer cur;
        
        for (cur = __start().node; cur < __finish().node; ++cur){
            uninitialized_fill(*cur, *cur + buffer_size(), val);
        }
        uninitialized_fill(__finish().first, __finish().cur, val);
        
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void
    deque<T, Alloc>::allocate_and_copy(InputIterator first, InputIterator last){
        
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::create_map_and_nodes(size_type num_elements){
        size_type num_nodes = num_elements / buffer_size() + 1;
        __map_size = max(num_nodes + 2, DEQUE_INIT_MAP_SIZE);
        __map = __map_allocator().allocate(__map_size);
        map_pointer nstart = __map + (__map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try{
            for (cur = nstart; cur <= nfinish; ++cur){
                *cur = __data_allocator().allocate(buffer_size());
            }
        }
        catch(...){
            while(cur != nstart){
                --cur;
                __data_allocator().deallocate(*cur, buffer_size());
            }
            throw;
        }
        __start().set_node(nstart);
        __finish().set_node(nfinish);
        __start().cur = __start().first;
        __finish().cur = __finish().first + num_elements % buffer_size();
    }

} // namespace

#endif