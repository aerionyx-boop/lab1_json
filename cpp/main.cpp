#include "Database.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>
int main(int argc,char**argv){try{string file,q;for(int i=1;i<argc;i++){string k=argv[i];check(i+1<argc,"missing argument");if(k=="--file")file=argv[++i];else if(k=="--query")q=argv[++i];else throw runtime_error("unknown option");}check(!file.empty()&&!q.empty(),"usage: dbms --file FILE --query 'COMMAND NAME ...'");istringstream in(q);V a;string s;while(in>>s)a.push_back(s);DB db;load(db,file);if(run(db,a,std::filesystem::path(argv[0]).stem()=="dbms"))save(db,file);return 0;}catch(exception&e){cerr<<"ERROR: "<<e.what()<<'\n';return 1;}}

