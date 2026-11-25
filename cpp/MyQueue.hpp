#pragma once
#include "ForwardList.hpp"
struct MyQueue : List {
    MyQueue();
    void push(string s);
    string pop();
    string peek() const;
};
