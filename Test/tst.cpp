#include <iostream>
#include "../STL/allocator.h"
#include "../STL/vector.h"
#include <vector>

/*
** 
** 
*/

using std::cout;
using std::endl;
using std::vector;

int main() {
    // vector<int, pocket_stl::allocator<int>> tst;
    pocket_stl::vector<int> tst{1, 2, 3, 4, 5};
    pocket_stl::vector<int> aux(3, 88);

    for(auto& elem : tst){
        cout << elem << ' ';
    }
    cout << endl;
    aux.assign(tst.begin(), tst.begin() + 1);
    for(auto& elem : aux){
        cout << elem << ' ';
    }

    aux.push_back(15);
    cout << endl;
    for(auto& elem : aux){
        cout << elem << ' ';
    }
    aux.pop_back();
cout << endl;
    for(auto& elem : aux){
        cout << elem << ' ';
    }

    cout << endl;
    cout << endl;


    return 0;
}