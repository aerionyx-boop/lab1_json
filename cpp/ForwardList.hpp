#pragma once
#include "Data.hpp"
struct List : Data {
 struct Node { string s; Node* next=nullptr; Node* prev=nullptr; };
 Node *head=nullptr,*tail=nullptr; bool doub; size_t n=0;
 explicit List(bool d=false);
 ~List() override;
 Node* find(string s) const;
 Node* prev(Node* p) const;
 Node* at(size_t i) const;
 void after(Node* p,string s);
 void append(string s) override;
 string eraseAfter(Node* p);
 V values() const override;
 V reverse() const;
};
struct Forwardlist : List { Forwardlist(); };
