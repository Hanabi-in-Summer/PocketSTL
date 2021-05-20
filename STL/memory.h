#ifndef _POCKET_MEMORY_H
#define _POCKET_MEMORY_H

/*
** 负责一些动态内存管理
*/

#include <climits>
#include <cstddef>

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"


namespace pocket_stl{

    // 获取对象地址
    template <class T>
    constexpr T* address_of(T& val) noexcept{
        return &val;
    }

    // 分配 len 个 T size 的内存，失败则每次减少一半，返回分配地址与分配大小
    template <class T>
    std::pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*){
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
        len = INT_MAX / sizeof(T);
        while (len > 0)
        {
            T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
            if (tmp)
                return std::pair<T*, ptrdiff_t>(tmp, len);
            len /= 2;  // 申请失败时减少 len 的大小
        }
        return std::pair<T*, ptrdiff_t>(nullptr, 0);
    }

    template <class T>
    std::pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len){
        return get_buffer_helper(len, static_cast<T*>(0));
    }

    template <class T>
    std::pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*){
        return get_buffer_helper(len, static_cast<T*>(0));
    }

    template <class T>
    void release_temporary_buffer(T* ptr){
        free(ptr);
    }

    // ---------------------------------------------------------------------------------------
    // temporary_buffer
    // 临时缓冲区的申请以及释放

    template <class ForwardIterator, class T>
    class temporary_buffer{
    private:
        ptrdiff_t original_len;  // 缓冲区申请的大小
        ptrdiff_t len;           // 缓冲区实际的大小
        T*        buffer;        // 指向缓冲区的指针

    public:
    // 构造、析构函数
        temporary_buffer(ForwardIterator first, ForwardIterator last);

        ~temporary_buffer(){
            pocket_stl::destroy(buffer, buffer + len);
            free(buffer);
        }

    public:

        ptrdiff_t size()           const noexcept { return len; }
        ptrdiff_t requested_size() const noexcept { return original_len; }
        T*        begin()                noexcept { return buffer; }
        T*        end()                  noexcept { return buffer + len; }

    private:
        void allocate_buffer();
        void initialize_buffer(const T&, std::true_type) {}
        void initialize_buffer(const T& value, std::false_type)
        { pocket_stl::uninitialized_fill_n(buffer, len, value); }

    private:
        temporary_buffer(const temporary_buffer&);
        // void operator=(const temporary_buffer&);
    };

    // 构造函数
    template <class ForwardIterator, class T>
    temporary_buffer<ForwardIterator, T>::
    temporary_buffer(ForwardIterator first, ForwardIterator last){
        try{
            len = std::distance(first, last);
            allocate_buffer();
            if (len > 0){
                initialize_buffer(*first, std::is_trivially_default_constructible<T>());
            }
        }
        catch (...){
            free(buffer);
            buffer = nullptr;
            len = 0;
        }
    }

    template <class ForwardIterator, class T>
    void temporary_buffer<ForwardIterator, T>::allocate_buffer(){
        original_len = len;
        if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);
        while (len > 0){
            buffer = static_cast<T*>(malloc(len * sizeof(T)));
            if (buffer)
            break;
            len /= 2;  // 申请失败时减少申请空间大小
        }
    }

}


#endif