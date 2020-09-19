#ifndef _POCKET_CONSTRUCT_H_
#define _POCKET_CONSTRUCT_H_

/*
** construct & destroy
** 在已分配好的内存上进行对象的构造和析构
*/

#include <new>
#include "iterator.h"
#include "type_traits.h"

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

    // destroy
    template <class T>
    void destroy(T* p, __true_type) { }

    template <class T>
    void destroy(T* p, __false_type){
        if (!p) {
            p->~T();
        }
    }

    template <class ForwardIterator>
    void destroy(ForwardIterator first, ForwardIterator last){
        destroy_aux(first, last, typename ForwardIterator::value_type());
    }

    template <class ForwardIterator, class T>
    void destroy_aux(ForwardIterator first, ForwardIterator last, T*){
        __destroy(first, last, pocket_stl::__type_traits<T>::has_trivial_destructor());
    }

    template <class ForwardIterator>
    void __destroy(ForwardIterator first, ForwardIterator last, __true_type) { }

    template <class ForwardIterator>
    void __destroy(ForwardIterator first, ForwardIterator last, __false_type){
        while(first < last){
            destroy(&*first);
            ++first;
        }
    }


}


#endif