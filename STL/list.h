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
    class __list_node{
        __list_node* prev;
        __list_node* next;
        T data;
    };

    template <class T, class Ref = T&, class Ptr = T*>
    class __list_iterator{
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
        link_type           __node_ptr;
        node_allocator_type node_allocator;
        allocator_type      data_allocator;

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
        template <class InputIterator>
        list(InputIterator first, InputIterator last);
        // **** copy ctor
        list(const list& x);
        // **** move ctor
        list(list&& x);
        // **** initializer list
        list(std::initializer_list<value_type> il);



    private:
        /***********************内存分配构造工具*****************************/
        link_type get_node() { return node_allocator.allcoate(1); }     // 配置一个节点并传回
        void put_node(link_type p) { node_allocator.deallocate(p); }    // 释放一个节点
        template <class... Args>
        link_type create_node(Args&&... args);
        void destroy_node(link_type p);
        void empty_initialize();                                        // 产生一个空链表
        void allocate_and_fill(size_type n, const value_type& val);
    };

    /*-------------------------------部分函数定义------------------------------------*/
    // -------------------- 内存分配构造工具
    template <class T, class Alloc>
    template <class... Args>
    typename list<T, Alloc>::link_type
    list<T, Alloc>::create_node(Args&&... args){
        link_type node_get_ptr = get_node();
        try{
            data_allocator.construct(&*(node_get_ptr->data), std::forward<Args>(args)...);
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
        data_allocator.destroy(p->data);
        put_node(p);
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::empty_initialize(){
        __node_ptr = get_node();
        __node_ptr->next = __node_ptr;
        __node_ptr->prev = __node_ptr;
    }

    template <class T, class Alloc>
    void
    list<T, Alloc>::allocate_and_fill(size_type n, const value_type& val){
        __node_ptr = get_node();
        link_type aux = __node_ptr;
        try{
            for (; n > 0; --n){
                link_type node_tmp_p = get_node();
                data_allocator.construct(&*(node_tmp_p->data), val);
                node_tmp_p->prev = aux;
                aux = aux->next = node_tmp_p;
            }
            __node_ptr->prev = aux;
            aux->next = __node_ptr;
        }
        catch(...){
            link_type next = __node_ptr;
            link_type node_to_dtr = __node_ptr;
            while(next != aux){
                next = next->next;
                destroy_node(node_to_dtr);
                node_to_dtr = next;
            }
            destroy_node(aux);
            __node_ptr = nullptr;
            throw;
        }
    }

}

#endif