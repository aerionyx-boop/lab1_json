#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <utility>
using namespace std;
using V = vector<string>;
inline void check(bool b, string m = "invalid argument") {
    if (!b) {
        throw runtime_error(m);
    }
}
struct Data {
    virtual ~Data() = default;
    virtual V values() const = 0;
    virtual void append(string) = 0;
};
