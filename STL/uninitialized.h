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
            for (; result != cur; ++result){
                destroy(&*result);
            }
        }
        return cur;
    }

    template <class InputIterator, class ForwardIterator, class T>
    inline ForwardIterator
    __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*){  // T* 提取迭代器解引用的型别
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }

    template <class InputIterator, class ForwardIterator>
    ForwardIterator
    uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
        return __uninitialized_copy(first, last, result, value_type(result));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // uninitialized_copy_n                                                                    //
    // 调用 copy constructor 给 [first, first + n) 内的每一个对象产生一个复制品，放入输出范围内   //
    // 返回复制结束的位置                                                                       //
    // commit or rollback                                                                      //
    /////////////////////////////////////////////////////////////////////////////////////////////
    template <class InputIterator, class Size, class ForwardIterator>
    ForwardIterator
    __uninitialized_copy_n_aux(InputIterator first, Size n, ForwardIterator result, __true_type){
        return std::copy_n(first, n, result);
    }

    template <class InputIterator, class Size, class ForwardIterator>
    ForwardIterator
    _uninitialized_copy_n_aux(InputIterator first, Size n, ForwardIterator result, __false_type){
        ForwardIterator cur = result;
        try{
            for (; n > 0; --n, ++cur, ++first){
                construct(&*cur, *first);
            }
        }
        catch(...){
            for (; cur != result; ++result){
                destroy(&*result);
            }
        }
        return cur;
    }

    template <class InputIterator, class Size, class ForwardIterator, class T>
    ForwardIterator
    __uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result, T*){
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_n_aux(first, n, result, is_POD());
    }

    template <class InputIterator, class Size, class ForwardIterator>
    ForwardIterator
    uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result){
        return __uninitialized_copy_n(first, n, result, value_type(result));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // uninitialized_fill                                                                      //
    // 调用 copy constructor 在 [first, last) 范围内填充元素                                    //
    // commit or rollback                                                                      //
    /////////////////////////////////////////////////////////////////////////////////////////////
    template <class ForwardIterator, class T>
    void
    __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type){
        std::fill(first, last, x);
    }

    template <class ForwardIterator, class T>
    void
    __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type){
        ForwardIterator cur = first;
        try{
            for (; cur != last; ++cur){
                construct(&*cur, x);
            }
        }
        catch(...){
            for (; first != cur; ++first){
                destroy(&*first);
            }
        }
    }

    template <class ForwardIterator, class T, class T1>
    void
    __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*){
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }

    template <class ForwardIterator, class T>
    void
    uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x){
        __uninitialized_fill(first, last, x, value_type(first));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // uninitialized_fill_n                                                                    //
    // 调用 copy constructor 在 [first, first + n) 范围内填充元素                               //
    // commit or rollback                                                                      //
    /////////////////////////////////////////////////////////////////////////////////////////////
    template <class ForwardIterator, class Size, class T>
    ForwardIterator
    __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type){
        return std::fill_n(first, n, x);
    }

    template <class ForwarIterator, class Size, class T>
    ForwarIterator
    __uninitialized_fill_n_aux(ForwarIterator first, Size n, const T& x, __false_type){
        ForwarIterator cur = first;
        try{
            for (; n != 0; --n, ++cur){
                construct(&*cur, x);
            }
        }
        catch(...){
            for (; first != cur; ++first){
                destroy(&*first);
            }
        }
        return cur;
    }

    template <class ForwardIterator, class Size, class T, class T1>
    ForwardIterator
    __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*){
        typedef typename __type_traits<T1>::is_POD_type is_POD;
        return __uninitialized_fill_n_aux(first, n, x, is_POD());
    }

    template <class ForwardIterator, class Size, class T>
    ForwardIterator 
    uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        return __uninitialized_fill_n(first, n, x, value_type(first));
    }

}


#endif