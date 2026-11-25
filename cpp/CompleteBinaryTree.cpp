#include "CompleteBinaryTree.hpp"
void CompleteBinaryTree::clear(Node*p){if(p){clear(p->l);clear(p->r);delete p;}}CompleteBinaryTree::~CompleteBinaryTree(){clear(root);} // heap-style path, no array backing
 void CompleteBinaryTree::append(string s){size_t k=++n;if(k==1){root=new Node{s};return;}size_t bit=1;while(bit<=k/2)bit*=2;auto p=root;for(bit/=2;bit>1;bit/=2)p=(k&bit)?p->r:p->l;((k&1)?p->r:p->l)=new Node{s};}
 bool CompleteBinaryTree::has(Node*p,const string&s){return p&&(p->s==s||has(p->l,s)||has(p->r,s));}
 size_t CompleteBinaryTree::count(Node*p){return p?1+count(p->l)+count(p->r):0;}
 bool CompleteBinaryTree::complete(Node*p,size_t i,size_t n){return !p||(i<n&&complete(p->l,2*i+1,n)&&complete(p->r,2*i+2,n));}
 void CompleteBinaryTree::walk(Node*p,V&v,string mode){if(!p)return;if(mode=="PRE")v.push_back(p->s);walk(p->l,v,mode);if(mode=="IN")v.push_back(p->s);walk(p->r,v,mode);if(mode=="POST")v.push_back(p->s);}
 V CompleteBinaryTree::values()const{V v;if(!root)return v;vector<Node*> q{root};for(size_t i=0;i<q.size();i++){auto p=q[i];v.push_back(p->s);if(p->l)q.push_back(p->l);if(p->r)q.push_back(p->r);}return v;}

#include <iostream>
namespace {
void showNode(CompleteBinaryTree::Node* p,const string& indent,const string& label){
 if(!p)return;
 cout<<indent<<label<<p->s<<'\n';
 if(p->l)showNode(p->l,indent+"    ","L: ");
 if(p->r)showNode(p->r,indent+"    ","R: ");
}
}
void CompleteBinaryTree::show() const {if(!root)cout<<"(empty)\n";else showNode(root,"","Root: ");}
