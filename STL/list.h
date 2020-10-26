#ifndef _POCKET_LIST_H_
#define _POCKET_LIST_H_

/*
** list
** 
*/

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
        __list_iterator() = default;
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
        
        self& operator++(int){                      // 后置版++
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            node_ptr = (*node_ptr).prev;
            return *this;
        }
        self& operator--(int){
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
        __list_const_iterator() = default;
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
        
        self& operator++(int){                      // 后置版++
            self tmp = *this;
            ++*this;
            return tmp;
        }

        self& operator--(){
            node_ptr = (*node_ptr).prev;
            return *this;
        }
        self& operator--(int){
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
        /*************** iterator functions *****************/
        iterator        begin() noexcept { return __node_ptr()->next; }
        const_iterator  begin() const noexcept { return __node_ptr()->next; }
        iterator        end() noexcept { return __node_ptr(); }
        const_iterator  end() const noexcept { return __node_ptr(); }

       public:
        /************************ modifiers ***********************/
        // insert
        iterator insert(const_iterator position, const value_type& val) { return insert_one(position, val); }
        iterator insert(const_iterator position, size_type n, const value_type& val) { return insert_fill(position, n, val); }
        template <class InputIterator>
        iterator insert(const_iterator position, InputIterator first, InputIterator last) { return insert_copy(position, first, last); }
        iterator insert(const_iterator position, value_type&& val);
        iterator insert(const_iterator position, std::initializer_list<value_type> il) { return insert_copy(il.begin(), il.end()); }
        // erase
        iterator erase (const_iterator position);
        iterator erase (const_iterator first, const_iterator last);

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
        template <class InputIterator>
        void        copy_aux(InputIterator first, InputIterator last);
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
    typename list<T, Alloc>::iterator
    list<T, Alloc>::insert(const_iterator position, value_type&& val){
        link_type node = create_node(std::move(val));
        link_node_back(position.node_ptr->prev, node);
        __size()++;
        return node;
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
        if (n == 0) return iterator(pos.node_ptr);
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
            cur = start->next;
            for (; cur != end; ++cur){
                cur.node_ptr->next->prev = cur.node_ptr;
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
    template <class InputIterator>
    void
    list<T, Alloc>::copy_aux(InputIterator first, InputIterator last){
        iterator cur = begin();
        iterator end = end();
        if (first == last) {
            erase(cur, end);
            return;
        }
        while(cur.node_ptr != __node_ptr() && first != last){
            *cur = *first;
            ++first;
            ++cur;
        }
        if(first != last){
            insert_copy(cur, first, last);
        }
    }

}

#endif