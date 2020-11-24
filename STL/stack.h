#ifndef _POCKET_STACK_H_
#define _POCKET_STACK_H_

/*
** stack
** 
*/

#include "deque.h"

namespace pocket_stl{
    template <class T, class Container = deque<T>> class stack;
    template <class T, class Container>
    bool operator== (const stack<T,Container>& lhs, const stack<T,Container>& rhs);
    template <class T, class Container>
    bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs);

    template <class T, class Container> class stack{
    public:
        using value_type        = T;
        using container_type    = Container;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using size_type         = typename Container::size_type;

    private:
        container_type __c;
 
    public:
        friend bool operator== <>(const stack& lhs, const stack& rhs);
        friend bool operator< <>(const stack& lhs, const stack& rhs);

    public:
     /***************ctor 、 copy_ctor 、 move_ctor 、 dtor 、 operator=*****************/
     explicit stack(const container_type& ctnr) : __c(ctnr) {}
     explicit stack(container_type&& ctnr = container_type()) : __c(std::move(ctnr)) {}
     stack(const stack& x) : __c(x.__c) {}
     stack(stack&& x) : __c(std::move(x.__c)) {}
     ~stack() = default;

    public:
        /***************************** Member functions *****************************/
        bool empty() const { return __c.empty(); }
        size_type size() const { return __c.size(); }
        value_type& top() { return __c.back(); }
        const value_type& top() const { return __c.back(); }
        void push(const value_type& val) { __c.push_back(val); }
        void push(value_type&& val) { __c.push_back(std::move(val)); }
        template <class... Args>
        void emplace(Args&&... args) { __c.emplace_back(std::forward<Args>(args)...); }
        void pop() { __c.pop_back(); }
        void swap(stack& x) noexcept(noexcept(pocket_stl::swap(__c, x.__c))) { pocket_stl::swap(__c, x.__c); }
    };
    /************************* Non-member function overloads **********************/

    template <class T, class Container>
    bool operator== (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return lhs.__c == rhs.__c;
    }
    template <class T, class Container>
    bool operator!= (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return !(lhs == rhs);
    }
    template <class T, class Container>
    bool operator<  (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return lhs.__c < rhs.__c;
    }
    template <class T, class Container>
    bool operator<= (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return !(rhs < lhs);
    }
    template <class T, class Container>
    bool operator>  (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return rhs < lhs;
    }
    template <class T, class Container>
    bool operator>= (const stack<T,Container>& lhs, const stack<T,Container>& rhs){
        return !(lhs < rhs);
    }
    template <class T, class Container>
    void swap (stack<T,Container>& x, stack<T,Container>& y) noexcept(noexcept(x.swap(y))){
        x.swap(y);
    }
    
} // namespace

#endif