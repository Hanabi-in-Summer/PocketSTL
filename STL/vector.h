#ifndef _POCKET_VECTOR_H_
#define _POCKET_VECTOR_H_

/*
** vector
*/

#include <cstddef>
#include "allocator.h"

namespace pocket_stl{
    template <class T, class Alloc = allocator<T>>
    class vector{
    public:
        typedef T           value_type;
        typedef T*          iterator;
        typedef T*          pointer;
        typedef const T*    const_pointer;
        typedef T&          reference;
        typedef const T&    const_reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;

    private:
        iterator start;
        iterator end;
        iterator end_of_storage;

        
    };
}

#endif