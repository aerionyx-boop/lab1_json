#pragma once
#include "ForwardList.hpp"
struct MyStack : List { MyStack(); void push(string s); string pop(); string peek() const; };
