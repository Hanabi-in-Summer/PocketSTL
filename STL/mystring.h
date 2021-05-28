#ifndef _POCKET_MYSTRING_H_
#define _POCKET_MYSTRING_H_

#include "basic_string.h"

namespace pocket_stl{
    using string = pocket_stl::basic_string<char>;
    using wstring = pocket_stl::basic_string<wchar_t>;
    using u16string = pocket_stl::basic_string<char16_t>;
    using u32string = pocket_stl::basic_string<char32_t>;
} // namespace

#endif