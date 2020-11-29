#ifndef _POCKET_HASHTABLE_H_
#define _POCKET_HASHTABLE_H_

/*
** hashtable
** unordered_set, unordered_map 的底层
*/

#include <cstddef>
#include <stdexcept>
#include "allocator.h"
#include "vector.h"

namespace pocket_stl{
    #define HASHTABLE __hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>

    static const int __stl_num_primes = 28;
    static const unsigned long __stl_prime_list[__stl_num_primes] = {
        53ul,         97ul,         193ul,       389ul,       769ul,
        1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
        49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
        1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
        50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
        1610612741ul, 3221225473ul, 4294967291ul
    };

    template <class Value>
    struct __hashtable_node{
        __hashtable_node* next;
        Value val;
        __hashtable_node() : next(nullptr) {}
        __hashtable_node(const Value& v) : val(v), next(nullptr) {}
        __hashtable_node(const __hashtable_node& node) : next(node.next), val(node.val) {}
        __hashtable_node(__hashtable_node&& node) : next(node.next), val(std::move(node.next)) {
            node.next = nullptr;
        }
    };

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    class __hashtable;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    class __hashtable_iterator;

    template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey, class Alloc>
    class __hashtable_const_iterator;

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    class __hashtable_iterator{
    public:
        using hashtable         = __hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using iterator          = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using const_iterator    = __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using node              = __hashtable_node<Value>;

        using iterator_category = forward_iterator_tag;
        using value_type        = Value;
        using difference_type   = ptrdiff_t;
        using size_type         = size_t;
        using reference         = value_type&;
        using pointer           = value_type*;
    private:
        template <class, class, class, class, class, class> friend class __hashtable;
        template <class, class, class, class, class, class> friend class __hashtable_const_iterator;
        node* cur;
        hashtable* ht;
    public:
        __hashtable_iterator() = default;
        __hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
        __hashtable_iterator(const iterator& rhs) : cur(rhs.cur), ht(rhs.ht) {}

        reference operator*() const { return cur->val; }
        pointer operator->() const { return &(operator*()); }
        iterator& operator++() noexcept{
            const node* old = cur;
            cur = cur->next;
            if(!cur){
                size_type bucket = ht->bkt_num(old->val);
                while(!cur && ++bucket < ht->buckets.size()){
                    cur = ht->buckets[bucket];
                }
            }
            return *this;
        }
        iterator operator++(int) noexcept{
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
    };

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    class __hashtable_const_iterator{
    public:
        using hashtable         = __hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using iterator          = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using const_iterator    = __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using node              = __hashtable_node<Value>;

        using iterator_category = forward_iterator_tag;
        using value_type        = Value;
        using difference_type   = ptrdiff_t;
        using size_type         = size_t;
        using reference         = const value_type&;
        using pointer           = const value_type*;
    private:
        template <class, class, class, class, class, class> friend class __hashtable;
        template <class, class, class, class, class, class> friend class __hashtable_const_iterator;
        node* cur;
        hashtable* ht;
    public:
        __hashtable_const_iterator() = default;
        __hashtable_const_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
        __hashtable_const_iterator(const iterator& rhs) : cur(rhs.cur), ht(rhs.ht) {}
        __hashtable_const_iterator(const const_iterator& rhs) : cur(rhs.cur), ht(rhs.ht) {}

        reference operator*() const { return cur->val; }
        pointer operator->() const { return &(operator*()); }
        iterator& operator++() noexcept{
            const node* old = cur;
            cur = cur->next;
            if(!cur){
                size_type bucket = ht->bkt_num(old->val);
                while(!cur && ++bucket < ht->buckets.size()){
                    cur = ht->buckets[bucket];
                }
            }
            return *this;
        }
        iterator operator++(int) noexcept{
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
    };

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc = allocator<Value>>
    class __hashtable{
    public:
        using hasher        = HashFcn;
        using key_equal     = EqualKey;
        using allocator_type = Alloc;
        using size_type     = size_t;
        using value_type    = Value;
        using key_type      = Key;
        using reference     = Value&;
        using pointer       = Value*;
        using iterator      = __hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;
        using const_iterator = __hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>;

    private:
        hasher hash;
        key_equal equals;
        ExtractKey get_key;

        using node                  = __hashtable_node<Value>;
        using node_allocator_type   = typename Alloc::template rebind<Value*>::other;

        vector<node*, Alloc> buckets;
        compressed_pair<size_type, node_allocator_type> num_and_node_allocator;
        compressed_pair<size_type, node_allocator_type>&        node_allocator() noexcept{ return num_and_node_allocator; }
        const compressed_pair<size_type, node_allocator_type>&  node_allocator() const noexcept{ return num_and_node_allocator; }
        size_type&          num_elements() noexcept { return num_and_node_allocator.data; }
        const size_type&    num_elements() const noexcept { return num_and_node_allocator.data; }

    public:
        __hashtable(size_type n, const HashFcn& hf, const EqualKey& eql)
                : hash(hf), equals(eql), get_key(ExtractKey()){
            num_elements() = 0;
            initialize_buckets(n);
        }

    public:
        std::pair<iterator, bool> insert_unique(const value_type& obj);
        void resize(size_type num_elements_hint);

    private:
        /************************** 辅助工具 *****************************/
        inline unsigned long __stl_next_prime(unsigned long n) noexcept;
        size_type max_bucket_count() const noexcept { return __stl_prime_list[__stl_num_primes - 1]; }
        node* new_node(const value_type& obj);
        void delete_node(node* n);
        void initialize_buckets(size_type n);
        size_type next_size(size_type n) const { return __stl_next_prime(n); }
        std::pair<iterator, bool> insert_unique_noresize(const value_type& obj);
    };

    /*-------------------------------部分函数定义------------------------------------*/
    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, bool>
    HASHTABLE::insert_unique(const value_type& obj){
        resize(num_elements() + 1);
        return insert_unique_noresize(obj);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::resize(size_type num_elements_hint){
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n) {  //确定是否需要重新配置
            const size_type n = next_size(num_elements_hint);
            if (n > old_n) {
                vector<node*> tmp(n, static_cast<node*>(nullptr)); // 设立新的 buckets
                try {
                    // 以下处理每一个旧的 bucket
                    for (size_type bucket = 0; bucket < old_n; ++bucket) {
                        node* first = buckets[bucket];
                        while (first) {
                            // 找出节点落在哪个新的bucket内
                            size_type new_bucket = bkt_num(first->val, n);
                            buckets[bucket] = first->next; 
                            first->next = tmp[new_bucket];
                            tmp[new_bucket] = first;
                            first = buckets[bucket];
                        }
                    }
                    buckets.swap(tmp);
                }
                catch (...) {
                    throw;
                    //
                }
            }
        }
    }

    // -------------------- 辅助工具
    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    inline unsigned long 
    HASHTABLE::__stl_next_prime(unsigned long n) noexcept {
        const unsigned long *first = __stl_prime_list;
        const unsigned long *last = __stl_prime_list + __stl_num_primes;
        const unsigned long *pos = std::lower_bound(first, last, n);
        return pos == last ? *(last - 1) : *pos;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::node*
    HASHTABLE::new_node(const value_type& obj){
        node* n = node_allocator().allocate();
        n->next = nullptr;
        try{
            node_allocator().construct(&n->val, obj);
            return n;
        }
        catch(...){
            node_allocator().deallocate(n);
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::delete_node(node* n){
        destroy(&n->val);
        node_allocator().deallocate(n);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::initialize_buckets(size_type n){
        const size_type n_buckets = next_size(n);
        buckets.reverse(n_buckets);
        buckets.insert(buckets.end(), n_buckets, (node*)nullptr);
        num_elements() = 0;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, bool>
    HASHTABLE::insert_unique_noresize(const value_type& obj){
        const size_type n = bkt_num(obj);  // 决定 obj 位于哪个bucket
        node *first = buckets[n];       // 令 first 指向 bucket 对应串行头部
        for (node *cur = first; cur; cur = cur->next) {
            if (equals(get_key(cur->val), get_key(obj)))  // 存在相同键值，拒绝插入
                return std::pair<iterator, bool>(iterator(cur, this), false);
        }
        // 在 bucket 对应串行头部 创造新节点
        node *temp = new_node(obj);
        temp->next = first;
        buckets[n] = temp;
        ++num_elements();
        return std::pair<iterator, bool>(iterator(temp, this), true);
    }
}

#endif