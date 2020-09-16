#ifndef _POCKET_CONSTRUCT_H_
#define _POCKET_CONSTRUCT_H_

/*
** construct & destroy
** 在已分配好的内存上进行对象的构造和析构
*/

#include <new>
#include "iterator.h"

namespace pocket_stl{
    // construct

    template <class T1, class T2>
    void construct(T1 *p, const T2& value){
        ::new (p) T1(value);
    }

    template <class T, class... Args>
    void construct(T* p, Args&&... args){
        ::new (p) T(std::forward<Args>(args)...);
    }
}


#endif