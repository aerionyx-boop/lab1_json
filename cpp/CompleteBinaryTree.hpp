#pragma once
#include "Data.hpp"
struct CompleteBinaryTree : Data {
    struct Node {
        string s;
        Node *l = nullptr, *r = nullptr;
    };
    Node *root = nullptr;
    size_t n = 0;
    static void clear(Node *p);
    ~CompleteBinaryTree() override;
    void append(string s) override;
    static bool has(Node *p, const string &s);
    static size_t count(Node *p);
    static bool complete(Node *p, size_t i, size_t n);
    static void walk(Node *p, V &v, string mode);
    void show() const;
    V values() const override;
};
