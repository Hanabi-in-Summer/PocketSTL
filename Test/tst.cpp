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
    cout << list_tst.size() << endl;
    for (auto x = list_tst.begin(); x != list_tst.end(); --x){
        cout << *x << ' ';
    }
    list_tst.erase(list_tst.begin(), list_tst.end());
    cout << "qqqq";
    cout << list_tst.size() << endl;
    list_tst.insert(list_tst.begin(), 2, 8);

    for (auto x = list_tst.begin(); x != list_tst.end(); --x){
        cout << *x << ' ';
    }
    cout << "pp" << endl;
    cout << list_tst.size() << endl;
    list_tst.resize(9, 7);
    for (auto x = --list_tst.end(); x != list_tst.begin(); --x){
        cout << *x << ' ';
    }
     cout << endl;
    cout << list_tst.size() << endl;
    list_tst.clear();

    cout << endl
         << list_tst.size() << endl;

    return 0;
}