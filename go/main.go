package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strconv"
	"strings"
)

func check(b bool, msg string) {
	if !b {
		panic(msg)
	}
}
func index(s string) int {
	for _, c := range s {
		check(c >= '0' && c <= '9', "invalid index")
	}
	n, e := strconv.Atoi(s)
	check(e == nil && n >= 0, "invalid index")
	return n
}

func main() {
	defer func() {
		if e := recover(); e != nil {
			fmt.Fprintln(os.Stderr, "ERROR:", e)
			os.Exit(1)
		}
	}()
	file, q := "", ""
	for i := 1; i < len(os.Args); i += 2 {
		check(i+1 < len(os.Args), "missing argument")
		switch os.Args[i] {
		case "--file":
			file = os.Args[i+1]
		case "--query":
			q = os.Args[i+1]
		default:
			panic("unknown option")
		}
	}
	check(file != "" && q != "", "usage: dbms --file FILE --query 'COMMAND NAME ...'")
	db := load(file)
	name := filepath.Base(os.Args[0])
	if run(db, strings.Fields(q), name == "dbms" || name == "dbms.exe") {
		save(db, file)
	}
}
