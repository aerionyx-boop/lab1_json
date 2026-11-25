#include "ForwardList.hpp"
List::List(bool d):doub(d){} List::~List(){while(head){auto p=head;head=head->next;delete p;}}
 List::Node* List::find(string s)const{for(auto p=head;p;p=p->next)if(p->s==s)return p;return nullptr;}
 List::Node* List::prev(Node*p)const{if(doub)return p->prev;if(p==head)return nullptr;for(auto q=head;q;q=q->next)if(q->next==p)return q;return nullptr;}
 List::Node* List::at(size_t i)const{check(i<n,"index out of range");auto p=head;while(i--)p=p->next;return p;}
 void List::after(Node*p,string s){auto q=new Node{s,p?p->next:head,doub?p:nullptr};if(p)p->next=q;else head=q;if(q->next&&doub)q->next->prev=q;if(!q->next)tail=q;n++;}
 void List::append(string s){after(tail,s);} string List::eraseAfter(Node*p){auto q=p?p->next:head;check(q!=nullptr,"no element");string s=q->s;if(p)p->next=q->next;else head=q->next;if(q->next&&doub)q->next->prev=p;if(q==tail)tail=p;delete q;n--;return s;}
 V List::values()const{V v;for(auto p=head;p;p=p->next)v.push_back(p->s);return v;} V List::reverse()const{V v;if(doub){for(auto p=tail;p;p=p->prev)v.push_back(p->s);}else{v=values();for(size_t i=0;i<v.size()/2;i++)swap(v[i],v[v.size()-1-i]);}return v;}
Forwardlist::Forwardlist():List(false) {}
