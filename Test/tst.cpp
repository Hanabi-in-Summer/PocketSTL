#include <iostream>
#include "../STL/allocator.h"
#include <vector>

/*
** allocator 配不上标准接口
** 
*/

using std::vector;

int main() {
    std::vector<int, pocket_stl::allocator<vector<int>>> a(3);
    for (int i = 0; i < a.size(); ++i){
        std::cout << a[i] << ' ';
    }
    return 0;
}