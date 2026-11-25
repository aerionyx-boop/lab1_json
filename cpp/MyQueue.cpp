#include "MyQueue.hpp"
MyQueue::MyQueue():List(false) {}
void MyQueue::push(string s){append(s);}
string MyQueue::pop(){check(head,"empty container");return eraseAfter(nullptr);}
string MyQueue::peek()const{check(head,"empty container");return head->s;}
