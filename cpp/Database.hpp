#pragma once
#include "Data.hpp"
#include <map>
using DB = map<string, pair<char, unique_ptr<Data>>>;
void load(DB &db, string file);
void save(DB &db, string file);
bool run(DB &db, V args, bool reportMode);
