#include <iostream>
#include "../STL/allocator.h"
#include "../STL/vector.h"
#include <vector>

/*
** allocator 配不上标准接口
** 
*/

using std::cout;
using std::endl;
using std::vector;

int main() {
    // vector<int, pocket_stl::allocator<int>> tst;
    pocket_stl::vector<int> tst{1, 2, 3, 4, 5};
    pocket_stl::vector<int> aux(3, 88);

    cout << tst.capacity() << endl;
    // cout << tst.max_size() << endl;
    tst.reserve(100);
    cout << tst.capacity() << endl;
    // tst.shrink_to_fit();
    // cout << tst.capacity() << endl;

    return 0;
}