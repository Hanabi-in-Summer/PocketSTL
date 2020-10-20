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
    pocket_stl::vector<int, pocket_stl::allocator<int>> tst(5, 5);
    for (int i = 0; i < tst.size(); ++i){
        std::cout << tst[i] << ' ';
    }
    std::cout << endl;
    int a = 10;
    tst.erase(tst.begin());
    for (int i = 0; i < tst.size(); ++i){
        std::cout << tst[i] << ' ';
    }
    return 0;
}