#include <iostream>
#include "../STL/allocator.h"
#include "../STL/vector.h"
#include "../STL/list.h"
#include "../STL/deque.h"
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
    pocket_stl::list<int> tst2(4, 4);
    pocket_stl::list<int> tst3(6, 6);
    pocket_stl::list<int> tst4(5, 5);
    pocket_stl::list<int> tst5(4, 7);
    tst2.merge(tst3);
    tst4.merge(tst5);
    tst2.merge(tst4);
    tst2.splice(tst2.begin(), list_tst, ++list_tst.begin());
    for(auto& elem : tst2){
        cout << elem << ' ';
    }
    cout << endl;
    tst2.unique();

    for(auto& elem : tst2){
        cout << elem << ' ';
    }
    cout << endl;
    pocket_stl::list<int> tst6{8, 9, 4, 5, 7, 6, 1};
    tst6.swap(tst2);
    cout << tst6.empty();
    tst6.reverse();
    for(auto& elem : tst6){
        cout << elem << ' ';
    }
    cout << endl;

    cout << "-------list-------" << endl;
    pocket_stl::deque<int> deq{1, 2, 3};

    return 0;
}