#ifndef _POCKET_ALLOCATOR_H_
#define _POCKET_ALLOCATOR_H_

// #include <new>
#include <cstddef>
#include <climits>
#include "construct.h"

namespace pocket_stl{
    template <class T>
    class allocator{
    public:
        typedef size_t          size_type;
        typedef T               value_type;
        typedef ptrdiff_t       difference_type;
        typedef T*              pointer;
        typedef const T*        const_pointer;
        typedef T&              reference;
        typedef const T&        const_reference;

        // default ctor
        // copy ctor
        // dtor

    public:
        // rebind 允许一个类型对象的 allocator 分配其他类型的存储
        template <class U>
        struct rebind{
            typedef allocator<U> other;
        };

        pointer         address(reference x) const noexcept;            // 返回某个对象的地址
        const_pointer   address(const_reference x) const noexcept;
        size_type       max_size() const noexcept;                      // 返回成功配置的最大量

        static pointer  allocate(size_type n);                          // 配置空间足以存储 n 个对象
        static void     deallocate(pointer p, size_type n);             // 归还先前配置的空间
        static void     deallocate(pointer p);
        
        void            construct(pointer p, const_reference x);        // 对象构造操作

        template <class... Args>
        void            construct(T* p, Args&&... args);                // c++11 construct

        void            destroy(pointer p);                             // 对象析构操作

    };

    template <class T>
    typename allocator<T>::pointer 
    allocator<T>::address(typename allocator<T>::reference x)const noexcept{
        return (pointer)&x;
    }

    template <class T>
    typename allocator<T>::const_pointer 
    allocator<T>::address(typename allocator<T>::const_reference x)const noexcept{
        return (const_pointer)&x;
    }

    template <class T>
    typename allocator<T>::size_type 
    allocator<T>::max_size() const noexcept{
        return size_type(UINT_MAX / sizeof(T));
    }

    template <class T>
    typename allocator<T>::pointer
    allocator<T>::allocate(typename allocator<T>::size_type n){
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    template <class T>
    void allocator<T>::deallocate(typename allocator<T>::pointer p, allocator<T>::size_type n){
        deallocate(p);
    }

    template <class T>
    void allocator<T>::deallocate(typename allocator<T>::pointer p){
        if (nullptr == p) return;
        ::operator delete(p);
    }

    template <class T>
    void allocator<T>::construct(pointer p, const_reference x){
        construct<T, T>(p, x);
        return;
    }

    template <class T>
    template <class... Args>
    void allocator<T>::construct(T* p, Args&&... args){
        pocket_stl::construct(p, std::forward<Args>(args)...);
        return;
    }

    template <class T>
    void allocator<T>::destroy(pointer p){
        pocket_stl::destroy(p, pocket_stl::__type_traits<T>::has_trivial_destructor());
        return;
    }
}

#endif