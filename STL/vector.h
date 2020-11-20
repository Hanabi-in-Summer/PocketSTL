#ifndef _POCKET_VECTOR_H_
#define _POCKET_VECTOR_H_

/*
** vector
** vector<bool> 需特化， 在 std 中不是容器
*/
#include <stdexcept>
#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include "allocator.h"
#include "uninitialized.h"
#include "algobase.h"

namespace pocket_stl{
    template <class T, class Alloc = allocator<T>>
    class vector{
    public:
        typedef Alloc   allocator_type;
        // typedef Alloc   data_allocator;

        using value_type        = typename allocator_type::value_type;
        using size_type         = typename allocator_type::size_type;
        using difference_type   = typename allocator_type::difference_type;
        using pointer           = typename allocator_type::pointer;
        using const_pointer     = typename allocator_type::const_pointer;
        using reference         = typename allocator_type::reference;
        using const_reference   = typename allocator_type::const_reference;

        using iterator          = value_type*;
        using const_iterator    = const value_type*;
        using reverse_iterator          = std::reverse_iterator<iterator>;          // 暂时使用 std::reverse_iterator
        using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

    private:
        
        iterator __start;
        iterator __end;
        // iterator __end_of_storage;
        compressed_pair<iterator, allocator_type> __end_cap;
        
        
    public:
        /***************ctor 、 copy_ctor 、 move_ctor 、 dtor 、 operator=*****************/
        // **** default ctor
        vector() noexcept{
            try{
                __start = __end_cap.allocate(16);
                __end = __start;
                __end_cap.data = __start + 16;
            }
            catch(...){
                __start = nullptr;
                __end = nullptr;
                __end_cap.data = nullptr;
            }
        }

        // **** fill ctor
        explicit vector(size_type n){
            allocate_and_fill(n, value_type());
        }

        explicit vector(size_type n, const value_type& val){
            allocate_and_fill(n, val);
        }

        // **** range ctor
        // 判断 InputIterator 是否为非整型，防止匹配错误
        template <class InputIterator, class = typename std::enable_if<!std::is_integral<InputIterator>::value>::type> // 元编程 enable_if
        vector(InputIterator first, InputIterator last){
            range_initialize(first, last);
        }
        
        // **** copy ctor
        vector(const vector& x){
            allocate_and_copy(x.begin(), x.end());
        }

        // **** move ctor
        vector(vector&& x) noexcept
            : __start(x.__start), __end(x.__end), __end_cap(x.__end_cap.data){
            x.__start = nullptr;
            x.__end = nullptr;
            x.__end_cap.data = nullptr;
        }

        // **** initializer list
        vector(std::initializer_list<value_type> il){
            allocate_and_copy(il.begin(), il.end());
        }

        // **** dtor
        ~vector(){
            destroy_and_deallocate_all();
        }

        // **** operator=
        // copy
        vector& operator=(const vector& x);
        // move
        vector& operator=(vector&& x);
        //initializer_list
        vector& operator=(std::initializer_list<value_type> il);

    public:
        /********************** Iterator 函数 *****************************/
        iterator                begin() noexcept { return __start; }
        const_iterator          begin() const noexcept { return __start; }
        iterator                end() noexcept { return __end; }
        const_iterator          end() const noexcept { return __end; }
        reverse_iterator        rbegin() noexcept { return reverse_iterator(end()); }
        const_reverse_iterator  rbegin() const noexcept { return reverse_iterator(end()); }
        reverse_iterator        rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator  rend() const noexcept { return reverse_iterator(begin()); }
        const_iterator          cbegin() const noexcept { return begin(); }
        const_iterator          cend() const noexcept { return end(); }
        const_reverse_iterator  crbegin() { return reverse_iterator(end()); }
        const_reverse_iterator  crend() { return reverse_iterator(begin()); }
        /********************** Capacity 函数 *****************************/
        size_type   size() const noexcept { return static_cast<size_type>(__end - __start); }
        size_type   max_size() const noexcept { return size_type(-1) / sizeof(value_type); }
        void        resize(size_type n) { return resize(n, value_type()); }
        void        resize(size_type n, const value_type& val);
        size_type   capacity() const noexcept { return static_cast<size_type>(__end_cap.data - __start); }
        bool        empty() const noexcept { return __start == __end; }
        void        reserve (size_type n);
        void        shrink_to_fit() { vector tmp(*this); swap(tmp); }
        /********************** Element Access 函数 *************************/
        reference       operator[] (size_type n) { return *(__start + n); }
        const_reference operator[] (size_type n) const{ return *(__start + n); }
        reference       at (size_type n){
            if (n < size()) return *(__start + n); 
            else throw std::out_of_range("vector : the parameter of [at] is out of range");
        }        
        const_reference at (size_type n) const{
            if (n < size()) return *(__start + n); 
            else throw std::out_of_range("vector : the parameter of [at] is out of range");
        }
        reference       front() { return *__start; }
        const_reference front() const { return *__start; }
        reference       back() { return *(__end - 1); }
        const_reference back() const { return *(__end - 1); }
        value_type*     data() noexcept { return __start; }
        const value_type* data() const noexcept { return  __start; }
        /********************** Modifiers 函数 ****************************/
        // assign
        template <class InputIterator, class = typename std::enable_if<!std::is_integral<InputIterator>::value>::type>
        void        assign (InputIterator first, InputIterator last);                           // range
        void        assign (size_type n, const value_type& val);                                // fill
        void        assign (std::initializer_list<value_type> il);                              // initializer_list
        // push_back
        void        push_back (const value_type& val);
        void        push_back (value_type&& val);
        
        void        pop_back();
        // insert
        iterator    insert (const_iterator position, const value_type& val);                    // single element
        iterator    insert (const_iterator position, size_type n, const value_type& val){       // fill
            return insert_fill(const_cast<iterator>(position), n, val);
        }
        template <class InputIterator, class = typename std::enable_if<!std::is_integral<InputIterator>::value>::type>  
        iterator    insert (const_iterator position, InputIterator first, InputIterator last){  //range
            return insert_range(const_cast<iterator>(position), first, last);
        }
        iterator    insert (const_iterator position, value_type&& val);                         // move
        // iterator    insert (const_iterator position, std::initializer_list<value_type> il);     // initializer_list
        // erase
        iterator    erase (const_iterator position);
        iterator    erase (const_iterator first, const_iterator last);
        void        swap (vector& x);
        void        clear() noexcept;
        template <class... Args>
        iterator    emplace (const_iterator position, Args&&... args);
        template <class... Args>
        void        emplace_back (Args&&... args);
        /**********************************其它*******************************/
        allocator_type get_allocator() const noexcept { return allocator_type(); }

        
       private:
        /***********************内存分配构造工具*****************************/
        void allocate_and_fill (size_type n, const value_type& val);
        template <class InputIterator>
        void allocate_and_copy (InputIterator first, InputIterator last);
        template <class... Args>
        void reallocate_and_emplace (iterator position, Args... arg);
        template <class InputIterator>
        void range_initialize (InputIterator first, InputIterator last);
        template <class InputIterator>
        void range_initialize_aux (InputIterator first, InputIterator last, std::true_type);
        template <class Integer>
        void range_initialize_aux (Integer n, const value_type& val, std::false_type);
        void destroy_and_deallocate_all();
    private:
        /***********************其他辅助函数*****************************/
        iterator insert_fill(iterator position, size_type n, const value_type& val);
        template <class InputIterator>
        iterator insert_range(iterator position, InputIterator first, InputIterator last);
    };



    /*-------------------------------部分函数定义------------------------------------*/
    //--------------------- operator=
    //copy
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(const vector<T, Alloc>& x){
        if(this != &x){
            const size_type len = x.size();
            if(len > capacity()){
                vector tmp(x.begin(), x.end());
                swap(tmp);
            }
            else if(len >= size()){
                pocket_stl::copy(x.begin(), x.begin() + size(), __start);
                uninitialized_copy(x.begin() + size(), x.end(), __end);
                __end = __start + len;
            }
            else if(len < size()){
                pocket_stl::copy(x.begin(), x.end(), __start);
                // data_allocator.destroy(__start + len, __end);
                destroy(__start + len, __end);
                __end = __start + len;
            }
        }
        return *this;
    }

    //move
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(vector<T, Alloc>&& x){
        destroy_and_deallocate_all();
        __start = x.__start;
        __end = x.__end;
        __end_cap.data = x.__end_cap.data;
        x.__start = x.__end = x.__end_cap.data = nullptr;
        return *this;
    }

    //initializer_list
    template <class T, class Alloc>
    vector<T, Alloc>& 
    vector<T, Alloc>::operator=(std::initializer_list<value_type> il){
        destroy_and_deallocate_all();
        allocate_and_copy(il.begin(), il.end());
    }

    //--------------------- capacity 函数
    template <class T, class Alloc>
    void 
    vector<T, Alloc>::resize(size_type n, const value_type& val){
        size_type size = size();
        if(n < size){
            erase(__start + n, __end);
            __end = __start + n;
        }
        else if(n > capacity()){
            insert(__end, n - size, val);
        }
        else{
            uninitialized_fill_n(__end, n - size, val);
            __end = __start + n;
        }
    }

    template <class T, class Alloc>
    void 
    vector<T, Alloc>::reserve(size_type n){
        if (n > max_size()) throw std::length_error("vector : the size requested is larger than the max_size");
        if (n <= capacity()) return;
        
        // pointer new_start = data_allocator.allocate(n);
        pointer new_start = __end_cap.allocate(n);
        pointer new_end = uninitialized_copy(__start, __end, new_start);
        destroy_and_deallocate_all();
        __start = new_start;
        __end_cap.data = new_start + n;
        __end = new_end;
    }

    //--------------------- Modifiers 函数
    template <class T, class Alloc>
    template <class InputIterator, class>
    void
    vector<T, Alloc>::assign(InputIterator first, InputIterator last){
        const size_type len = std::distance(first, last);
        if(len <= size()){
            iterator ptr = __start;
            for (; first < last; ++ptr, ++first){
                *ptr = *first;
            }
            auto new_end = ptr;
            for (; ptr < __end; ++ptr){
                // data_allocator.destroy(ptr);
                __end_cap.destroy(ptr);
            }
            __end = new_end;
        }
        else if(len <= capacity()){
            iterator ptr = __start;
            for (; ptr < __end; ++ptr, ++first){
                *ptr = *first;
            }
            __end = uninitialized_copy(first, last, ptr);
        }
        else{
            destroy_and_deallocate_all();
            allocate_and_copy(first, last);
        }
    }

    template <class T, class Alloc>
    void        
    vector<T, Alloc>::assign(size_type n, const value_type& val){
        if(n <= size()){
            iterator ptr = __start;
            for (size_type i = 0; i < n; ++i, ++ptr){
                *ptr = val;
            }
            for (; ptr < __end; ++ptr){
                // data_allocator.destroy(ptr);
                __end_cap.destroy(ptr);
            }
            __end = __start + n;
        }
        else if(n < capacity()){
            iterator ptr = __start;
            for (; ptr < __end; ++ptr){
                *ptr = val;
            }
            uninitialized_fill_n(ptr, n - size());
            __end = __start + n;
        }
        else{
            destroy_and_deallocate_all();
            allocate_and_fill(n, val);
        }
    }

    template <class T, class Alloc>
    void
    vector<T, Alloc>::assign(std::initializer_list<value_type> il){
        auto len = il.size();
        if(len <= size()){
            iterator ptr = __start;
            auto itr_il = il.begin();
            for (; itr_il < end(); ++ptr, ++itr_il){
                *ptr = *itr_il;
            }
            for (; ptr < __end; ++ptr){
                // data_allocator.destroy(ptr);
                __end_cap.destroy(ptr);
            }
            __end = __start + il.size();
        }
        else if(len <= capacity()){
            iterator ptr = __start;
            auto itr_il = il.begin();
            for (; ptr < __end; ++ptr, ++itr_il){
                *ptr = *itr_il;
            }
            __end = uninitialized_copy(itr_il, il.end(), ptr);
        }
        else{
            destroy_and_deallocate_all();
            allocate_and_copy(il.begin(), il.end());
        }
    }

    template <class T, class Alloc>
    void
    vector<T, Alloc>::push_back(const value_type& val){
        if(__end != __end_cap.data){
            // data_allocator.construct(&*__end, val);
            __end_cap.construct(&*__end, val);
            ++__end;
        }
        else{            
            const size_type new_size = 2 * size();
            // iterator new_start = data_allocator.allocate(new_size);
            iterator new_start = __end_cap.allocate(new_size);
            iterator new_end = new_start;
            try{
                new_end = uninitialized_copy(__start, __end, new_start);
                // data_allocator.construct(&*new_end, val);
                __end_cap.construct(&*new_end, val);
                ++new_end;
            }
            catch(...){
                destroy(new_start, new_end);
                // data_allocator.deallocate(new_start, new_end - new_start);
                __end_cap.deallocate(new_start, new_end - new_start);
                throw;
            }
            __start = new_start;
            __end = new_end;
            __end_cap.data = __start + new_size;
        }
    }

    template <class T, class Alloc>
    void
    vector<T, Alloc>::push_back(value_type&& val){
        emplace_back(std::move(val));
    }

    template <class T, class Alloc>
    void
    vector<T, Alloc>::pop_back(){
        if(!empty()){
            --__end;
            // data_allocator.destroy(&*__end);
            __end_cap.destroy(&*__end);
        }
        else
            throw;
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert(const_iterator position, const value_type& val){
        return insert(position, size_type(1), val);
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert(const_iterator position, value_type&& val){
        return emplace(position, std::move(val));
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::erase(const_iterator position){
        iterator pos_tmp = __start + (position - __start);
        if(pos_tmp + 1 != __end){
            pocket_stl::copy(pos_tmp + 1, __end, pos_tmp);
        }
        --__end;
        // data_allocator.destroy(__end);
        __end_cap.destroy(__end);
        return pos_tmp;
    }

    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::erase(const_iterator first, const_iterator last){
        iterator first_copy = __start + (first - __start);
        iterator last_copy = __start + (last - __start);
        iterator tmp = first_copy;
        if(last != __end){
            tmp = pocket_stl::copy(last_copy, __end, first_copy);
        }
        pocket_stl::destroy(tmp, last_copy);
        __end -= last - first;
        return first_copy;
    }

    template <class T, class Alloc>
    void 
    vector<T, Alloc>::swap(vector<T, Alloc>& x) {
        if(this != &x){             // 暂时调用 std::swap
            std::swap(__start, x.__start);
            std::swap(__end, x.__end);
            std::swap(__end_cap.data, x.__end_cap.data);
        }
    }

    template <class T, class Alloc>
    void
    vector<T, Alloc>::clear() noexcept{
        destroy(__start, __end);
        __end = __start;
    }

    template <class T, class Alloc>
    template <class... Args>
    typename vector<T, Alloc>::iterator 
    vector<T, Alloc>::emplace (const_iterator position, Args&&... args){
        const size_type elems_before_pos = position - __start;
        iterator pos_copy = __start + elems_before_pos;
        if(__end_cap.data != __end){
            if(position == __end){
                // data_allocator.construct(position, std::forward<Args>(args)...);
                __end_cap.construct(position, std::forward<Args>(args)...);
                __end++;
                return pos_copy;
            }
            else{
                // data_allocator.construct(&*__end, *(__end - 1));
                __end_cap.construct(&*__end, *(__end - 1));
                pocket_stl::copy_backward(pos_copy, __end - 1, __end);
                *position = vaule_type(std::forward<Args>(args)...);
                __end++;
                return pos_copy;
            }
        }
        else{
            reallocate_and_emplace(pos_copy, args...);
        }
        return __start + elems_before_pos;
    }

    template <class T, class Alloc>
    template <class... Args>
    void
    vector<T, Alloc>::emplace_back(Args&&... args){
        if(__end != __end_cap.data){
            // data_allocator.construct(&*__end, std::forward<Args>(args)...);
            __end_cap.construct(&*__end, std::forward<Args>(args)...);
            ++__end;
        }
        else{
            reallocate_and_emplace(__end, args...);
        }
    }

    // -------------------- 内存分配工具
    template <class T, class Alloc>
    void 
    vector<T, Alloc>::allocate_and_fill(size_type n, const value_type& val) {
        // __start = data_allocator.allocate(n);
        __start = __end_cap.allocate(n);
        __end = __start + n;
        uninitialized_fill_n(__start, n, val);
        __end_cap = __end;
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void 
    vector<T, Alloc>::allocate_and_copy(InputIterator first, InputIterator last){
        // __start = data_allocator.allocate(last - first);
        __start = __end_cap.allocate(last - first);
        __end = uninitialized_copy(first, last, __start);
        __end_cap = __end;
    }

    template <class T, class Alloc>
    template <class... Args>
    void
    vector<T, Alloc>::reallocate_and_emplace (iterator position, Args... arg){
        const size_type new_size = 2 * size() < max_size() ? 2 * size() : size() + 1;
        // iterator new_start = data_allocator.allocate(new_size);
        iterator new_start = __end_cap.allocate(new_size);
        iterator new_end = new_start;
        try{
            new_end = uninitialized_copy(__start, position, new_start);
            // data_allocator.construct(&*new_end, std::forward<Args>(arg)...);
            __end_cap.construct(&*new_end, std::forward<Args>(arg)...);
            new_end++;
            new_end = uninitialized_copy(position, __end, new_end);
        }
        catch(...){
            // data_allocator.deallocate(new_start, new_end - new_start);
            __end_cap.deallocate(new_start, new_end - new_start);
            throw;
        }
        destroy_and_deallocate_all();
        __start = new_start;
        __end = new_end;
        __end_cap.data = __start + new_size;
    }


    template <class T, class Alloc>
    template <class InputIterator>
    void 
    vector<T, Alloc>::range_initialize(InputIterator first, InputIterator last){
        // range_initialize_aux(first, last, typename std::is_integral<InputIterator>::type());
        allocate_and_copy(first, last);
    }

    // template <class T, class Alloc>
    // template <class InputIterator>
    // void 
    // vector<T, Alloc>::range_initialize_aux(InputIterator first, InputIterator last, std::true_type){
    //     allocate_and_copy(first, last);
    // }

    // // template <class T, class Alloc>
    // template <class Integer>
    // void 
    // vector<T, Alloc>::range_initialize_aux(Integer n, const value_type& val, std::false_type){
    //     __start = allocate(static_cast<size_type>(n));
    //     uninitialized_fill_n(__start, n, val);
    //     __end = __start + n;
    //     __end_of_storage = __end;
    // }

    template <class T, class Alloc>
    void 
    vector<T, Alloc>::destroy_and_deallocate_all (){
        pocket_stl::destroy(__start, __end);
        // data_allocator.deallocate(__start, __end_of_storage - __start);
        __end_cap.deallocate(__start, __end_cap.data - __start);
    }
    
    // -------------------- 其他辅助函数
    template <class T, class Alloc>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert_fill(iterator position, size_type n, const value_type& val){
        if(n != 0){
            const size_type pos_before = position - __start;
            if(size_type(__end_cap.data - __end) >= n){
                const size_type elems_after = __end - position;
                iterator old_end = __end;
                if(elems_after > n){
                    uninitialized_copy(__end - n, old_end, __end);
                    __end += n;
                    pocket_stl::copy_backward(position, old_end - n, old_end);
                    pocket_stl::fill(position, position + n, val);
                }
                else{
                    uninitialized_fill_n(__end, n - elems_after, val);
                    __end = position + n;
                    uninitialized_copy(position, old_end, position + n);
                    __end += elems_after;
                    pocket_stl::fill(position, old_end, val);
                }
            }
            else{
                // 需要分配新的内存
                const size_type old_size = size();
                const size_type len = old_size + std::max(old_size, n);
                // iterator new_start = data_allocator.allocate(len);
                iterator new_start = __end_cap.allocate(len);
                iterator new_end = new_start;
                try{
                    new_end = uninitialized_copy(__start, position, new_start);
                    new_end = uninitialized_fill_n(new_end, n, val);
                    new_end = uninitialized_copy(position, __end, new_end);
                }
                catch(...){
                    destroy(new_start, new_end);
                    // data_allocator.deallocate(new_start, new_end - new_start);
                    __end_cap.deallocate(new_start, new_end - new_start);
                    throw;
                }
                destroy_and_deallocate_all();
                __start = new_start;
                __end = new_end;
                __end_cap.data = __start + len;
            }
            return __start + pos_before;
        }
    }

    template <class T, class Alloc>
    template <class InputIterator>
    typename vector<T, Alloc>::iterator
    vector<T, Alloc>::insert_range(iterator position, InputIterator first, InputIterator last){
        if (first == last) return position;
        const size_type n = std::distance(first, last);
        // const size_type pos_before = position - __start;
        if (size_type(__end_cap.data - __end) > n){
            const size_type elems_after = __end - position;
            iterator old_end = __end;
            if (elems_after > n){
                uninitialized_copy(old_end - n, old_end, old_end);
                __end += n;
                pocket_stl::copy_backward(position, old_end - n, old_end);
                pocket_stl::copy(first, last, position);
            }
            else{
                InputIterator mid = first;
                std::advance(mid, elems_after);
                __end = uninitialized_copy(mid, last, __end);
                __end = uninitialized_copy(position, old_end, __end);
                uninitialized_copy(first, mid, position);
            }
            return position;
        }
        else{
            // 需要重新分配空间
            const size_type elems_before_pos = position - __start;
            const size_type old_size = size();
            const size_type len = old_size + std::max(old_size, n);
            // iterator new_start = data_allocator.allocate(len);
            iterator new_start = __end_cap.allocate(len);
            iterator new_end = new_start;
            try{
                new_end = uninitialized_copy(__start, position, new_start);
                new_end = uninitialized_copy(first, last, new_end);
                new_end = uninitialized_copy(position, __end, new_end);
            }
            catch(...){
                destroy(new_start, new_end);
                // data_allocator.deallocate(new_start, new_end - new_start);
                __end_cap.deallocate(new_start, new_end - new_start);
                throw;
            }
            __start = new_start;
            __end = new_end;
            __end_cap.data = __start + len;
            return __start + elems_before_pos;
        }
    }

    //****************************非成员函数************************************/
    /****************************relational operator****************************/
    template <class T, class Alloc>
    bool operator== (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        if (lhs.size() != rhs.size()) return false;
        else{
            for (int i = 0; i < lhs.size(); ++i){
                if (!(lhs[i] == rhs[i])) return false;
            }
            return true;
        }
    }

    template <class T, class Alloc>
    bool operator!= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        return !(operator==(lhs, rhs));
    }

    template <class T, class Alloc>
    bool operator<  (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template <class T, class Alloc>
    bool operator>  (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        return rhs < lhs;
    }

    template <class T, class Alloc>
    bool operator<= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        return !(rhs < lhs);
    }

    template <class T, class Alloc>
    bool operator>= (const vector<T,Alloc>& lhs, const vector<T,Alloc>& rhs){
        return !(lhs < rhs);
    }

    template <class T, class Alloc>
    void swap (vector<T,Alloc>& x, vector<T,Alloc>& y){
        return x.swap(y);
    }

} // namespace

#endif