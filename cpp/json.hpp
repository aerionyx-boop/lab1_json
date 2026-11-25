#pragma once
#include "Data.hpp"
#include <map>
#include <cctype>
#include <sstream>
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#define LAB_NLOHMANN 1
#endif
// Minimal JSON writer for arrays of strings (not a third-party JSON library).
namespace json {
inline string quote(const string &s) {
    string out = "\"";
    const char *hex = "0123456789abcdef";
    for (unsigned char c : s) {
        if (c == '"' || c == '\\') {
            out += '\\';
            out += char(c);
        } else if (c < 32) {
            out += "\\u00";
            out += hex[c >> 4];
            out += hex[c & 15];
        } else {
            out += char(c);
        }
    }
    return out + '"';
}
inline string array(const V &values) {
    string out = "[";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i) {
            out += ",";
        }
        out += quote(values[i]);
    }
    return out + "]";
}
} // namespace json

namespace json {
using Records = map<string, pair<char, V>>;
#ifdef LAB_NLOHMANN
inline Records decode(const string &text) {
    auto j = nlohmann::json::parse(text);
    check(j.is_object(), "expected JSON object");
    Records r;
    for (auto it = j.begin(); it != j.end(); ++it) {
        auto t = it.value().at("type").get<string>();
        check(t.size() == 1, "bad type");
        r.emplace(it.key(), make_pair(t[0], it.value().at("values").get<V>()));
    }
    return r;
}
inline string encode(const Records &records) {
    auto j = nlohmann::json::object();
    for (auto &[name, p] : records) {
        j[name] = {{"type", string(1, p.first)}, {"values", p.second}};
    }
    return j.dump(2) + "\n";
}
#else
// Self-contained parser for the database schema: object -> named records -> type and string array.
struct Reader {
    const string &s;
    size_t i = 0;
    void ws() {
        while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) {
            ++i;
        }
    }
    bool take(char c) {
        ws();
        if (i < s.size() && s[i] == c) {
            ++i;
            return true;
        }
        return false;
    }
    void need(char c) {
        check(take(c), "invalid JSON");
    }
    unsigned hex4() {
        unsigned v = 0;
        for (int k = 0; k < 4; ++k) {
            check(i < s.size(), "invalid unicode");
            char c = s[i++];
            int d = c >= '0' && c <= '9'   ? c - '0'
                    : c >= 'a' && c <= 'f' ? c - 'a' + 10
                    : c >= 'A' && c <= 'F' ? c - 'A' + 10
                                           : -1;
            check(d >= 0, "invalid unicode");
            v = v * 16 + d;
        }
        return v;
    }
    static void utf(string &out, unsigned u) {
        if (u < 128) {
            out += char(u);
        } else if (u < 2048) {
            out += char(192 | (u >> 6));
            out += char(128 | (u & 63));
        } else if (u < 65536) {
            out += char(224 | (u >> 12));
            out += char(128 | ((u >> 6) & 63));
            out += char(128 | (u & 63));
        } else {
            out += char(240 | (u >> 18));
            out += char(128 | ((u >> 12) & 63));
            out += char(128 | ((u >> 6) & 63));
            out += char(128 | (u & 63));
        }
    }
    string str() {
        need('"');
        string out;
        while (i < s.size()) {
            unsigned char c = s[i++];
            if (c == '"') {
                return out;
            }
            check(c >= 32, "invalid string");
            if (c != '\\') {
                out += char(c);
                continue;
            }
            check(i < s.size(), "invalid escape");
            char e = s[i++];
            switch (e) {
            case '"':
            case '\\':
            case '/':
                out += e;
                break;
            case 'b':
                out += '\b';
                break;
            case 'f':
                out += '\f';
                break;
            case 'n':
                out += '\n';
                break;
            case 'r':
                out += '\r';
                break;
            case 't':
                out += '\t';
                break;
            case 'u': {
                unsigned u = hex4();
                if (u >= 0xd800 && u <= 0xdbff) {
                    check(i + 2 <= s.size() && s.substr(i, 2) == "\\u", "invalid surrogate");
                    i += 2;
                    unsigned v = hex4();
                    check(v >= 0xdc00 && v <= 0xdfff, "invalid surrogate");
                    u = 0x10000 + ((u - 0xd800) << 10) + (v - 0xdc00);
                }
                check(!(u >= 0xd800 && u <= 0xdfff), "invalid surrogate");
                utf(out, u);
                break;
            }
            default:
                throw runtime_error("invalid escape");
            }
        }
        throw runtime_error("unterminated string");
    }
    V arr() {
        V v;
        need('[');
        if (!take(']')) {
            do {
                v.push_back(str());
            } while (take(','));
            need(']');
        }
        return v;
    }
    Records read() {
        Records r;
        need('{');
        if (!take('}')) {
            do {
                auto name = str();
                need(':');
                need('{');
                string type;
                V vals;
                bool ht = false, hv = false;
                do {
                    auto key = str();
                    need(':');
                    if (key == "type" && !ht) {
                        type = str();
                        ht = true;
                    } else if (key == "values" && !hv) {
                        vals = arr();
                        hv = true;
                    } else {
                        throw runtime_error("invalid record field");
                    }
                } while (take(','));
                need('}');
                check(ht && hv && type.size() == 1, "invalid record");
                check(r.emplace(name, make_pair(type[0], std::move(vals))).second, "duplicate name");
            } while (take(','));
            need('}');
        }
        ws();
        check(i == s.size(), "trailing JSON data");
        return r;
    }
};
inline Records decode(const string &text) {
    return Reader{text}.read();
}
inline string encode(const Records &records) {
    string out = "{\n";
    bool first = true;
    for (auto &[name, p] : records) {
        if (!first) {
            out += ",\n";
        }
        first = false;
        out += "  " + quote(name) + ": {\n    \"type\": " + quote(string(1, p.first)) +
               ",\n    \"values\": " + array(p.second) + "\n  }";
    }
    return out + "\n}\n";
}
#endif
} // namespace json
