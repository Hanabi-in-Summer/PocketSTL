#ifndef _POCKET_UNORDERED_SET_H_
#define _POCKET_UNORDERED_SET_H_

#include "hashtable.h"

namespace pocket_stl{
    template <class Key, class Hash, class Pred, class Alloc>
    class unordered_set;

    template <class Key, class Hash = hash<Key>, 
        class Pred = equal_to<Key>, class Alloc = allocator<Key>>
    class unordered_set{
        
    private:
        using ht = __hashtable<Key, Key, Hash, identity<Key>, Pred, Alloc>;
        ht rep;
    public:
        using key_type = typename ht::key_type;
        using value_type = typename ht::value_type;
        using hasher = typename ht::hasher;
        using key_equal = typename ht::key_equal;
        using iterator = typename ht::iterator;
        using const_iterator = typename ht::const_iterator;
        using size_type = typename ht::size_type;
        using pointer = typename ht::pointer;
        using reference = typename ht::reference;
        using allocator_type = Alloc;
        using difference_type = ptrdiff_t;

    public:
    /******************** ctor \ operator= ****************/
        explicit unordered_set ( size_type n = 100/* see below */,
                         const hasher& hf = hasher(),
                         const key_equal& eql = key_equal())
                    : rep(n, hf, eql){ }

        template <class InputIterator, class = typename std::enable_if<
                                           !std::is_integral<InputIterator>::value>::type>
        unordered_set(InputIterator first, InputIterator last,
                      size_type n = 100 /* see below */,
                      const hasher& hf = hasher(),
                      const key_equal& eql = key_equal())
            : rep(first, last, n, hf, eql) { }
        
        unordered_set ( const unordered_set& ust ) : rep(ust.rep) {}
        unordered_set ( unordered_set&& ust ) : rep(std::move(ust.rep)) {}
        unordered_set& operator=(const unordered_set& ust) { 
            rep = ust.rep;
            return *this;
        }
        unordered_set& operator=(unordered_set&& ust){
            rep = std::move(ust.rep);
            return *this;
        }
    /********************** capacity ********************/

        bool empty() const noexcept { return rep.empty(); }
        size_type size() const noexcept { return rep.size(); }
        size_type max_size() const noexcept { return rep.max_size(); }
     /********************** iterator **********************/
        iterator begin() noexcept { return rep.begin(); }
        const_iterator begin() const noexcept { return rep.begin(); }
        iterator end() noexcept { return rep.end(); }
        const_iterator end() const noexcept { return rep.end(); }
        const_iterator cbegin() const noexcept { return rep.cbegin(); }
        const_iterator cend() const noexcept { return rep.cend(); }

    /************************ Element lookup ******************/
        iterator find(const key_type& k) { return rep.find(k); }
        const_iterator find(const key_type& k) const { return rep.find(k); }
        size_type count(const key_type& k) const { return rep.count(k); }
        std::pair<iterator, iterator>
        equal_range(const key_type& k) { return rep.equal_range_unique(k); }
        std::pair<const_iterator, const_iterator>
        equal_range(const key_type& k) const { return rep.equal_range_unique(k); }
    /*********************** Modifiers ***********************/
        template <class... Args>
        std::pair<iterator, bool> emplace(Args&&... args) { return rep.emplace_unique(std::forward<Args>(args)...); }
        // template <class... Args>
        // iterator emplace_hint ( const_iterator position, Args&&... args );
        std::pair<iterator, bool> insert(const value_type& val) { return rep.insert_unique(val); }
        std::pair<iterator, bool> insert(value_type&& val) { return rep.emplace_unique(std::move(val)); }
        template <class InputIterator, class = typename std::enable_if<
                                           !std::is_integral<InputIterator>::value>::type>
        void insert(InputIterator first, InputIterator last) { return rep.insert_unique_copy(first, last); }
        iterator erase(const_iterator position) { return rep.erase(position); }
        size_type erase(const key_type& k) { return rep.erase(k); }
        iterator erase(const_iterator first, const_iterator last) { return rep.erase(first, last); }
        void clear() noexcept { rep.clear(); }
        void swap(unordered_set& ust) { rep.swap(ust.rep); }\
    /*********************** Buckets ***********************/
        size_type bucket_count() const noexcept { return rep.bucket_count(); }
        size_type max_bucket_count() const noexcept { return rep.max_bucket_count(); }
        size_type bucket_size(size_type n) const { return rep.bucket_size(); }
        size_type bucket(const key_type& k) const { return rep.bucket; }

        float load_factor() const noexcept { return rep.load_factor(); }
        float max_load_factor() const noexcept { return rep.max_load_factor(); }
        void max_load_factor(float z) { return rep.max_load_factor(z); }
        void rehash(size_type n) { rep.rehash(n); }
        void reserve(size_type n) { rep.reserve(n); }

        hasher hash_function() const { return rep.hash_function(); }
        key_equal key_eq() const { return rep.key_eq(); }
        allocator_type get_allocator() const noexcept { return allocator_type(); }
    };
   

}

#endif