#ifndef _pocket_QUEUE_H_
#define _pocket_QUEUE_H_

/*
** queue
** 
*/

#include "deque.h"

namespace pocket_stl{
    template <class T, class Container = deque<T>> class queue;
    template <class T, class Container>
    bool operator==(const queue<T, Container>&, const queue<T, Container>&);
    template <class T, class Container>
    bool operator<(const queue<T, Container>&, const queue<T, Container>&);
    template <class T, class Container> class queue{
    public:
        friend bool operator== <>(const queue&, const queue&);
        friend bool operator< <>(const queue&, const queue&);

    public:
        using value_type        = T;
        using container_type    = Container;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using size_type         = typename Container::size_type;

    private:
        container_type __c;

    public:
        explicit queue (const container_type& ctnr) : __c(ctnr) {}
        explicit queue (container_type&& ctnr = container_type()) : __c(std::move(ctnr)) {}
        template <class Alloc>
        queue(const queue& x) : __c(x.__c) {}
        template <class Alloc>
        queue(queue&& x) : __c(std::move(x.__c)) {}
        ~queue() = default;

        bool empty() const { return __c.empty(); }
        size_type size() const { return __c.size(); }
        value_type& front() { return __c.front(); }
        const value_type& front() const { return __c.front(); }
        reference back() { return __c.back(); }
        const_reference back() const { return __c.back(); }
        void push(const value_type& val) { __c.push_back(val); }
        void push(value_type&& val) { __c.push_back(std::move(val)); }
        template <class... Args>
        void emplace(Args&&... args) { __c.emplace_back(std::forward<Args>(args)...); }
        void pop() { __c.pop_front(); }
        void swap(queue& x) noexcept(noexcept(__c.swap(x.__c))) { __c.swap(x.__c); }

    };

    template <class T, class Container>
    bool operator== (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return lhs.__c == rhs.__c;
    }
    template <class T, class Container>
    bool operator!= (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return lhs == rhs;
    }
    template <class T, class Container>
    bool operator<  (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return lhs.__c < rhs.__c;
    }
    template <class T, class Container>
    bool operator<= (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return !(rhs < lhs);
    }
    template <class T, class Container>
    bool operator>  (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return rhs < lhs;
    }
    template <class T, class Container>
    bool operator>= (const queue<T,Container>& lhs, const queue<T,Container>& rhs){
        return !(lhs < rhs);
    }
    template <class T, class Container>
    void swap (queue<T,Container>& x, queue<T,Container>& y) noexcept(noexcept(x.swap(y))){
        x.swap(y);
    }
}

#endif