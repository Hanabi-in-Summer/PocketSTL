#ifndef _POCKET_HASHTABLE_H_
#define _POCKET_HASHTABLE_H_

/*
** hashtable
** unordered_set, unordered_map 的底层
*/

#include <cstddef>
#include <stdexcept>
#include "allocator.h"
#include "algobase.h"
#include "vector.h"

#define HASHTABLE __hashtable<Value, Key, HashFcn, ExtractKey, EqualKey, Alloc>

namespace pocket_stl{
    #if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
    #define SYSTEM_64 1
    #else
    #define SYSTEM_32 1
    #endif

    #ifdef SYSTEM_64
    static const int __stl_num_primes = 99;
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 63 - 1)
    static constexpr size_t __stl_prime_list[__stl_num_primes] = {
        101ull,                 173ull,                 263ull,                 397ull, 
        599ull,                 907ull,                 1361ull,                2053ull, 
        3083ull,                4637ull,                6959ull,                10453ull,
        15683ull,               23531ull,               35311ull,               52967ull, 
        79451ull,               119179ull,              178781ull,              268189ull, 
        402299ull,              603457ull,              905189ull,              1357787ull,
        2036687ull,             3055043ull,             4582577ull,             6873871ull, 
        10310819ull,            15466229ull,            23199347ull,            34799021ull, 
        52198537ull,            78297827ull,            117446801ull,           176170229ull,
        264255353ull,           396383041ull,           594574583ull,           891861923ull, 
        1337792887ull,          2006689337ull,          3010034021ull,          4515051137ull, 
        6772576709ull,          10158865069ull,         15238297621ull,         22857446471ull, 
        34286169707ull,         51429254599ull,         77143881917ull,         115715822899ull,        
        173573734363ull,        260360601547ull,        390540902329ull,        585811353559ull,        
        878717030339ull,        1318075545511ull,       1977113318311ull,       2965669977497ull,       
        4448504966249ull,       6672757449409ull,       10009136174239ull,      15013704261371ull,      
        22520556392057ull,      33780834588157ull,      50671251882247ull,      76006877823377ull,      
        114010316735089ull,     171015475102649ull,     256523212653977ull,     384784818980971ull,     
        577177228471507ull,     865765842707309ull,     1298648764060979ull,    1947973146091477ull,    
        2921959719137273ull,    4382939578705967ull,    6574409368058969ull,    9861614052088471ull,    
        14792421078132871ull,   22188631617199337ull,   33282947425799017ull,   49924421138698549ull,   
        74886631708047827ull,   112329947562071807ull,  168494921343107851ull,  252742382014661767ull,  
        379113573021992729ull,  568670359532989111ull,  853005539299483657ull,  1279508308949225477ull, 
        1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull, 6477510814055453699ull, 
        9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
    };

    #else
    static const int __stl_num_primes = 44
    // 1. start with p = 101
    // 2. p = next_prime(p * 1.7)
    // 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
    // 4. end with p = prev_prime(2 << 31 - 1)
    static constexpr size_t __stl_prime_list[__stl_num_primes] = {
        101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
        10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
        402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
        10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
        176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
        2006689337u, 3010034021u, 4294967291u,
    };
    #endif


    // static const int __stl_num_primes = 28;
    // static const unsigned long __stl_prime_list[__stl_num_primes] = {
    //     53ul,         97ul,         193ul,       389ul,       769ul,
    //     1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    //     49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    //     1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    //     50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
    //     1610612741ul, 3221225473ul, 4294967291ul
    // };

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
        
//         explicit unordered_set ( size_type n = /* see below */,
//                          const hasher& hf = hasher(),
//                          const key_equal& eql = key_equal(),
//                          const allocator_type& alloc = allocator_type() );
// explicit unordered_set ( const allocator_type& alloc );
// range (2)	
// template <class InputIterator>
//          unordered_set ( InputIterator first, InputIterator last,
//                          size_type n = /* see below */,
//                          const hasher& hf = hasher(),
//                          const key_equal& eql = key_equal(),
//                          const allocator_type& alloc = allocator_type() );
// copy (3)	
// unordered_set ( const unordered_set& ust );
// unordered_set ( const unordered_set& ust, const allocator_type& alloc );
// move (4)	
// unordered_set ( unordered_set&& ust );
// unordered_set ( unordered_set&& ust, const allocator_type& alloc );
// initializer list (5)	
// unordered_set ( initializer_list<value_type> il,
//                 size_type n = /* see below */,
//                 const hasher& hf = hasher(),
//                 const key_equal& eql = key_equal(),
//                 const allocator_type& alloc = allocator_type() );
        

    public:
        std::pair<iterator, bool> insert_unique(const value_type& obj);     // 插入元素，不允许重复
        iterator insert_equal(const value_type& obj);                       // 插入元素，允许重复
        void resize(size_type num_elements_hint);       // 判断是否需要重建表格，如需要就扩充
        void clear();
        void copy_from(const __hashtable& ht);

    private:
        /************************** 辅助工具 *****************************/
        inline size_type __stl_next_prime(size_type n) noexcept;
        size_type max_bucket_count() const noexcept { return __stl_prime_list[__stl_num_primes - 1]; }
        node* new_node(const value_type& obj);
        void delete_node(node* n);
        void initialize_buckets(size_type n);
        size_type next_size(size_type n) const { return __stl_next_prime(n); }
        std::pair<iterator, bool> insert_unique_noresize(const value_type& obj);    // 在不需要重建表格的情况下插入新节点，键值不允许重复
        iterator insert_equal_noresize(const value_type& obj);
        size_type bkt_num(const value_type& obj, size_t n) const { return bkt_num_key(get_key(obj), n); }   // 确定元素落脚处，接收实值和 buckets 个数
        size_type bkt_num(const value_type& obj) const { return bkt_num(get_key(obj)); }
        size_type bkt_num_key(const key_type& key) const { return bkt_num_key(key, buckets.size()); }
        size_type bkt_num_key(const key_type& key, size_t n) const { return hash(key) % n; }
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
    typename HASHTABLE::iterator
    HASHTABLE::insert_equal(const value_type& obj){
        resize(num_elements() + 1);
        return insert_equal_noresize(obj);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::resize(size_type num_elements_hint){
        const size_type old_n = buckets.size();
        if (num_elements_hint > old_n) {  //确定是否需要重新配置，新增后元素个数大于 buckets 大小，则扩充
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

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::clear(){
        for (size_type i = 0; i < buckets.size(); ++i){
            node* cur = buckets[i];
            while(cur != nullptr){
                node* next = cur->next;
                delete_node(cur);
                cur = next;
            }
            buckets[i] = nullptr;
        }
        num_elements() = 0;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::copy_from(const __hashtable& ht){
        // 若己方空间大于对方，则不变，否则增大
        buckets.reserve(ht.buckets.size());
        buckets.insert(buckets.end(), ht.buckets.size(), static_cast<node*>(nullptr));
        try{
            for (size_type i = 0; i < ht.buckets.size(); ++i){
                if(const node* cur = ht.buckets[i]){
                    node* copy = new_node(cur->val);
                    buckets[i] = copy;
                    for (node* next = cur->next; next; cur = next, next = cur->next){
                        copy->next = new_node(next->val);
                        copy = copy->next;
                    }
                }
            }
            num_elements() = ht.num_elements();
        }
        catch(...){
            clear();
        }
    }

    // -------------------- 辅助工具
    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    inline typename HASHTABLE::size_type 
    HASHTABLE::__stl_next_prime(size_type n) noexcept {
        const size_type* first = __stl_prime_list;
        const size_type* last = __stl_prime_list + __stl_num_primes;
        const size_type* pos = std::lower_bound(first, last, n);
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

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator
    HASHTABLE::insert_equal_noresize(const value_type& obj){
        const size_type n = bkt_num(obj);
        node* first = buckets[n];
        // 如果 buckets[n] 已被占用， 则 first 为非空指针，进入循环
        // 遍历链表
        for (node* cur = first; cur; cur = cur->next){
            // 发现与链表中某键值相同则插入
            if(equals(get_key(cur->val), get_key(obj))){
                node* tmp = new_node(obj);
                tmp->next = cur->next;
                cur->next = tmp;
                ++num_elements();
                return iterator(tmp, this);
            }
        }
        // 未发现有重复键值
        node* tmp = new_node(obj);
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements();
        return iterator(tmp, this);
    }
}

#endif