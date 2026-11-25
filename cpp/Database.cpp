#include "Database.hpp"
#include "MyArray.hpp"
#include "ForwardList.hpp"
#include "DoublyList.hpp"
#include "MyStack.hpp"
#include "MyQueue.hpp"
#include "CompleteBinaryTree.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <filesystem>
size_t idx(const string &s) {
    check(!s.empty() && s.find_first_not_of("0123456789") == string::npos, "invalid index");
    return stoull(s);
}
unique_ptr<Data> make(char t) {
    if (t == 'M') {
        return make_unique<Array>();
    }
    if (t == 'T') {
        return make_unique<CompleteBinaryTree>();
    }
    check(string("FLQS").find(t) != string::npos, "unknown type");
    if (t == 'F') {
        return make_unique<Forwardlist>();
    }
    if (t == 'L') {
        return make_unique<Doublelist>();
    }
    if (t == 'S') {
        return make_unique<MyStack>();
    }
    return make_unique<MyQueue>();
}

void load(DB &db, string file) {
    ifstream f(file);
    if (!f) {
        check(!std::filesystem::exists(file), "cannot read file");
        return;
    }
    stringstream buffer;
    buffer << f.rdbuf();
    check(!f.bad(), "read error");
    for (auto &[name, p] : json::decode(buffer.str())) {
        auto d = make(p.first);
        for (auto &value : p.second) {
            d->append(value);
        }
        db.emplace(name, make_pair(p.first, std::move(d)));
    }
}
void save(DB &db, string file) {
    json::Records records;
    for (auto &[name, p] : db) {
        records.emplace(name, make_pair(p.first, p.second->values()));
    }
    string tmp = file + ".tmp";
    ofstream f(tmp);
    check(bool(f), "cannot write file");
    f << json::encode(records);
    f.close();
    check(bool(f), "write error");
    check(rename(tmp.c_str(), file.c_str()) == 0, "rename error");
}
void print(V v) {
    for (size_t i = 0; i < v.size(); i++) {
        cout << (i ? " " : "") << v[i];
    }
    cout << '\n';
}
bool run(DB &db, V a, bool reportMode) {
    check(a.size() >= 2, "command and name required");
    string c = a[0], name = a[1];
    auto ar = [&](size_t n) { check(a.size() == n + 2, "wrong argument count"); };
    auto val = [&](size_t i) { return a.at(i + 2); };
    if (c == "PRINT" || c == "PRINTJSON") {
        ar(0);
        check(db.count(name), "unknown container");
        const auto &[type, data] = db.at(name);
        if (c == "PRINTJSON") {
            cout << json::array(data->values()) << '\n';
        } else if (!reportMode) {
            print(data->values());
        } else if (type == 'T') {
            cout << "Tree '" << name << "':\n";
            dynamic_cast<const CompleteBinaryTree &>(*data).show();
        } else {
            auto values = data->values();
            cout << name << ": ";
            if (type == 'F' || type == 'L') {
                for (const auto &value : values) {
                    cout << value << (type == 'F' ? " -> " : " <-> ");
                }
                cout << "NULL\n";
            } else if (type == 'M') {
                cout << "[ ";
                for (const auto &value : values) {
                    cout << value << ' ';
                }
                cout << "]\n";
            } else if (type == 'S') {
                print(dynamic_cast<const List &>(*data).reverse());
            } else {
                print(values);
            }
        }
        return false;
    }
    char t = c == "ISMEMBER" ? 'T' : c[0];
    const string original = c;
    if (c == "MUPDATE") {
        c = "MSET";
    }
    if (c == "MDEL_AT") {
        c = "MDEL";
    }
    if (t == 'F' || t == 'L') {
        static const map<string, string> aliases{{"PUSH_FRONT", "PUSHHEAD"},
                                                 {"PUSH_BACK", "PUSHTAIL"},
                                                 {"POP_FRONT", "DELHEAD"},
                                                 {"POP_BACK", "DELTAIL"},
                                                 {"INSERT_BEFORE_VAL", "PUSHBEFORE"},
                                                 {"INSERT_AFTER_VAL", "PUSHAFTER"},
                                                 {"DEL_VALUE", "DEL"},
                                                 {"DEL_BEFORE", "DELBEFORE"},
                                                 {"DEL_AFTER", "DELAFTER"}};
        auto alias = aliases.find(c.substr(1));
        if (alias != aliases.end()) {
            c = t + alias->second;
        }
    }
    auto printRemoved = [&](const string &value) {
        if (!reportMode || c == original) {
            cout << value << '\n';
        }
    };
    bool create = c == string(1, t) + "CREATE";
    bool autoCreate = c == "MPUSH" || c == "FPUSH" || c == "LPUSH" || c == "QPUSH" ||
                      c == "SPUSH" || c == "TINSERT" || c == "FPUSHHEAD" || c == "FPUSHTAIL" ||
                      c == "LPUSHHEAD" || c == "LPUSHTAIL";
    bool fresh = !db.count(name);
    if (fresh) {
        check(create || autoCreate, "unknown container");
        db.emplace(name, make_pair(t, make(t)));
    }
    auto &p = db.at(name);
    check(p.first == t, "wrong container type");
    if (create) {
        ar(0);
        check(fresh, "already exists");
        return true;
    }
    auto &d = *p.second;
    if (t == 'M') {
        auto &m = dynamic_cast<Array &>(d);
        if (c == "MPUSH") {
            ar(1);
            m.append(val(0));
        } else if (c == "MINSERT") {
            ar(2);
            m.insert(idx(val(0)), val(1));
        } else if (c == "MDEL") {
            ar(1);
            printRemoved(m.erase(idx(val(0))));
        } else if (c == "MDEL_VALUE") {
            ar(1);
            for (size_t i = 0; i < m.n; i++) {
                if (m.at(i) == val(0)) {
                    m.erase(i);
                    return true;
                }
            }
            return false;
        } else if (c == "MSET") {
            ar(2);
            m.at(idx(val(0))) = val(1);
        } else if (c == "MGET") {
            ar(1);
            cout << m.at(idx(val(0))) << '\n';
            return false;
        } else if (c == "MLEN") {
            ar(0);
            cout << m.n << '\n';
            return false;
        } else {
            throw runtime_error("unknown command");
        }
    } else if (t == 'T') {
        auto &tr = dynamic_cast<CompleteBinaryTree &>(d);
        if (c == "TSHOW") {
            ar(0);
            tr.show();
            return false;
        } else if (c == "TINSERT") {
            ar(1);
            tr.append(val(0));
        } else if (c == "TGET" || c == "ISMEMBER") {
            ar(1);
            cout << (CompleteBinaryTree::has(tr.root, val(0)) ? "TRUE" : "FALSE") << '\n';
            return false;
        } else if (c == "TCOMPLETE") {
            ar(0);
            cout << (CompleteBinaryTree::complete(tr.root, 0, CompleteBinaryTree::count(tr.root))
                         ? "TRUE"
                         : "FALSE")
                 << '\n';
            return false;
        } else if (c == "TBFS" || c == "TIN" || c == "TPRE" || c == "TPOST") {
            ar(0);
            V v;
            if (c == "TBFS") {
                v = tr.values();
            } else {
                CompleteBinaryTree::walk(tr.root, v, c.substr(1));
            }
            print(v);
            return false;
        } else {
            throw runtime_error("unknown command");
        }
    } else {
        auto &l = dynamic_cast<List &>(d);
        string op = c.substr(1);
        if (t == 'S') {
            auto &st = dynamic_cast<MyStack &>(d);
            if (op == "PUSH") {
                ar(1);
                st.push(val(0));
                if (!reportMode) {
                    cout << val(0) << '\n';
                }
            } else if (op == "POP") {
                ar(0);
                cout << st.pop() << '\n';
            } else if (op == "PEEK") {
                ar(0);
                cout << st.peek() << '\n';
                return false;
            } else {
                throw runtime_error("unknown command");
            }
        } else if (t == 'Q') {
            auto &qu = dynamic_cast<MyQueue &>(d);
            if (op == "PUSH") {
                ar(1);
                qu.push(val(0));
            } else if (op == "POP") {
                ar(0);
                cout << qu.pop() << '\n';
            } else if (op == "PEEK") {
                ar(0);
                cout << qu.peek() << '\n';
                return false;
            } else {
                throw runtime_error("unknown command");
            }
        } else {
            auto target = [&]() {
                auto p = l.find(val(0));
                check(p, "value not found");
                return p;
            };
            if (op == "INSERT_BEFORE_AT" || op == "INSERT_AFTER_AT") {
                ar(2);
                auto node = l.at(idx(val(0)));
                l.after(op == "INSERT_BEFORE_AT" ? l.prev(node) : node, val(1));
            } else if (op == "DEL_AT") {
                ar(1);
                l.eraseAfter(l.prev(l.at(idx(val(0)))));
            } else if (op == "DEL_ALL") {
                ar(1);
                auto value = val(0);
                List::Node *previous = nullptr;
                auto node = l.head;
                while (node) {
                    auto next = node->next;
                    if (node->s == value) {
                        l.eraseAfter(previous);
                    } else {
                        previous = node;
                    }
                    node = next;
                }
            } else if (op == "PUSH" || op == "PUSHTAIL") {
                ar(1);
                l.append(val(0));
            } else if (op == "PUSHHEAD") {
                ar(1);
                l.after(nullptr, val(0));
            } else if (op == "PUSHBEFORE" || op == "PUSHAFTER") {
                ar(2);
                auto p = target();
                l.after(op == "PUSHBEFORE" ? l.prev(p) : p, val(1));
            } else if (op == "DELHEAD" || op == "DELTAIL") {
                ar(0);
                check(l.head, "empty list");
                printRemoved(l.eraseAfter(op == "DELHEAD" ? nullptr : l.prev(l.tail)));
            } else if (op == "DEL" || op == "DELBEFORE" || op == "DELAFTER") {
                ar(1);
                auto p = l.find(val(0));
                if (!p && original != c && op == "DEL") {
                    return false;
                }
                check(p, "value not found");
                if (op == "DELBEFORE") {
                    p = l.prev(p);
                    check(p, "no predecessor");
                }
                printRemoved(l.eraseAfter(op == "DELAFTER" ? p : l.prev(p)));
            } else if (op == "GET") {
                ar(1);
                if (reportMode) {
                    cout << (l.find(val(0)) ? "TRUE" : "FALSE") << '\n';
                } else {
                    cout << l.at(idx(val(0)))->s << '\n';
                }
                return false;
            } else if (op == "FIND") {
                ar(1);
                long i = 0;
                auto p = l.head;
                while (p && p->s != val(0)) {
                    p = p->next;
                    i++;
                }
                cout << (p ? i : -1) << '\n';
                return false;
            } else if (op == "READ" || op == "REVERSE") {
                ar(0);
                print(op == "READ" ? l.values() : l.reverse());
                return false;
            } else {
                throw runtime_error("unknown command");
            }
        }
    }
    return true;
}
