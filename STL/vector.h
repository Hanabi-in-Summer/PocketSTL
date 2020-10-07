#ifndef _POCKET_VECTOR_H_
#define _POCKET_VECTOR_H_

/*
** vector
** vector<bool> 需特化， 在 std 中不是容器
*/

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include "allocator.h"
#include "uninitialized.h"

namespace pocket_stl{
    template <class T, class Alloc = allocator<T>>
    class vector{
    public:
        typedef Alloc   allocator_type;
        typedef Alloc   data_allocator;

        using value_type        = typename allocator_type::value_type;
        using size_type         = typename allocator_type::size_type;
        using difference_type   = typename allocator_type::difference_type;
        using pointer           = typename allocator_type::pointer;
        using const_pointer     = typename allocator_type::const_pointer;
        using reference         = typename allocator_type::reference;
        using const_reference   = typename allocator_type::const_reference;

        using iterator          = value_type*;
        using const_iterator    = const value_type*;
        using reverse_iterator          = std::reverse_iterator<iterator>;          // 暂时使用 std::reverse_iterator
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

    private:
        iterator start;
        iterator end;
        iterator end_of_storage;

    public:
        /***************************ctor 、 copy_ctor 、 move_ctor 、 dtor************************/
        // default
        vector() noexcept{
            try{
                start = allocator_type::allocate(16);
                end = start;
                end_of_storage = start + 16;
            }
            catch(...){
                start = nullptr;
                end = nullptr;
                end_of_storage = nullptr;
            }
        }

        // fill
        explicit vector(size_type n){
            allocate_and_fill(n, value_type());
        }

        explicit vector(size_type n, const value_type& val){
            allocate_and_fill(n, val);
        }

        // range
        template <class InputIterator>
        vector(InputIterator first, InputIterator last){
            initialize_aux(first, last, std::is_integral<InputIterator>::type);
        }




    public:
        void allocate_and_fill(size_type n, const value_type& val);
        template <class InputIterator>
        void initialize_aux(InputIterator first, InputIterator last, std::true_type);
        template <class Integer>
        void initialize_aux(Integer n, Integer val, std::false_type);

    };

    template <class T, class Alloc>
    void vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val) {
        start = allocator_type::allocate(n);
        end = start + n;
        uninitialized_fill_n(start, n, val);
        end_of_storage = end;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::initialize_aux(InputIterator first, InputIterator last, std::true_type){
        size_type n = last - first;
        start = allocate(n);
        end = uninitialized_copy(first, last, start);
        end_of_storage = end;
    }
    
    template <class T, class Alloc>
    template <class Integer>
    void vector<T, Alloc>::initialize_aux(Integer n, Integer val, std::false_type){
        start = allocate(static_cast<size_type>(n));
        uninitialized_fill_n(start, n, val);
        end = start + n;
        end_of_storage = end;
    }
    
} // namespace

#endif