#ifndef _POCKET_LIST_H_
#define _POCKET_LIST_H_

/*
** list
** 
*/

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include "allocator.h"
#include "uninitialized.h"

namespace pocket_stl{
    // list的结点node
    // class __list_node_base{
    //     __list_node_base* prev;
    //     __list_node_base* next;
    // };

    template <class T>
    struct __list_node{
        __list_node* prev;
        __list_node* next;
        T data;
    };

    template <class T, class Ref = T&, class Ptr = T*>
    class __list_iterator{
    public:
        using iterator              = __list_iterator<T, T&, T*>;
        using self                  = __list_iterator<T, Ref, Ptr>;

        using iterator_category     = bidrectional_iterator_tag;
        using value_type            = T;
        using pointer               = Ptr;
        using reference             = Ref;
        using link_type             = __list_node<T>*;
        using size_type             = size_t;
        using difference_type       = ptrdiff_t;

        link_type node_ptr;         // iterator 占用一个指针的空间

        // ctor
        __list_iterator() : node_ptr(nullptr) { };
        __list_iterator(link_type x) : node_ptr(x) {}
        __list_iterator(const iterator& x) : node_ptr(x.node_ptr) {}

        bool        operator==(const self& x) const { return node_ptr == x.node_ptr; }
        bool        operator!=(const self& x) const { return node_ptr != x.node_ptr; }
        reference   operator*() const { return (*node_ptr).data; }
        pointer     operator->() const { return &(operator*()); }
        
        self& operator++() {                        // 前置版++
            node_ptr = (*node_ptr).next;
            return *this;
        }
        
        self operator++(int){                      // 后置版++
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            node_ptr = (*node_ptr).prev;
            return *this;
        }
        self operator--(int){
            self tmp = *this;
            --*this;
            return tmp;
        }
    };

    // 需要定义 const_iterator, 指向的值不能修改, 也就是重载的"->"和"*"运算符的返回值不能被修改, 迭代器本身可以修改.
    // 为了使const类型的迭代器依旧可以遍历const容器
    template <class T, class Ref = const T&, class Ptr = const T*>
    class __list_const_iterator{
    public:
        using iterator              = __list_const_iterator<T, T&, T*>;
        using self                  = __list_const_iterator<T, Ref, Ptr>;

        using iterator_category     = bidrectional_iterator_tag;
        using value_type            = T;
        using pointer               = Ptr;
        using reference             = Ref;
        using link_type             = __list_node<T>*;
        using size_type             = size_t;
        using difference_type       = ptrdiff_t;

        link_type node_ptr;         // iterator 占用一个指针的空间

        // ctor
        __list_const_iterator() : node_ptr(nullptr) { };
        __list_const_iterator(link_type x) : node_ptr(x) {}
        __list_const_iterator(const iterator& x) : node_ptr(x.node_ptr) {}
        __list_const_iterator(const __list_iterator<T>& x) : node_ptr(x.node_ptr) {}

        bool        operator==(const self& x) const { return node_ptr == x.node_ptr; }
        bool        operator!=(const self& x) const { return node_ptr != x.node_ptr; }
        reference   operator*() const { return (*node_ptr).data; }
        pointer     operator->() const { return &(operator*()); }
        
        self& operator++() {                        // 前置版++
            node_ptr = (*node_ptr).next;
            return *this;
        }
        
        self operator++(int){                      // 后置版++
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            node_ptr = (*node_ptr).prev;
            return *this;
        }
        self operator--(int){
            self tmp = *this;
            --*this;
            return tmp;
        }
    };

    template <class T, class Alloc = pocket_stl::allocator<T>>
    class list{
    public:
        using value_type                = T;
        using allocator_type            = Alloc;
        using reference                 = value_type&;
        using const_reference           = const value_type&;
        using pointer                   = typename allocator_type::pointer;
        using const_pointer             = typename allocator_type::const_pointer;
        using iterator                  = __list_iterator<value_type>;
        using const_iterator            = __list_const_iterator<value_type>;

        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

        using difference_type           = typename allocator_type::difference_type;
        using size_type                 = typename allocator_type::size_type;

    private:
        using list_node             = __list_node<T>;
        using link_type             = list_node*;
        using node_allocator_type   = pocket_stl::allocator<list_node>;
        // link_type           __node_ptr;
        compressed_pair<link_type, allocator_type> data_allocator ;         // 保存 __node_ptr, 指向头尾节点
        compressed_pair<size_type, node_allocator_type> node_allocator;     // 保存 size, 表示 list 的长度

        link_type& __node_ptr() { return data_allocator.data; }
        size_type& __size() { return node_allocator.data; }
        // node_allocator_type node_allocator;
        // allocator_type      data_allocator;

    public:
        /***************ctor 、 copy_ctor 、 move_ctor 、 dtor 、 operator=*****************/
        // **** default ctor
        list() { empty_initialize(); }
        // **** fill ctor
        explicit list(size_type n){
            allocate_and_fill(n, value_type());
        }
        list(size_type n, const value_type& val){
            allocate_and_fill(n, val);
        }
        // **** range ctor
        template <class InputIterator,
                  class std::enable_if<
                      !std::is_integral<InputIterator>::value>::type>
        list(InputIterator first, InputIterator last) { allocate_and_copy(first, last); }
        // **** copy ctor
        list(const list& x) { allocate_and_copy(x.begin(), x.end()); }
        // **** move ctor
        list(list&& x){
            __node_ptr() = x.__node_ptr();
            x.__node_ptr() = nullptr;
        }
        // **** initializer list
        list(std::initializer_list<value_type> il) { allocate_and_copy(il.begin(), il.end()); }
        // **** dtor
        ~list() { destroy_and_deallocate_all(); }
        // **** operator=
        list& operator= (const list& x);
        list& operator= (list&& x);
        list& operator= (std::initializer_list<value_type> il);

    public:
        /*************** iterator functions *****************/
        iterator                begin() noexcept { return __node_ptr()->next; }
        const_iterator          begin() const noexcept { return __node_ptr()->next; }
        iterator                end() noexcept { return __node_ptr(); }
        const_iterator          end() const noexcept { return __node_ptr(); }
        reverse_iterator        rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin() const noexcept { return reverse_iterator(end()); }
        reverse_iterator        rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator  rend() const noexcept { return reverse_iterator(begin()); }
        const_iterator          cbegin() const noexcept { return begin(); }
        const_iterator          cend() const noexcept { return end(); }
        const_reverse_iterator  crbegin() const noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  crend() const noexcept { return reverse_iterator(begin()); }

        /*************** capacity functions *****************/
        bool            empty() const noexcept { return __node_ptr()->next == __node_ptr(); }
        size_type       size() const noexcept { return node_allocator.data; }
        size_type       max_size() const noexcept { return static_cast<size_type>(-1); }
        /***************** element access *******************/
        reference       front() { return *begin(); }
        const_reference front() const { return *begin(); }
        reference       back() { return *end(); }
        const_reference back() const { return *end(); }

    public:
        /************************ modifiers ***********************/
        // assign
        template <class InputIterator, class std::enable_if<
                                            !std::is_integral<InputIterator>::value>::type>
        void        assign(InputIterator first, InputIterator last) { return copy_aux(first, last); } 
        void        assign(size_type n, const value_type& val);
        void        assign(std::initializer_list<value_type> il) { return copy_aux(il.begin(), il.end()); }

        template <class... Args>
        void        emplace_front(Args&&... args) { emplace(begin(), std::forward<Args>(args)...); }
        void        push_front(const value_type& val) { insert(begin(), val); }
        void        push_front(value_type&& val) { insert(begin(), std::move(val)); }
        void        pop_front() { erase(begin()); }
        template <class... Args>
        void        emplace_back(Args&&... args) { emplace(end(), std::forward<Args>(args)...); }
        void        push_back(const value_type& val) { insert(end(), val); }
        void        push_back(value_type&& val) { insert(end(), std::move(val)); }
        void        pop_back() { erase(end()); }
        template <class... Args>
        iterator    emplace (const_iterator position, Args&&... args);
        // insert
        iterator    insert(const_iterator position, const value_type& val) { return insert_one(position, val); }
        iterator    insert(const_iterator position, size_type n, const value_type& val) { return insert_fill(position, n, val); }
        template <class InputIterator, class std::enable_if<
                                            !std::is_integral<InputIterator>::value>::type>
        iterator    insert(const_iterator position, InputIterator first, InputIterator last) { return insert_copy(position, first, last); } 
        iterator    insert(const_iterator position, value_type&& val);
        iterator    insert(const_iterator position, std::initializer_list<value_type> il) { return insert_copy(il.begin(), il.end()); }
        // erase
        iterator    erase(const_iterator position);
        iterator    erase(const_iterator first, const_iterator last);
        void        swap (list& x);
        void        resize(size_type n) { resize(n, value_type()); }
        void        resize (size_type n, const value_type& val);
        void        clear() noexcept { erase(begin(), end()); }
        /************************ operations ***********************/
        void        splice (const_iterator position, list& x);
        void        splice (const_iterator position, list&& x);
        void        splice (const_iterator position, list& x, const_iterator i);
        void        splice (const_iterator position, list&& x, const_iterator i);
        void        splice (const_iterator position, list& x, const_iterator first, const_iterator last);
        void        splice (const_iterator position, list&& x, const_iterator first, const_iterator last);
        void        remove (const value_type& val) { 
            remove_if([&](const value_type& elem) { return val == elem; });
        }
        template <class Predicate>
        void        remove_if (Predicate pred);
        void        unique() {
            unique([&](const value_type& x, const value_type& y) { return x == y; });
        }
        template <class BinaryPredicate>
        void        unique (BinaryPredicate binary_pred);
        void        merge (list& x){
                    merge(x, std::less<T>());
        }
        void        merge(list&& x) { merge(x); }
        template <class Compare>
        void        merge (list& x, Compare comp);
        template <class Compare>
        void        merge(list&& x, Compare comp) { merge(x, comp); }

       private:
        /***********************辅助工具*****************************/
        link_type   get_node() { return node_allocator.allocate(1); }           // 配置一个节点并传回
        void        put_node(link_type p) { node_allocator.deallocate(p); }     // 释放一个节点
        template <class... Args>
        link_type   create_node(Args&&... args);                                // 分配并构造一个节点
        void        destroy_node(link_type p);
        void        destroy_node(iterator p) { return destroy_node(p.node_ptr); }
        void        empty_initialize();                                         // 产生一个空链表
        void        allocate_and_fill(size_type n, const value_type& val);
        template <class InputIterator>
        void        allocate_and_copy(InputIterator first, InputIterator last);
        void        destroy_and_deallocate_all();
        iterator    insert_one(const_iterator pos, const value_type& val);
        iterator    insert_fill(const_iterator pos, size_type n, const value_type& val);
        template <class InputIterator>
        iterator    insert_copy(const_iterator position, InputIterator first, InputIterator last);
        void        link_node_back(iterator pos, link_type node);
        void        link_node(link_type front, link_type back);
        template <class InputIterator>
        void        copy_aux(InputIterator first, InputIterator last);
        void        transfer(const_iterator position, iterator first, iterator last);
    };

    /*-------------------------------部分函数定义------------------------------------*/
    // -------------------- operator=
    template <class T, class Alloc>
    list<T, Alloc>&
    list<T, Alloc>::operator=(const list& x){
        if(&x != this){
            copy_aux(x.begin(), x.end());
        }
        return *this;
    }

    // -------------------- modifiers
    template <class T, class Alloc>
    void
    list<T, Alloc>::assign(size_type n, const value_type& val){
        if (n == 0) return;
        iterator cur = begin();
        for (; cur != end() && n != 0; ++cur, --n){
            *cur = val;
        }
        if(cur != end()){
            erase(cur, end());
        }
        else if(n != 0){
            insert(cur, n, val);
        }
    }

    template <class T, class Alloc>
    template <class... Args>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::emplace(const_iterator position, Args&&... args){
        link_type node = create_node(std::forward<Args>(args)...);
        link_node_back(position.node_ptr->prev, node);
        __size()++;
        return node;
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert(const_iterator position, value_type&& val){
        link_type node = create_node(std::move(val));
        link_node_back(position.node_ptr->prev, node);
        __size()++;
        return node;
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::erase(const_iterator position){
        if (position == end()) throw;
        iterator prev(position.node_ptr->prev);
        iterator next(position.node_ptr->next);
        link_node(prev.node_ptr, next.node_ptr);
        destroy_node(position.node_ptr);
        --__size();
        return next;
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::erase(const_iterator first, const_iterator last){
        iterator prev = first.node_ptr->prev;
        iterator node_to_dty((first).node_ptr);
        // for (; first != last; ++first){
        //     std::cout << *node_to_dty;
        //     destroy_node(node_to_dty);
        //     node_to_dty.node_ptr = first.node_ptr;
        //     --__size();
        //     std::cout << 'a';
        // }
        while ( first != last){
            ++first;
            destroy_node(node_to_dty);
            node_to_dty.node_ptr = first.node_ptr;
            --__size();
        }
        link_node(prev.node_ptr, last.node_ptr);
        return last.node_ptr;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::swap(list& x){
        link_type tmp = x.__node_ptr();
        x.__node_ptr() = __node_ptr();
        __node_ptr() = tmp;
        __size() ^= x.__size();
        x.size() ^= __size();
        __size() ^= x.__size();
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::resize(size_type n, const value_type& val){
        if (__size() == n) return;
        else if(__size() > n){
            iterator start = end();
            for (; n != __size(); ++n){
                --start;
            }
            erase(start, end());
        }
        else{
            insert(end(), n - __size(), val);
        }
    }

    // -------------------- operations
    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list& x){
        if (this == &x) return;
        transfer(position, x.begin(), x.end());
        __size() += x.__size();
        x.__size() = 0;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list&& x){
        return splice(position, x);
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list& x, const_iterator i){
        i.node_ptr->prev->next = i.node_ptr->next;
        i.node_ptr->next->prev = i.node_ptr->prev;
        position.node_ptr->prev->next = i.node_ptr;
        i.node_ptr->prev = position.node_ptr->prev;
        position.node_ptr->prev = i.node_ptr;
        i.node_ptr->next = position.node_ptr;
        ++__size();
        --x.__size();
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list&& x, const_iterator i){
        splice(position, x, i);
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list& x, const_iterator first, const_iterator last){
        const size_type len = std::distance(first, last);
        transfer(position, first.node_ptr, last.node_ptr);
        __size() += len;
        x.__size() -= len;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::splice(const_iterator position, list&& x, const_iterator first, const_iterator last){
        splice(position, x, first, last);
    }

    template <class T, class Alloc>
    template <class Predicate>
    void
    list<T, Alloc>::remove_if(Predicate pred){
        iterator next = begin();
        iterator check = begin();
        while(check != end()){
            ++next;
            if (pred(*check)){
                erase(check);
            }
            check = next;
        }
    }

    template <class T, class Alloc>
    template <class Predicate>
    void
    list<T, Alloc>::unique(Predicate pred){
        iterator ref = begin();
        iterator check = begin();
        ++check;
        while(check != end()){            
            if(pred(*ref, *check)){
                erase(check);
                check = ref;
            }
            else{
                ++ref;
            }
            ++check;
        }
    }

    template <class T, class Alloc>
    template <class Compare>
    void
    list<T, Alloc>::merge(list& x, Compare comp){
        if (this == &x) return;
        iterator first_1 = begin();
        iterator last_1 = end();
        iterator first_2 = x.begin();
        iterator last_2 = x.end();
        while(first_1 != last_1 && first_2 != last_2){
            if(comp(*first_2, *first_1)){
                iterator until = first_2;
                ++until;
                while(until != last_2 && comp(*until, *first_1)){
                    ++until;
                }
                transfer(first_1, first_2, until);
                first_2 = until;
            }
            ++first_1;
        }
        if(first_2 != last_2){
            transfer(last_1, first_2, last_2);
        }
        __size() += x.__size();
        x.__size() = 0;
    }

    // -------------------- 辅助工具
    template <class T, class Alloc>
    template <class... Args>
    typename list<T, Alloc>::link_type
    list<T, Alloc>::create_node(Args&&... args){
        link_type node_get_ptr = get_node();
        try{
            data_allocator.construct(&(node_get_ptr->data), std::forward<Args>(args)...);
            node_get_ptr->prev = nullptr;
            node_get_ptr->next = nullptr;
        }
        catch(...){
            put_node(node_get_ptr);
            throw;
        }
        return node_get_ptr;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::destroy_node(link_type p){
        data_allocator.destroy(&(p->data));
        put_node(p);
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::empty_initialize(){
        __node_ptr() = get_node();
        __node_ptr()->next = __node_ptr();
        __node_ptr()->prev = __node_ptr();
        __size() = 0;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::allocate_and_fill(size_type n, const value_type& val){
        __node_ptr() = get_node();
        __size() = 0;
        if (n == 0) return;
        iterator cur(__node_ptr());
        try{
            size_type count = 0;   
            for (; count < n; ++count){
                link_type tmp = create_node(val);
                cur.node_ptr->next = tmp;
                ++cur;
            }
            cur.node_ptr->next = __node_ptr();
            cur = __node_ptr()->next;
            cur.node_ptr->prev = __node_ptr();
            for (; cur != __node_ptr(); ++cur){
                cur.node_ptr->next->prev = cur.node_ptr;
            }
            __size() = n;
        }
        catch(...){
            iterator node_to_dty(__node_ptr());
            iterator next = node_to_dty.node_ptr->next;
            while(node_to_dty != cur){
                destroy_node(node_to_dty);
                node_to_dty = next;
                ++next;
            }
            destroy_node(cur);
            __node_ptr() = nullptr;
            throw;
        }
        // try{
        //     for (; n > 0; --n){
        //         link_type node_tmp_p = get_node();
        //         data_allocator.construct(&*(node_tmp_p->data), val);
        //         node_tmp_p->prev = aux;
        //         aux = aux->next = node_tmp_p;
        //         ++__size();
        //     }
        //     __node_ptr()->prev = aux;
        //     aux->next = __node_ptr();
        // }
        // catch(...){
        //     link_type next = __node_ptr();
        //     link_type node_to_dtr = __node_ptr();
        //     while(next != aux){
        //         next = next->next;
        //         destroy_node(node_to_dtr);
        //         node_to_dtr = next;
        //     }
        //     destroy_node(aux);
        //     destroy_node(__node_ptr());
        //     __node_ptr() = nullptr;
        //     __size() = 0;
        //     throw;
        // }
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void
    list<T, Alloc>::allocate_and_copy (InputIterator first, InputIterator last){
        __node_ptr() = get_node();
        __size() = 0;
        iterator cur(__node_ptr());
        try{
            for (; first != last; ++first, ++cur){
                link_type tmp = create_node(*first);
                link_node_back(cur, tmp);
                ++__size();
            }
        }
        catch(...){
            iterator prev = cur.node_ptr->prev;
            while(prev != __node_ptr()){
                destroy_node(cur);
                cur = prev;
                prev = cur.node_ptr->prev;
                --__size();
            }
            destroy_node(__node_ptr());
            __node_ptr() = nullptr;
            throw;
        }
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::destroy_and_deallocate_all(){
        iterator cur(__node_ptr()->next);
        iterator next(cur.node_ptr->next);
        while(cur != __node_ptr()){
            destroy_node(cur);
            cur = next;
            next = cur.node_ptr->next;
        }
        destroy_node(__node_ptr());
        __node_ptr() = nullptr;
        __size() = 0;
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert_one(const_iterator pos, const value_type& val){
        link_type new_node = create_node(val);
        new_node->next = pos.node_ptr;
        new_node->prev = pos.node_ptr->prev;
        pos.node_ptr->prev->next = new_node;
        pos.node_ptr->prev = new_node;
        __size()++;
        return iterator(new_node);
    }

    template <class T, class Alloc>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert_fill(const_iterator pos, size_type n, const value_type& val){
        if (n == 0) return iterator();
        else if(n == 1)
            return insert_one(pos, val);
        iterator start(pos.node_ptr->prev);
        iterator end(pos.node_ptr);
        iterator cur(start);
        try{
            size_type count = 0;   
            for (; count < n; ++count){
                link_type tmp = create_node(val);
                cur.node_ptr->next = tmp;
                ++cur;
            }
            cur.node_ptr->next = end.node_ptr;
            end.node_ptr->prev = cur.node_ptr;
            cur.node_ptr = start.node_ptr;
            cur.node_ptr->next->prev = cur.node_ptr;
            ++cur;
            while (cur != end){
                cur.node_ptr->next->prev = cur.node_ptr;
                ++cur;
            }
            __size() += n;
        }
        catch(...){
            iterator node_to_dty(start.node_ptr->next);
            iterator next = node_to_dty.node_ptr->next;
            while(node_to_dty != cur){
                destroy_node(node_to_dty);
                node_to_dty = next;
                ++next;
            }
            destroy_node(cur);
            start.node_ptr->next = end.node_ptr;
            throw;
        }
        return start.node_ptr->next;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert_copy(const_iterator position, InputIterator first, InputIterator last){
        iterator start(position.node_ptr->prev);
        iterator end(position.node_ptr);
        // InputIterator cur = first;
        iterator cur = start;
        size_type old_size = __size();
        try{
            for (; first != last; ++first, ++cur){
                link_type node = create_node(*first);
                cur.node_ptr->next = node;
                node->prev = cur.node_ptr;
                ++__size();
            }
            cur.node_ptr->next = end.node_ptr;
            end.node_ptr->prev = cur.node_ptr;
        }
        catch(...){
            while(cur != start){
                --cur;
                destroy_node(cur.node_ptr->next);
            }
            start.node_ptr->next = end;
            __size() = old_size;
            throw;
        }
        return start.node_ptr->next;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::link_node_back(iterator pos, link_type node){
        node->next = pos.node_ptr->next;
        node->prev = pos.node_ptr->prev;
        pos.node_ptr->next->prev = node;
        pos.node_ptr->next = node;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::link_node(link_type front, link_type back){
        front->next = back;
        back->prev = front;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void
    list<T, Alloc>::copy_aux(InputIterator first, InputIterator last){
        iterator cur = begin();
        iterator end = end();
        while(cur != end() && first != last){
            *cur = *first;
            ++first;
            ++cur;
        }
        if (first == last) {
            erase(cur, end);
            return;
        }       
        else if(cur == end()){
            insert_copy(cur, first, last);
        }
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::transfer(const_iterator position, iterator first, iterator last){
        iterator start(position.node_ptr->prev);
        iterator end(position.node_ptr);
        iterator tail(last.node_ptr->prev);
        first.node_ptr->prev->next = last.node_ptr;
        last.node_ptr->prev = first.node_ptr->prev;

        start.node_ptr->next = first.node_ptr;       
        first.node_ptr->prev = start.node_ptr;
        end.node_ptr->prev = tail.node_ptr;
        tail.node_ptr->next = end.node_ptr;
    }

} // namespace

#endif