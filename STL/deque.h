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
        __deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
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
        deque(const deque& x) { allocate_and_copy(x.begin(), x.end()); }
        deque(deque&& x) : __map(std::move(x.__map)), __map_size(std::move(__map_size)){
            __start() = std::move(x.__start());
            __finish() = std::move(x.__finish());
            x.__map = nullptr;
            x.__map_size = 0;
        }
        deque(std::initializer_list<value_type> il) { allocate_and_copy(il.begin(), il.end()); }
        ~deque() { destroy_and_deallocate_all(); }
        deque& operator= (const deque& x);
        deque& operator= (deque&& x);
        deque& operator= (std::initializer_list<value_type> il);  

    public:
        /*************** Iterators *****************/
        iterator                begin() noexcept { return __start().cur; }
        const_iterator          begin() const noexcept { return __start().cur; }
        iterator                end() noexcept { return __finish().cur; }
        const_iterator          end() const noexcept { return __finish().cur; }
        reverse_iterator        rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin() const noexcept { return reverse_iterator(end()); }
        reverse_iterator        rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator  rend() const noexcept { return reverse_iterator(begin()); }
        const_iterator          cbegin() const noexcept { return begin(); }
        const_iterator          cend() const noexcept { return end(); }
        const_reverse_iterator  crbegin() const noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  crend() const noexcept { return reverse_iterator(begin()); }
        /*************** Capacity *****************/
        size_type size() const noexcept { return __finish() - __start(); }
        size_type max_size() const noexcept { return static_cast<size_type>(-1); }

        /*************** Element access *****************/
        reference       front() { return *__start(); }
        const_reference front() const { return *__start(); }
        reference       back() { return *(__finish() - 1); }
        const_reference back() const { return *(__finish() - 1); }
        /*************** Modifiers *****************/
        void        push_front (const value_type& val);
        void        push_front(value_type&& val) { emplace_front(std::move(val)); }
        void        push_back (const value_type& val);
        void        push_back(value_type&& val) { emplace_back(std::move(val)); }
        iterator    insert (const_iterator position, const value_type& val);
        iterator    insert (const_iterator position, size_type n, const value_type& val);
        template <class InputIterator, class std::enable_if<
                    std::is_integral<InputIterator>::value
                    >::type>
        iterator    insert (const_iterator position, InputIterator first, InputIterator last);
        iterator    insert (const_iterator position, value_type&& val);
        iterator    insert (const_iterator position, std::initializer_list<value_type> il);

        iterator    erase (const_iterator position );
        iterator    erase (const_iterator first, const_iterator last );
        template <class... Args>
        iterator    emplace (const_iterator position, Args&&... args);
        template <class... Args>
        void        emplace_front (Args&&... args);
        template <class... Args>
        void        emplace_back (Args&&... args);
    private:
        /***********************辅助工具*****************************/
        static size_type    buffer_size() { return iterator::buffer_size(); }
        void                allocate_and_fill(size_type n, const value_type& val);
        template <class InputItearator>
        void                allocate_and_copy(InputItearator first, InputItearator last);
        pointer             allocate_node() { return __data_allocator().allocate(buffer_size()); }
        void                create_map_and_nodes(size_type num_elementes);
        void                destroy_and_deallocate_all();
        void                expand_at_back(size_type nodes_to_add = 1);
        void                expand_at_front(size_type nodes_to_add = 1);
        void                reserve_map_at_back(size_type nodes_to_add = 1);
        void                reserve_map_at_front(size_type nodes_to_add = 1);
        void                reallocate_map(size_type nodes_to_add, bool add_at_front);
        template <class... Args>
        iterator            insert_aux(iterator pos, Args&&... args);
        iterator            insert_fill(iterator pos, size_type n, const value_type& val);

    };

    /*-------------------------------部分函数定义------------------------------------*/
    // -------------------- operator=
    template <class T, class Alloc>
    deque<T, Alloc>&
    deque<T, Alloc>::operator=(const deque& x){
        if(this != &x){

        }
    }


    // -------------------- Modifiers
    template <class T, class Alloc>
    template <class... Args>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::emplace(const_iterator position, Args&&... args){
        if(__start().cur == position.cur){
            emplace_front(std::forward<Args>(args)...);
            return __start();
        }
        else if(__finish().cur == position.cur){
            emplace_back(std::forward<Args>(args)...);
            return __finish() - 1;
        }
        else{
            insert_aux(position, std::forward<Args>(args)...);
        }
    }


    template <class T, class Alloc>
    void
    deque<T, Alloc>::push_back(const value_type& val){
        if(__finish().cur != __finish().last - 1){
            __data_allocator().construct(__finish().cur, val);
            ++__finish().cur;
        }
        else{
            expand_at_back();
            __data_allocator().construct(__finish().cur, val);
            ++__finish();
        }
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::push_front(const value_type& val){
        if(__finish().cur != __start().first){
            __data_allocator().construct(__start().cur - 1, val);
            --__start().cur;
        }
        else{
            expand_at_front();
            __data_allocator().construct(__start().cur - 1, val);
            --__start();
        }
    }

    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert(const_iterator position, const value_type& val){
        if(__start().cur == position.cur){
            push_front(val);
            return __start();
        }
        else if(__finish().cur == position.cur){
            push_back(val);
            return __finish() - 1;
        }
        else{
            insert_aux(position, val);
        }
    }

    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert(const_iterator position, value_type&& val){
        return emplace(position, std::move(val));
    }

    template <class T, class Alloc>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert(const_iterator position, size_type n, const value_type& val){
        if(position.cur == __start().cur){
            int nodes_to_add = (n - __start().cur + __start().first) / buffer_size();
            expand_at_front(nodes_to_add);
            iterator new_begin = __start() - n;
            uninitialized_fill_n(new_begin, n, val);
            __start() = new_begin;
            return __start();
        }
        else if(position.cur == __finish().cur){
            int nodes_to_add = (n - __finish().last + __start().cur + 1) / buffer_size();
            expand_at_back(nodes_to_add);
            iterator new_end = __finish() + n;
            iterator old_end = __finish();
            uninitialized_fill_n(__finish(), n, val);
            __finish() = new_end;
            return old_end;
        }
        else{
            return insert_fill(position, n, val);
        }
    }

    template <class T, class Alloc>
    template <class... Args>
    void
    deque<T, Alloc>::emplace_back(Args&&... args){
        if(__finish().cur != __finish().last - 1){
            __data_allocator().construct(__finish().cur, std::forward<Args>(args)...);
            ++__finish().cur;
        }
        else{
            expand_at_back();
            __data_allocator().construct(__finish().cur, std::forward<Args>(args)...);
            ++__finish();
        }
    }

    template <class T, class Alloc>
    template <class... Args>
    void
    deque<T, Alloc>::emplace_front(Args&&... args){
        if(__finish().cur != __start().first){
            __data_allocator().construct(__start().cur - 1, std::forward<Args>(args)...);
            --__start().cur;
        }
        else{
            expand_at_front();
            __data_allocator().construct(__start().cur - 1, std::forward<Args>(args)...);
            --__start();
        }
    }

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
        const size_type n = std::distance(first, last);
        create_map_and_nodes(n);
        for (auto cur = __start().node; cur < __finish().node; ++cur){
            auto next = first;
            std::advance(next, buffer_size());
            uninitialized_copy(first, next, *cur);
            first = next;
        }
        uninitialized_copy(first, last, __finish().first);
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::create_map_and_nodes(size_type num_elements){
        size_type num_nodes = num_elements / buffer_size() + 1;
        __map_size = std::max(num_nodes + 2, size_type(DEQUE_INIT_MAP_SIZE));
        __map = __map_allocator().allocate(__map_size);
        map_pointer nstart = __map + (__map_size - num_nodes) / 2;
        map_pointer nfinish = nstart + num_nodes - 1;
        map_pointer cur;
        try{
            for (cur = nstart; cur <= nfinish; ++cur){
                *cur = allocate_node();
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

    template <class T, class Alloc>
    void
    deque<T, Alloc>::destroy_and_deallocate_all(){
        for (auto cur = __start().node + 1; cur < __finish().node; ++cur){
            destroy(*cur, *(cur + buffer_size()));
        }
        if(__start().node != __finish().node){
            destroy(__start().cur, __start().last);
            destroy(__finish().first, __finish().cur);
        }
        __data_allocator().deallocate(*__start().node, buffer_size());
        __data_allocator().deallocate(__map, __map_size);
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::expand_at_back(size_type nodes_to_add){
        reserve_map_at_back(nodes_to_add);
        for (int i = 1; i <= nodes_to_add; ++i){
            *(__finish().node + i) = allocate_node();
        }
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::expand_at_front(size_type nodes_to_add){
        reserve_map_at_front(nodes_to_add);
        for (int i = 1; i <= nodes_to_add; ++i){
            *(__start().node - i) = allocate_node();
        }
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::reserve_map_at_back(size_type nodes_to_add){
        if(nodes_to_add + 1 > __map_size  - (__finish().node - __map)){
            reallocate_map(nodes_to_add, false);
        }
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::reserve_map_at_front(size_type nodes_to_add){
        if(nodes_to_add + 1 > __start().node - __map){
            reallocate_map(nodes_to_add, true);
        }
    }

    template <class T, class Alloc>
    void
    deque<T, Alloc>::reallocate_map(size_type nodes_to_add, bool add_at_front){
        size_type old_num_nodes = __finish().node - __start().node + 1;
        size_type new_num_nodes = old_num_nodes + nodes_to_add;
        map_pointer new_nstart;
        if(__map_size > 2 * new_num_nodes){
            new_nstart = __map + (__map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            if(new_nstart < __start().node){
                std::copy(__start().node, __finish().node + 1, new_nstart);
            }
            else{
                std::copy_backward(__start().node, __finish().node, new_nstart + old_num_nodes);
            }
        }
        else{
            size_type new_map_size = __map_size + std::max(__map_size, nodes_to_add) + 2;
            map_pointer new_map = __map_allocator().allocate(new_map_size);
            new_nstart = new_map + (new_map_size - new_num_nodes) / 2 + (add_at_front ? nodes_to_add : 0);
            std::copy(__start().node, __finish().node + 1, new_nstart);
            __map_allocator().deallocate(__map, __map_size);
            __map = new_map;
            __map_size = new_map_size;
        }
        __start().set_node(new_nstart);
        __finish().set_node(new_nstart + old_num_nodes - 1);
    }

    template <class T, class Alloc>
    template <class... Args>
    typename deque<T, Alloc>::iterator
    deque<T, Alloc>::insert_aux(iterator pos, Args&&... args){
        const size_type elems_before = pos - __start();
        value_type val_cp = value_type(std::forward<Args>(args)...);
        if(elems_before < size() / 2){
            emplace_front(front());
            iterator front1 = __start();
            ++front1;
            iterator front2 = front1;
            ++front2;
            pos = __start() + elems_before;
            iterator pos1 = pos;
            ++pos1;
            std::copy(front2, pos1, front1);
        }
        else{
            emplace_back(back());
            iterator back1 = __finish();
            --back1;
            iterator back2 = back1;
            --back2;
            pos = __start() + elems_before;
            std::copy_backward(pos, back2, back1);
        }
        *pos = std::move(val_cp);
        return pos;
    }

} // namespace

#endif