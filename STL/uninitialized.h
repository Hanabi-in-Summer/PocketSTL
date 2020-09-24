#ifndef _POCKET_UNINITIALIZED_H_
#define _POCKET_UNINITIALIZED_H_

/*
** 在未初始化的空间上构造或析构
*/

#include <algorithm>
#include "construct.h"
#include "iterator.h"

namespace pocket_stl{

    //////////////////////////////////////////////////////////////////////////////////////
    // uninitialized_copy                                                               //
    // 调用 copy constructor 给 [first, last) 内的每一个对象产生一个复制品，放入输出范围内 //
    // 返回复制结束的位置                                                                //
    // commit or rollback                                                               //
    //////////////////////////////////////////////////////////////////////////////////////
    template <class InputIterator, class ForwardIterator>
    ForwardIterator
    uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
        return __uninitialized_copy(first, last, result, value_type(result));
    }

    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator
    __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*){  // T* 提取迭代器解引用的型别
        typedef typename __type_traits<T>::is_pod_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type){
        return std::copy(first, last, result);
    }

    template <class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type){
        ForwardIterator cur = result;
        try{
            for (; first != last; ++first, ++cur){
                construct(&*cur, *first);
            }
        }
        catch(...){                                 // commit or rollback
            for (; result != cur; ++cur){
                destroy(result);
            }
        }
        return cur;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // uninitialized_copy_n                                                                    //
    // 调用 copy constructor 给 [first, first + n) 内的每一个对象产生一个复制品，放入输出范围内   //
    // 返回复制结束的位置                                                                       //
    // commit or rollback                                                                      //
    /////////////////////////////////////////////////////////////////////////////////////////////
}


#endif