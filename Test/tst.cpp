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
    for (int i = 0; i < tst.size(); ++i){
        std::cout << tst[i] << ' ';
    }
    std::cout << sizeof(tst) << endl;
    
    int a = 10;
    tst.erase(tst.begin(), tst.begin() + 2);
    for (int i = 0; i < tst.size(); ++i){
        std::cout << tst[i] << ' ';
    }
    cout << endl;
    std::cout << tst.size() << ' ' << tst.capacity() << endl;

    
    tst.insert(tst.begin(), aux.begin(), aux.end());
    for (int i = 0; i < tst.size(); ++i){
        std::cout << tst[i] << ' ';
    }

    return 0;
}