#include "../STL/stack.h"
#include <iostream>

using std::cout;
using std::endl;
using namespace pocket_stl;
int main(){
    stack<int> t1;
    t1.push(5);
    stack<int> t2;
    t2.push(9);
    cout << ((t1 < t2) ? "true" : "false");
}