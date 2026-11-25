#pragma once
#include "Data.hpp"
struct Array : Data {
 unique_ptr<string[]> a; size_t n=0, cap=0;
 void insert(size_t i,string s);
 void append(string s) override;
 string& at(size_t i);
 string erase(size_t i);
 V values() const override;
};
