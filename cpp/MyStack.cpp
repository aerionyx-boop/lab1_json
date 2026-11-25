#include "MyStack.hpp"
MyStack::MyStack():List(true) {}
void MyStack::push(string s){append(s);}
string MyStack::pop(){check(tail,"empty container");return eraseAfter(prev(tail));}
string MyStack::peek()const{check(tail,"empty container");return tail->s;}
