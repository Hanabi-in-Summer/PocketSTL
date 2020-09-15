#ifndef _POCKET_ALLOCATOR_H_
#define _POCKET_ALLOCATOR_H_

#include <new>
#include <cstddef>

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


        // rebind 允许一个类型对象的 allocator 分配其他类型的存储
        template <class U>
        struct rebind{
            typedef allocator<U> other;
        };

        pointer         address(reference x) const;                     // 返回某个对象的地址
        const_pointer   address(const_reference x) const;
        pointer         allocate(size_type n);                          // 配置空间足以存储 n 个对象
        void            deallocate(pointer p, size_type n);             // 归还先前配置的空间
        size_type       max_size() const;                               // 返回成功配置的最大量
        void            construct(pointer p, const_reference x);        // 对象构造操作
        template <class... Args>
        void            construct(T* p, Args&&... args);                // c++11 construct
        void            destroy(pointer p);                             // 对象析构操作

    };
}

#endif