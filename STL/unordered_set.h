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
        std::pair <iterator,bool> emplace ( Args&&... args );
    };
   

}

#endif