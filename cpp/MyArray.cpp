#include "MyArray.hpp"
void Array::insert(size_t i, string s) {
    check(i <= n, "index out of range");
    if (n == cap) {
        cap = cap ? cap * 2 : 4;
        auto b = make_unique<string[]>(cap);
        for (size_t j = 0; j < n; j++) {
            b[j] = std::move(a[j]);
        }
        a = std::move(b);
    }
    for (size_t j = n; j > i; j--) {
        a[j] = std::move(a[j - 1]);
    }
    a[i] = s;
    n++;
}
void Array::append(string s) {
    insert(n, s);
}
string &Array::at(size_t i) {
    check(i < n, "index out of range");
    return a[i];
}
string Array::erase(size_t i) {
    string s = at(i);
    for (size_t j = i + 1; j < n; j++) {
        a[j - 1] = std::move(a[j]);
    }
    a[--n].clear();
    return s;
}
V Array::values() const {
    V v;
    for (size_t i = 0; i < n; i++) {
        v.push_back(a[i]);
    }
    return v;
}
