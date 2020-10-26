#include <iostream>
#include "../STL/allocator.h"
#include "../STL/vector.h"
#include "../STL/list.h"
#include <list>
#include <vector>

/*
** 
** 
*/

using std::cout;
using std::endl;
using std::vector;

int main() {
    cout << "-------vector-------" << endl;
    // vector<int, pocket_stl::allocator<int>> tst;
    const pocket_stl::vector<int> tst{1, 2, 3, 4, 5};
    pocket_stl::vector<int> aux(3, 88);

    for(auto& elem : aux){
        cout << elem << ' ';
    }
    
    cout << endl;
    // aux.assign(tst.begin(), tst.begin() + 1);
    aux = tst;
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
    cout << sizeof(tst);
    cout << endl;
    cout << "-------list-------" << endl;
    pocket_stl::list<int> list_tst(5, 5);
    cout << sizeof(list_tst) << endl;

    return 0;
}