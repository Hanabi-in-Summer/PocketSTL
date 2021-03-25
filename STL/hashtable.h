#ifndef _POCKET_HASHTABLE_H_
#define _POCKET_HASHTABLE_H_

/*
** hashtable
** unordered_set, unordered_map 的底层
*/

#include <cstddef>
#include <cmath>
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
    
    // HashFcn : HashFunction的函数型别
    // ExtractKey : 从节点中取出键值的方法
    // EqualKey : 判断键值相同与否
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
        node* cur;          // 迭代器目前所指节点
        hashtable* ht;      // 指向容器
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
        float mlf; // max load factor
        compressed_pair<size_type, node_allocator_type> num_and_node_allocator;
        compressed_pair<size_type, node_allocator_type>&        node_allocator() noexcept{ return num_and_node_allocator; }
        const compressed_pair<size_type, node_allocator_type>&  node_allocator() const noexcept{ return num_and_node_allocator; }
        size_type&          num_elements() noexcept { return num_and_node_allocator.data; }
        const size_type&    num_elements() const noexcept { return num_and_node_allocator.data; }

    public:
        explicit __hashtable(size_type n, const HashFcn& hf = hasher(), const EqualKey& eql = key_equal())
                : hash(hf), equals(eql), mlf(1.0f), get_key(ExtractKey()){
            num_elements() = 0;
            initialize_buckets(n);
        }

        __hashtable(const __hashtable& rhs)
            :  mlf(1.0f), hash(rhs.hash), equals(rhs.equals), get_key(rhs.get_key){
            num_elements() = 0;
            copy_from(rhs);
        }

        __hashtable(__hashtable&& rhs) noexcept
                : buckets(std::move(rhs.buckets)), mlf(rhs.mlf), hash(rhs.hash), equals(rhs.equals), get_key(rhs.get_key){
            num_elements() = rhs.num_elements();
            rhs.num_elements = 0;
            rhs.mlf = 0.0f;
        }

        template <class InputIterator, class = typename std::enable_if<
                                    !std::is_integral<InputIterator>::value
                                    >::type>
        __hashtable(InputIterator first, InputIterator last, size_type n, 
                const HashFcn& hf = hasher(), const EqualKey& eql = key_equal())
                : mlf(1.0f), hash(hf), equals(eql), get_key(ExtractKey()){
            const size_type len = std::distance(first, last);
            const size_type bucket_nums = __stl_next_prime(len);
            try{
                buckets.reserve(bucket_nums);
                buckets.assign(bucket_nums, nullptr);
                num_elements() = len;
            }
            catch(...){
                num_elements() = 0;
                throw;
            }
        }

        __hashtable& operator=(const __hashtable& rhs){
            if(this != &rhs){
                clear();
                hash = rhs.hash;
                equals = rhs.equals;
                get_key = rhs.get_key;
                copy_from(rhs);
            }
            return *this;
        }

        __hashtable& operator=(__hashtable&& rhs) noexcept{
            __hashtable tmp(std::move(rhs));
            swap(tmp);
            return *this;
        }

        ~__hashtable(){
            clear();
        }      

    public:
        size_type   size() { return num_elements(); }
        size_type   max_size() const { return size_type(-1); }
        bool        empty() const { return size() == 0; }
        void        swap(__hashtable& rhs);

        iterator        begin() noexcept;
        const_iterator  begin() const noexcept;
        iterator        end() noexcept { return iterator(nullptr, this); }
        const_iterator  end() const noexcept { return const_iterator(nullptr, this); }

        size_type bucket_count() const noexcept { return buckets.size(); }
        size_type max_bucket_count() const noexcept { return __stl_prime_list[(int)__stl_num_primes - 1]; }
        size_type bucket_size ( size_type n ) const noexcept;
        size_type bucket(const key_type& k) const { return bkt_num_key(k); }
        
        void clear() noexcept;

        iterator        find(const key_type& k);
        const_iterator  find(const key_type& k) const;
        size_type       count(const key_type& k) const;
        std::pair<iterator, iterator> equal_range_equal(const key_type& k);
        std::pair<const_iterator, const_iterator> equal_range_equal(const key_type& k) const;
        std::pair<iterator, iterator> equal_range_unique(const key_type& k);
        std::pair<const_iterator, const_iterator> equal_range_unique(const key_type& k) const;

        std::pair<iterator, bool> insert_unique(const value_type& obj);  // 插入元素，不允许重复
        iterator insert_equal(const value_type& obj);                    // 插入元素，允许重复
        template <class... Args>
        iterator emplace_equal(Args&&... args);
        template <class... Args>
        std::pair<iterator, bool> emplace_unique(Args&&... args);
        template <class InputIterator>
        void        insert_equal_copy(InputIterator first, InputIterator last);
        template <class InputIterator>
        void        insert_unique_copy(InputIterator first, InputIterator last);
        iterator    erase ( const_iterator position );
        size_type   erase(const key_type& k);
        iterator    erase(const_iterator first, const_iterator last);

        float load_factor() const noexcept;
        float max_load_factor() const noexcept { return mlf; }
        void max_load_factor(float z);
        void rehash(size_type n) { resize(n * mlf); }
        void reserve(size_type n) { rehash(static_cast<size_type>(ceil(n / max_load_factor()))); }

        hasher hash_function() const { return hash; }
        key_equal key_eq() const { return equals; }
        allocator_type get_allocator() const noexcept { return allocator_type(); }

    private:
        /************************** 辅助工具 *****************************/
        inline size_type __stl_next_prime(size_type n) noexcept;
        template <class... Args>
        node* new_node(Args&&... arg);
        void delete_node(node* n);
        void initialize_buckets(size_type n);
        void copy_from(const __hashtable& ht);
        size_type next_size(size_type n) const { return __stl_next_prime(n); }
        void resize(size_type num_elements_hint);                        // 判断是否需要重建表格，如需要就扩充
        std::pair<iterator, bool> insert_unique_noresize(const value_type& obj);    // 在不需要重建表格的情况下插入新节点，键值不允许重复
        iterator insert_equal_noresize(const value_type& obj);

        std::pair<iterator, bool> insert_node_unique(node* ptr);
        iterator insert_node_equal(node* ptr);
        size_type bkt_num(const value_type& obj, size_t n) const { return bkt_num_key(get_key(obj), n); }   // 确定元素落脚处，接收实值和 buckets 个数
        size_type bkt_num(const value_type& obj) const { return bkt_num(get_key(obj)); }
        size_type bkt_num_key(const key_type& key) const { return bkt_num_key(key, buckets.size()); }
        size_type bkt_num_key(const key_type& key, size_t n) const { return hash(key) % n; }
        void erase_bucket(const size_type n, node* first, node* last);
        void erase_bucket(const size_type n, node* last);
    };

    /*-------------------------------部分函数定义------------------------------------*/
    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::swap(__hashtable& rhs){
        std::swap(hash, rhs.hash);
        std::swap(equals, rhs.equals);
        std::swap(get_key, rhs.get_key);
        buckets.swap(rhs.buckets);
        std::swap(num_elements(), rhs.num_elements());
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator
    HASHTABLE::begin() noexcept{
        for (size_type n = 0; n < buckets.size(); ++n){
            if (buckets[n]) return iterator(buckets[n], this);
        }
        return end();
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::const_iterator
    HASHTABLE::begin() const noexcept{
        for (size_type n = 0; n < buckets.size(); ++n){
            if (buckets[n]) return const_iterator(buckets[n], this);
        }
        return end();
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::size_type 
    HASHTABLE::bucket_size(size_type n) const noexcept{
        size_type result = 0;
        for (auto cur = buckets[n]; cur; cur = cur->next){
            ++result;
        }
        return result;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator
    HASHTABLE::find(const key_type& k){
        size_type n = bkt_num_key(k);
        node* first;
        for (first = buckets[n]; first && !equals(get_key(first->val), k); first = first->next);
        return iterator(first, this);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::const_iterator
    HASHTABLE::find(const key_type& k) const{
        size_type n = bkt_num_key(k);
        node* first;
        for (first = buckets[n]; first && !equals(get_key(first->val), k); first = first->next);
        return const_iterator(first, this);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::size_type 
    HASHTABLE::count ( const key_type& k ) const{
        const size_type n = bkt_num_key(k);
        size_type result = 0;
        for (const node* cur = buckets[n]; cur; cur = cur->next){
            if (equals(get_key(cur->val), k)) ++result;    
        }
        return result;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, typename HASHTABLE::iterator> 
    HASHTABLE::equal_range_equal(const key_type& k){
        const size_type n = bkt_num_key(k);
        for (node* first = buckets[n]; first; first = first->next){
            if(equals(get_key(first->val), k)){
                for (node* second = first->next; second; second = second->next){
                    if(!equals(get_key(second->val), k)){
                        return std::make_pair(iterator(first, this), iterator(second, this));
                    }
                }
                for (size_type m = n + 1; m < buckets.size(); ++m){
                    if(buckets[m]){
                        return std::make_pair(iterator(first, this), iterator(buckets[m], this));
                    }
                }
            }
        }
        return std::make_pair(end(), end());
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::const_iterator, typename HASHTABLE::const_iterator> 
    HASHTABLE::equal_range_equal(const key_type& k) const{
        const size_type n = bkt_num_key(k);
        for (node* first = buckets[n]; first; first = first->next){
            if(equals(get_key(first->val), k)){
                for (node* second = first->next; second; second = second->next){
                    if(!equals(get_key(second->val), k)){
                        return std::make_pair(const_iterator(first, this), const_iterator(second, this));
                    }
                }
                for (size_type m = n + 1; m < buckets.size(); ++m){
                    if(buckets[m]){
                        return std::make_pair(cosnt_iterator(first, this), const_iterator(buckets[m], this));
                    }
                }
            }
        }
        return std::make_pair(end(), end());
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, typename HASHTABLE::iterator>
    HASHTABLE::equal_range_unique(const key_type& k){
        const size_type n = bkt_num_key(k);
        for (node* first = buckets[n]; first; first = first->next){
            if(equals(get_key(first->val), k)){
                if(first->next){
                    return std::make_pair(iterator(first, this), iterator(first->next, this));
                }
                for (size_type m = n + 1; m < buckets.size(); ++m){
                    if (buckets[m]) return std::make_pair(iterator(first, this), iterator(buckets[m], this));
                }
                return std::make_pair(end(), end());
            }
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::const_iterator, typename HASHTABLE::const_iterator>
    HASHTABLE::equal_range_unique(const key_type& k) const{
        const size_type n = bkt_num_key(k);
        for (node* first = buckets[n]; first; first = first->next){
            if(equals(get_key(first->val), k)){
                if(first->next){
                    return std::make_pair(const_iterator(first, this), const_iterator(first->next, this));
                }
                for (size_type m = n + 1; m < buckets.size(); ++m){
                    if (buckets[m]) return std::make_pair(const_iterator(first, this), const_iterator(buckets[m], this));
                }
                return std::make_pair(end(), end());
            }
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    template <class... Args>
    std::pair<typename HASHTABLE::iterator, bool>
    HASHTABLE::emplace_unique(Args&&... args){
        auto n = new_node(std::forward<Args>(args)...);
        rehash(num_elements() + 1);
        return insert_node_unique(n);
    }


    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    template <class... Args>
    typename HASHTABLE::iterator
    HASHTABLE::emplace_equal(Args&&... args){
        auto n = new_node(std::forward<Args>(args)...);
        rehash(num_elements() + 1);
        return insert_node_equal(n);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, bool>
    HASHTABLE::insert_unique(const value_type& obj){
        rehash(num_elements() + 1);
        return insert_unique_noresize(obj);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator
    HASHTABLE::insert_equal(const value_type& obj){
        rehash(num_elements() + 1);
        return insert_equal_noresize(obj);
    }

     template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    template <class InputIterator>
    void 
    HASHTABLE::insert_unique_copy(InputIterator first, InputIterator last){
        size_type n = std::distance(first, last);
        rehash(num_elements() + n);
        for (; n > 0; --n, ++first){
            insert_unique_noresize(*first);
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    template <class InputIterator>
    void 
    HASHTABLE::insert_equal_copy(InputIterator first, InputIterator last){
        size_type n = std::distance(first, last);
        rehash(num_elements() + n);
        for (; n > 0; --n, ++first){
            insert_equal_noresize(*first);
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::size_type 
    HASHTABLE::erase(const key_type& k){
        const size_type n = bkt_num_key(k);
        node* first = buckets[n];
        size_type erased = 0;
        if(first){
            node* cur = first;
            node* next = cur->next;
            while(next){
                if(equals(get_key(next->val), k)){
                    cur->next = next->next;
                    delete_node(next);
                    next = cur->next;
                    --num_elements();
                    ++erased;
                }
                else{
                    cur = next;
                    next = cur->next;
                }
            }
            if(equals(get_key(first->val), k)){
                buckets[n] = first->next;
                delete_node(first);
                --num_elements();
                ++erased;
            }
        }
        return erased;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator 
    HASHTABLE::erase(const_iterator position){
        node* p = position.cur;
        iterator ret(p, position.ht);
        ++ret;
        if(p){
            const size_type n = bkt_num(p->val);
            node* cur = buckets[n];
            if(cur == p){
                buckets[n] = cur->next;
                delete_node(cur);
                --num_elements();
            }
            else{
                node* next = cur->next;
                while(next){
                    if(next ==  p){
                        cur->next = next->next;
                        delete_node(next);
                        --num_elements();
                        break;
                    }
                    else{
                        cur = next;
                        next = cur->next;
                    }
                }
            }
        }
        return ret;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator 
    HASHTABLE::erase(const_iterator first, const_iterator last){
        size_type f_bucket = first.cur ? bkt_num(first.cur->val) : buckets.size();
        size_type l_bucket = last.cur ? bkt_num(last.cur->val) : buckets.size();
        if (first.cur == last.cur) return;
        else if(f_bucket == l_bucket)
            erase_bucket(f_bucket, first.cur, last.cur);
        else{
            erase_bucket(f_bucket, first.cur, nullptr);
            for (size_type n = f_bucket + 1; n < l_bucket; ++n){
                erase_bucket(n, nullptr);
            }
            if (l_bucket != buckets.size()) erase_bucket(l_bucket, last.cur);
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    float 
    HASHTABLE::load_factor() const noexcept{
        const size_type n = buckets.size();
        return n != 0 ? (float)num_elements() / n : 0.0f;
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void 
    HASHTABLE::max_load_factor(float z){
        if (z < 0) throw;
        mlf = z;
        rehash(num_elements());
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::clear() noexcept{
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
    template <class... Args>
    typename HASHTABLE::node*
    HASHTABLE::new_node(Args&&... arg){
        node* n = node_allocator().allocate(1);
        n->next = nullptr;
        try{
            node_allocator().construct(&n->val, std::forward<Args>(arg)...);
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
                    clear();
                    //
                }
            }
        }
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

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    std::pair<typename HASHTABLE::iterator, bool>
    HASHTABLE::insert_node_unique(node* ptr){
        const size_type n = bkt_num_key(get_key(ptr->val));
        auto cur = buckets[n];
        if(cur == nullptr){
            buckets[n] = ptr;    
        }
        else{
            for (; cur; cur = cur->next){
                if(equals(get_key(cur->val), get_key(ptr->val))){
                    return make_pair(iterator(ptr, this), false);
                }
            }
            ptr->next = buckets[n];
            buckets[n] = ptr;
        }
        ++num_elements();
        return make_pair(iterator(ptr, this), true);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    typename HASHTABLE::iterator
    HASHTABLE::insert_node_equal(node* ptr){
        const size_type n = bkt_num_key(get_key(ptr->val));
        auto cur = buckets[n];
        if(cur == nullptr){
            buckets[n] = ptr;
        }
        else{
            for (; cur; cur = cur->next){
                if(equals(get_key(cur->val), get_key(ptr->val))){
                    ptr->next = cur->next;
                    cur->next = ptr;
                    ++num_elements();
                    return iterator(ptr, this);
                }
            }
            ptr->next = buckets[n];
            buckets[n] = ptr;
        } 
        ++num_elements();
        return iterator(ptr, this);
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::erase_bucket(const size_type n, node* first, node* last){
        node* cur = buckets[n];
        if(cur == first){
            erase_bucket(n, last);
        }
        else{
            node* next;
            for (next = cur->next; next != first; cur = next, next = next->next);
            while(next != last){
                cur->next = next->next;
                delete_node(next);
                next = cur->next;
                --num_elements();
            }
        }
    }

    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void
    HASHTABLE::erase_bucket(const size_type n, node* last){
        node* cur = buckets[n];
        while(cur != last){
            node* next = cur->next;
            delete_node(cur);
            cur = next;
            buckets[n] = cur;
            --num_elements();
        }
    }

    /*-------------------------------Non-member function overloads------------------------------------*/
    template <class Value, class Key, class HashFcn,
                class ExtractKey, class EqualKey, class Alloc>
    void swap(HASHTABLE& lhs, HASHTABLE& rhs) noexcept{
        lhs.swap(rhs);
    }

} // namespace 

#endif