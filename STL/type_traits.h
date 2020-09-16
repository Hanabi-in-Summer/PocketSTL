#ifndef _POCKET_TYPE_TRAITS_H_
#define _POCKET_TYPE_TRAITS_H_

/*
** 萃取类型的信息
*/


namespace pocket_stl{
    struct __true_type {};
    struct __false_type {};

    // 泛化的 __type_traits
    template <class T>
    struct __type_traits{
        typedef     __false_type    has_trivial_default_constructor;
        typedef     __false_type    has_trivial_copy_constructor;
        typedef     __false_type    has_trivial_assignment_operator;
        typedef     __false_type    has_trivial_destructor;
        typedef     __false_type    is_POD_type;                        // Plain Old Data 即标量型或传统的 C Struct 型别
                                                                        // 必然拥有 trivial ctor / dtor / copy / assignment 函数
    };

    // 偏特化版 __type_traits
    template <>
    struct __type_traits<char>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<signed char>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned char>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<short>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned short>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<int>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned int>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned long>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<float>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<double>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long double>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<long long>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<unsigned long long>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <>
    struct __type_traits<bool>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <class T>
    struct __type_traits<T*>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };

    template <class T>
    struct __type_traits<const T*>{
        typedef     __true_type     has_trivial_default_constructor;
        typedef     __true_type     has_trivial_copy_constructor;
        typedef     __true_type     has_trivial_assignment_operator;
        typedef     __true_type     has_trivial_destructor;
        typedef     __true_type     is_POD_type;
    };
}

#endif