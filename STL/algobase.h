#ifndef _POCKET_ALGOBASE_H_
#define _POCKET_ALGOBASE_H_

#include "type_traits.h"
#include "iterator.h"
#include <type_traits>
#include <cstring>

namespace pocket_stl{
    /**************************** fill_n ****************************/
    template <class OutputIter, class Size, class T>
    OutputIter __fill_n(OutputIter first, Size n, const T& value){
        for (; n > 0; --n, ++first){
            *first = value;
        }
        return first;
    }

    template <class Tp, class Size, class Up>
    typename std::enable_if<
            std::is_integral<Tp>::value && 
            sizeof(Tp) == 1 &&
            !std::is_same<Tp, bool>::value &&
            std::is_integral<Up>::value && 
            sizeof(Up) == 1,
            Tp*>::type
    __fill_n(Tp* first, Size n, Up value){
        if (n > 0){
            std::memset(first, (unsigned char)value, (size_t)(n));
        }
        return first + n;
    }

    template <class OutputIter, class Size, class T>
    OutputIter fill_n(OutputIter first, Size n, const T& value){
        return __fill_n(first, n, value);
    }
    /**************************** fill ****************************/
    template <class ForwardIter, class T>
    void __fill(ForwardIter first, ForwardIter last, const T& value,
                pocket_stl::forward_iterator_tag){
        for (; first != last; ++first){
            *first = value;
        }
    }

    template <class RandomIter, class T>
    void __fill(RandomIter first, RandomIter last, const T& value,
                pocket_stl::random_access_iterator_tag){
        fill_n(first, last - first, value);
    }

    template <class ForwardIter, class T>
    void fill(ForwardIter first, ForwardIter last, const T& value){
        __fill(first, last, value, iterator_category(first));
    }
    /**************************** copy ****************************/
    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, 
                                OutputIterator result, Distance*){
        for (Distance n = last - first; n > 0; --n, ++result, ++first){
            *result = *first;
        }
        return result;
    }

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, pocket_stl::__true_type){
        memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    template <class T>
    inline T* __copy_t(const T* first, const T* last, T* result, pocket_stl::__false_type){
        return __copy_d(first, last, result, (ptrdiff_t*)0);
    }


    template <class InputIterator, class OutputIterator>
    inline OutputIterator __copy(InputIterator first, InputIterator last, 
                                OutputIterator result, pocket_stl::input_iterator_tag){
        for (; first != last; ++result, ++first){
            *result = *first;
        }
        return result;
    }

    template <class RandomAccessIterator, class OutputIterator>
    inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, 
                                OutputIterator result, pocket_stl::random_access_iterator_tag){
        return __copy_d(first, last, result, pocket_stl::distance_type(first));
    }

    // -------------------- 完全泛化版 __copy_dispatch
    template <class InputIterator, class OutputIterator>
    struct __copy_dispatch{
        OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result){
            return __copy(first, last, result, pocket_stl::iterator_category(first));
        }
    };
    // -------------------- 偏特化版 __copy_dispatch
    template <class T>
    struct __copy_dispatch<T*, T*>{
        T* operator()(T* first, T* last, T* result){
            typedef typename pocket_stl::__type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first, last, result, t());
        }
    };
    template <class T>
    struct __copy_dispatch<const T*, T*>{
        T* operator()(const T* first, const T* last, T* result){
            typedef typename pocket_stl::__type_traits<T>::has_trivial_assignment_operator t;
            return __copy_t(first, last, result, t());
        }
    };


    template <class InputIterator, class OutputIterator>
    inline OutputIterator copy (InputIterator first, InputIterator last, OutputIterator result){
        return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
    }

    inline char* copy(const char* first, const char* last, char* result){
        memmove(result, first, last - first);
        return result + (last - first);
    }

    inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }

    /**************************** copy_backward ****************************/
    template <class RandomAccessIterator, class OutputIterator, class Distance>
    inline OutputIterator __copy_backward_d(RandomAccessIterator first, RandomAccessIterator last, 
                                OutputIterator result, Distance*){
        for (Distance n = last - first; n > 0; --n){
            *--result = *--last;
        }
        return result;
    }

    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, pocket_stl::__true_type){
        const ptrdiff_t n = last - first;
        memmove(result - n, first, sizeof(T) * (last - first));
        return result - n;
    }
    template <class T>
    inline T* __copy_backward_t(const T* first, const T* last, T* result, pocket_stl::__false_type){
        return __copy_backward_d(first, last, result, (ptrdiff_t*)0);
    }

    template <class BidirectionalIterator, class OutputIterator>
    inline OutputIterator __copy_backward(BidirectionalIterator first, BidirectionalIterator last, 
                                OutputIterator result, pocket_stl::bidrectional_iterator_tag){
        while (first != last){
            *--result = *--last;
        }
        return result;
    }

    template <class RandomAccessIterator, class OutputIterator>
    inline OutputIterator __copy_bcakward(RandomAccessIterator first, RandomAccessIterator last, 
                                OutputIterator result, pocket_stl::random_access_iterator_tag){
        return __copy_d(first, last, result, pocket_stl::distance_type(first));
    }


    template <class BidirectionalIterator, class OutputIterator>
    struct __copy_backward_dispatch{
        OutputIterator operator()(BidirectionalIterator first, BidirectionalIterator last, OutputIterator result){
            return __copy_backward(first, last, result, pocket_stl::iterator_category(first));
        }
    };

    template <class T>
    struct __copy_backward_dispatch<T*, T*>{
        T* operator()(T* first, T* last, T* result){
            typedef typename pocket_stl::__type_traits<T>::has_trivial_assignment_operator t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    template <class T>
    struct __copy_backward_dispatch<const T*, T*>{
        T* operator()(const T* first, const T* last, T* result){
            typedef typename pocket_stl::__type_traits<T>::has_trivial_assignment_operator t;
            return __copy_backward_t(first, last, result, t());
        }
    };
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    BidirectionalIterator2 copy_backward (BidirectionalIterator1 first,
                                        BidirectionalIterator1 last,
                                        BidirectionalIterator2 result){
        return __copy_backward_dispatch<BidirectionalIterator1, BidirectionalIterator2>()(first, last, result);
    }
}

#endif