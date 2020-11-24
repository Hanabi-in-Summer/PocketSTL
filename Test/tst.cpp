#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <iostream>
#include "../STL/allocator.h"
#include "../STL/vector.h"
#include "../STL/list.h"
#include "../STL/deque.h"
#include "../STL/stack.h"
#include <list>
#include <vector>
#include <stack>
#include <deque>


/*
** 
** 
*/

using std::cout;
using std::endl;
//using std::vector;
//#define pocket_stl std
int main() {
    cout << "-------vector-------" << endl;
    // vector<int, pocket_stl::allocator<int>> tst;
    do{
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
    } while (0);
    cout << "-------list-------" << endl;
    do{
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
    } while (0);
    cout << "-------deque-------" << endl;
    do{
        pocket_stl::deque<int> deq{1, 2, 3};
        for (auto i = deq.begin(); i != deq.end(); ++i){
            cout << *i << ' ';
        }
        cout << endl;
        deq.push_back(8);
        deq.push_front(18);
        deq.emplace_back(18);
        deq.pop_back();
        auto itr = deq.end();
        itr -= 2;
        deq.insert(itr, 3, 10);
        for (int i = 0; i < deq.size(); ++i){
            cout << deq[i] << ' ';
        }
        cout << endl;
        itr = deq.begin();
        itr += 3;
        deq.erase(itr);

        // deq.push_front(18);
        for (int i = 0; i < deq.size(); ++i){
            cout << deq[i] << ' ';
        }
        deq.erase(itr, deq.end());
        cout << endl;
        for (int i = 0; i < deq.size(); ++i){
            cout << deq[i] << ' ';
        }
        //     // auto itr = deq.end();
        //     // cout << *(--itr);
        
        pocket_stl::deque<int> com_deq1;
        pocket_stl::deque<int> com_deq2;
        com_deq1.push_back(5);
        com_deq2.push_back(8);
        cout << '\n' << ((com_deq1 < com_deq2) ? "true" : "false");
        cout << endl;
    } while (0);
    cout << "-------stack-------" << endl;
    do{
        pocket_stl::stack<int> stk;
        stk.push(8);
        for (int i = 0; i < 5; ++i){
            stk.push(i);
        }
        while(!stk.empty()){
            int tmp = stk.top();
            stk.pop();
            cout << tmp << ' ';
        }
        cout << "empty\n";
        stk.push(7);
        pocket_stl::stack<int> stk2;
        stk2.push(5);
        cout << stk2.top() << stk.top();
        cout << '\n' << ((stk2 < stk) ? "true" : "false");
    } while (0);
    // cout << '\n' << (stk2 < stk);
    cout << "\n-------end-------\n";
    return 0;
}