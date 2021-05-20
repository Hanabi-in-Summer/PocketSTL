#ifndef _POCKET_BASIC_STRING_H_
#define _POCKET_BASIC_STRING_H_

/*
**  字符串类型
**
*/

#include <assert.h>
#include <cwchar>

#include "iterator.h"
#include "memory.h"


namespace pocket_stl{
    // char_traits
    template <class charT>
    struct char_traits{
        using char_type = charT;

        static size_t length(const char_type* str){
            size_t len = 0;
            for (; *str != char_type(0); ++str){
                ++len;
            }
            return len;
        }

        static int compare(const char_type* s1, const char_type* s2, size_t n){
            for (; n != 0; --n, ++s1, ++s2){
                if (*s1 < *s2) return -1;
                if (*s2 < *s1) return 1;
            }
            return 0;
        }

        static char_type* copy(char_type* dst, const char_type* src, size_t n) {
            assert(src + n <= dst || dst + n <= src);
            char_type* r = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        // 考虑发生重叠的情况
        static char_type* move(char_type* dst, const char_type* src, size_t n){
            char_type* r = dst;
            if (dst < src){
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst){
                dst += n;
                src += n;
                for (; n != 0; --n)
                    *--dst = *--src;
            }
            return r;
        }

        static char_type* fill(char_type* dst, char_type ch, size_t count){
            char_type* r = dst;
            for (; count > 0; --count, ++dst) *dst = ch;
            return r;
        }
    };

    // 偏特化
    template <>
    struct char_traits<char>{
        using char_type = char;
        static size_t length(const char_type* str) noexcept
        { return std::strlen(str); }

        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
        { return std::memcmp(s1, s2, n); }

        static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            return static_cast<char_type*>(std::memcpy(dst, src, n));
        }

        static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept
        {
            return static_cast<char_type*>(std::memmove(dst, src, n));
        }

        static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept
        { 
            return static_cast<char_type*>(std::memset(dst, ch, count));
        }
    };

    template <>
    struct char_traits<wchar_t>
    {
        using char_type = wchar_t;

        static size_t length(const char_type* str) noexcept
        {
            return std::wcslen(str);
        }

        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
        {
            return std::wmemcmp(s1, s2, n);
        }

        static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            return static_cast<char_type*>(std::wmemcpy(dst, src, n));
        }

        static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept
        {
            return static_cast<char_type*>(std::wmemmove(dst, src, n));
        }

        static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept
        { 
            return static_cast<char_type*>(std::wmemset(dst, ch, count));
        }
    };

    template <>
    struct char_traits<char16_t>{
        using char_type = char16_t;

        static size_t length(const char_type* str) noexcept{
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept{
            for (; n != 0; --n, ++s1, ++s2){
                if (*s1 < *s2)
                    return -1;
                if (*s2 < *s1)
                    return 1;
            }
            return 0;
        }

        static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept{
            assert(src + n <= dst || dst + n <= src);
            char_type* r = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept{
            char_type* r = dst;
            if (dst < src){
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst){
                dst += n;
                src += n;
                for (; n != 0; --n)
                    *--dst = *--src;
            }
            return r;
        }

        static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept{
            char_type* r = dst;
            for (; count > 0; --count, ++dst)
                *dst = ch;
            return r;
        }
    };

    template <>
    struct char_traits<char32_t>
    {
        using char_type = char32_t;

        static size_t length(const char_type* str) noexcept{
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept{
            for (; n != 0; --n, ++s1, ++s2){
                if (*s1 < *s2)
                    return -1;
                if (*s2 < *s1)
                    return 1;
            }
            return 0;
        }

        static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept{
            assert(src + n <= dst || dst + n <= src);
            char_type* r = dst;
            for (; n != 0; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept
        {
            char_type* r = dst;
            if (dst < src){
                for (; n != 0; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst){
                dst += n;
                src += n;
                for (; n != 0; --n)
                    *--dst = *--src;
            }
            return r;
        }

        static char_type* fill(char_type* dst, char_type ch, size_t count) noexcept{
            char_type* r = dst;
            for (; count > 0; --count, ++dst)
                *dst = ch;
            return r;
        }
    };


    // 初始化 basic_string 尝试分配的最小 buffer 大小，可能被忽略
    #define STRING_INIT_SIZE 32

    // basic_string
    template < class charT,
           class traits = char_traits<charT>,    // basic_string::traits_type
           class Alloc = pocket_stl::allocator<charT>        // basic_string::allocator_type
           > class basic_string{
    public:
        using traits_type       = traits;
        using char_traits       = traits;
        using allocator_type    = Alloc;

        using value_type        = typename allocator_type::value_type;
        using pointer           = typename allocator_type::pointer;
        using const_pointer     = typename allocator_type::const_pointer;
        using reference         = typename allocator_type::reference;
        using const_reference   = typename allocator_type::const_reference;
        using size_type         = typename allocator_type::size_type;
        using difference_type   = typename allocator_type::difference_type;

        using iterator          = value_type*;
        using const_iterator    = const value_type*;
        using reverse_iterator          = std::reverse_iterator<iterator>;
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

        allocator_type get_allocator() { return allocator_type(); }
        
        static_assert(std::is_pod<charT>::value, "Character type of basic_string must be a POD");
        static_assert(std::is_same<charT, typename traits_type::char_type>::value,
                "CharType must be same as traits_type::char_type");

    public:
        // 末尾位置的值
        // if (str.find('a') != string::npos) { /* do something */ }
        static constexpr size_type npos = static_cast<size_type>(-1);
    private:
        iterator buffer_;   // 储存字符串起始位置
        size_type size_;    // 大小
        compressed_pair<size_type, allocator_type> cap_and_alloc_;  // 容量
        // 返回容量
        size_type&          cap_() noexcept { return cap_and_alloc_.data; }
        const size_type&    cap_() const noexcept { return cap_and_alloc_.data; }
        compressed_pair<size_type, allocator_type>&         data_allocator() noexcept { return cap_and_alloc_; }
        const compressed_pair<size_type, allocator_type>&   data_allocator() const noexcept { return cap_and_alloc_; }

    public:
        basic_string() noexcept
        { try_init(); }

        basic_string(size_type n, value_type ch)
                :buffer_(nullptr), size_(0){
            cap_() = 0;
            fill_init(n, ch);
        }

        basic_string(const basic_string& other, size_type pos)
                :buffer_(nullptr), size_(0)
        {
            cap_() = 0;
            init_from(other.buffer_, pos, other.size_ - pos);
        }

        basic_string(const basic_string& other, size_type pos, size_type count)
                :buffer_(nullptr), size_(0){
            cap_() = 0;
            init_from(other.buffer_, pos, count);
        }

        basic_string(const_pointer str)
                :buffer_(nullptr), size_(0){
            cap_() = 0;
            init_from(str, 0, char_traits::length(str));
        }

        basic_string(const_pointer str, size_type count)
                :buffer_(nullptr), size_(0){
            cap_() = 0;
            init_from(str, 0, count);
        }

        template <class Iter, class = typename std::enable_if<!std::is_integral<Iter>::value>::type>
        basic_string(Iter first, Iter last)
        { copy_init(first, last, iterator_category(first)); }

        basic_string(const basic_string& rhs) 
                :buffer_(nullptr), size_(0){
            cap_() = 0;
            init_from(rhs.buffer_, 0, rhs.size_);
        }

        basic_string(basic_string&& rhs) noexcept
                :buffer_(rhs.buffer_), size_(rhs.size_){
            cap_() = rhs.cap_();
            rhs.buffer_ = nullptr;
            rhs.size_ = 0;
            rhs.cap_ = 0;
        }

        basic_string& operator=(const basic_string& rhs);
        basic_string& operator=(basic_string&& rhs) noexcept;

        basic_string& operator=(const_pointer str);
        basic_string& operator=(value_type ch);

        ~basic_string() { destroy_buffer(); }




    private:
          // init / destroy 
        void          try_init() noexcept;

        void          fill_init(size_type n, value_type ch);

        template <class Iter>
        void          copy_init(Iter first, Iter last, pocket_stl::input_iterator_tag);
        template <class Iter>
        void          copy_init(Iter first, Iter last, pocket_stl::forward_iterator_tag);

        void          init_from(const_pointer src, size_type pos, size_type n);

        void          destroy_buffer();
    };

    /**************************************************************************************************/

    // 复制赋值操作符
    template <class charT, class traits, class Alloc>
    basic_string<charT, traits, Alloc>&
    basic_string<charT, traits, Alloc>::
    operator=(const basic_string& rhs){
        if (this != &rhs){
            basic_string tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // 移动赋值操作符
    template <class charT, class traits, class Alloc>
    basic_string<charT, traits, Alloc>&
    basic_string<charT, traits, Alloc>::
    operator=(basic_string&& rhs) noexcept{
        if (this == &rhs) return *this;
        destroy_buffer();
        buffer_ = rhs.buffer_;
        size_ = rhs.size_;
        cap_() = rhs.cap_;
        rhs.buffer_ = nullptr;
        rhs.size_ = 0;
        rhs.cap_ = 0;
        return *this;
    }

    // 用一个字符串赋值
    template <class charT, class traits, class Alloc>
    basic_string<charT, traits, Alloc>&
    basic_string<charT, traits, Alloc>::
    operator=(const_pointer str)
    {
        const size_type len = char_traits::length(str);
        if (cap_() < len){
            auto new_buffer = data_allocator().allocate(len + 1);
            data_allocator().deallocate(buffer_);
            buffer_ = new_buffer;
            cap_() = len + 1;
        }
        char_traits::copy(buffer_, str, len);
        size_ = len;
        return *this;
    }

    // 用一个字符赋值
    template <class charT, class traits, class Alloc>
    basic_string<charT, traits, Alloc>&
    basic_string<charT, traits, Alloc>::
    operator=(value_type ch)
    {
        if (cap_() < 1){
            auto new_buffer = data_allocator().allocate(2);
            data_allocator().deallocate(buffer_);
            buffer_ = new_buffer;
            cap_() = 2;
        }
        *buffer_ = ch;
        size_ = 1;
        return *this;
    }

    // 尝试初始化一段 buffer，若分配失败则忽略，不会抛出异常
    template <class charT, class traits, class Alloc>
    void basic_string<charT, traits, Alloc>::
    try_init() noexcept{
        try{
            buffer_ = data_allocator().allocate(static_cast<size_type>(STRING_INIT_SIZE));
            size_ = 0;
            cap_() = 0;
        }
        catch (...){
            buffer_ = nullptr;
            size_ = 0;
            cap_() = 0;
            // no throw
        }
    }

    // fill init
    template <class charT, class traits, class Alloc>
    void basic_string<charT, traits, Alloc>::
    fill_init(size_type n, value_type ch){
        const auto init_size = std::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
        buffer_ = data_allocator().allocate(init_size);
        char_traits::fill(buffer_, ch, n);
        size_ = n;
        cap_() = init_size;
    }

    // copy_init 函数
    template <class charT, class traits, class Alloc>
    template <class Iter>
    void basic_string<charT, traits, Alloc>::
    copy_init(Iter first, Iter last, pocket_stl::input_iterator_tag)
    {
        size_type n = std::distance(first, last);
        const auto init_size = std::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
        try{
            buffer_ = data_allocator().allocate(init_size);
            size_ = n;
            cap_() = init_size;
        }
        catch (...){
            buffer_ = nullptr;
            size_ = 0;
            cap_() = 0;
            throw;
        }
        for (; n > 0; --n, ++first)
            append(*first);
    }

    template <class charT, class traits, class Alloc>
    template <class Iter>
    void basic_string<charT, traits, Alloc>::
    copy_init(Iter first, Iter last, pocket_stl::forward_iterator_tag){
        const size_type n = std::distance(first, last);
        const auto init_size = std::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
        try{
            buffer_ = data_allocator().allocate(init_size);
            size_ = n;
            cap_() = init_size;
            pocket_stl::uninitialized_copy(first, last, buffer_);
        }
        catch (...){
            buffer_ = nullptr;
            size_ = 0;
            cap_() = 0;
            throw;
        }
    }

    // init_from 函数
    // 根据指针与 pos 与 count
    template <class charT, class traits, class Alloc>
    void basic_string<charT, traits, Alloc>::
    init_from(const_pointer src, size_type pos, size_type count){
        const auto init_size = std::max(static_cast<size_type>(STRING_INIT_SIZE), count + 1);
        buffer_ = data_allocator().allocate(init_size);
        char_traits::copy(buffer_, src + pos, count);
        size_ = count;
        cap_() = init_size;
    }

    // destroy_buffer 函数
    template <class charT, class traits, class Alloc>
    void basic_string<charT, traits, Alloc>::
    destroy_buffer(){
        if (buffer_ != nullptr){
            data_allocator().deallocate(buffer_, cap_());
            buffer_ = nullptr;
            size_ = 0;
            cap_() = 0;
        }
    }


} // namespace

#endif