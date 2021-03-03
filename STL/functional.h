#ifndef _POCKET_FUNCTIONAL_H_
#define _POCKET_FUNCTIONAL_H_

#include <cstddef>

namespace pocket_stl{
    // 定义一元函数的参数型别和返回值型别
    template <class Arg, class Result>
    struct unary_function{
        using argument_type = Arg;
        using result_type   = Result;
    };

    // 定义二元函数的参数型别的返回值型别
    template <class Arg1, class Arg2, class Result>
    struct binary_function{
        using first_argument_type   = Arg1;
        using second_argument_type  = Arg2;
        using result_type = Result;
    };

    template<class T>
    struct identity : public unary_function<T, T>{
        const T& operator()(const T& x) const{
            return x;
        }
    };

    template <class T>
    struct equal_to :public binary_function<T, T, bool>{
        bool operator()(const T& x, const T& y) const { return x == y; }
    };

    /**************************** hash function *****************************/
    // 对于大部分类型，hash function 什么都不做
    template <class Key>
    struct hash {};

    // 针对指针的偏特化版本
    template <class T>
    struct hash<T*>{
        size_t operator()(T* p) const noexcept{ 
            return reinterpret_cast<size_t>(p); 
        }
    };

    // 对于整型类型，只是返回原值
    #define POCKETSTL_TRIVIAL_HASH_FUNC(Type)       \
    template <> struct hash<Type>{                  \
        size_t operator()(Type val) const noexcept{ \
            return static_cast<size_t>(val);        \
        }                                           \
    };

    POCKETSTL_TRIVIAL_HASH_FUNC(bool)
    POCKETSTL_TRIVIAL_HASH_FUNC(char)
    POCKETSTL_TRIVIAL_HASH_FUNC(signed char)
    POCKETSTL_TRIVIAL_HASH_FUNC(unsigned char)
    POCKETSTL_TRIVIAL_HASH_FUNC(wchar_t)
    POCKETSTL_TRIVIAL_HASH_FUNC(char16_t)
    POCKETSTL_TRIVIAL_HASH_FUNC(char32_t)
    POCKETSTL_TRIVIAL_HASH_FUNC(short)
    POCKETSTL_TRIVIAL_HASH_FUNC(unsigned short)
    POCKETSTL_TRIVIAL_HASH_FUNC(int)
    POCKETSTL_TRIVIAL_HASH_FUNC(unsigned int)
    POCKETSTL_TRIVIAL_HASH_FUNC(long)
    POCKETSTL_TRIVIAL_HASH_FUNC(unsigned long)
    POCKETSTL_TRIVIAL_HASH_FUNC(long long)
    POCKETSTL_TRIVIAL_HASH_FUNC(unsigned long long)

    #undef POCKETSTL_TRIVIAL_HASH_FUNC

    // 对于浮点数，逐位哈希
    inline size_t bitwise_hash(const unsigned char* first, size_t count){
        #if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
        #else
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
        #endif
        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i) {
            result ^= (size_t)first[i];
            result *= fnv_prime;
        }
        return result;
    }

    template <>
    struct hash<float>{
        size_t operator()(const float& val){ 
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
        }
    };

    template <>
    struct hash<double>{
        size_t operator()(const double& val){
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
        }
    };

    template <>
    struct hash<long double>{
        size_t operator()(const long double& val){
            return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
        }
    };

}

#endif