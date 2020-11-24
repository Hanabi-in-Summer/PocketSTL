#ifndef _pocket_QUEUE_H_
#define _pocket_QUEUE_H_

/*
** queue
** 
*/

#include "deque.h"

namespace pocket_stl{
    template <class T, class Container = deque<T> > class queue{
    public:
        using value_type        = T;
        using container_type    = Container;
        using reference         = value_type&;
        using const_reference   = const value_type&;
        using size_type         = typename Container::size_type;

    private:
        container_type __c;

    public:
        
    };
}

#endif