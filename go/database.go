package main

import (
	"encoding/json"
	"fmt"
	"os"
	"strings"
)

type Data interface {
	append(string)
	values() []string
}
type Entry struct {
	t byte
	d Data
}
type DB map[string]Entry

func create(t byte) Data {
	switch t {
	case 'M':
		return &Array{}
	case 'F':
		return newForwardList()
	case 'L':
		return newDoublyList()
	case 'S':
		return newStack()
	case 'Q':
		return newQueue()
	default:
		panic("unknown type (trees are implemented in C++)")
	}
}

type Record struct {
	Type   string   `json:"type"`
	Values []string `json:"values"`
}

func load(file string) DB {
	db := DB{}
	raw, e := os.ReadFile(file)
	if os.IsNotExist(e) {
		return db
	}
	check(e == nil, "cannot read file")
	var records map[string]Record
	check(json.Unmarshal(raw, &records) == nil && records != nil, "invalid JSON")
	for name, p := range records {
		check(len(p.Type) == 1 && p.Values != nil, "invalid record")
		t := p.Type[0]
		d := create(t)
		for _, v := range p.Values {
			d.append(v)
		}
		db[name] = Entry{t, d}
	}
	return db
}
func save(db DB, file string) {
	records := map[string]Record{}
	for name, p := range db {
		v := p.d.values()
		if v == nil {
			v = []string{}
		}
		records[name] = Record{string(p.t), v}
	}
	raw, e := json.MarshalIndent(records, "", "  ")
	check(e == nil, "cannot encode JSON")
	tmp := file + ".tmp"
	check(os.WriteFile(tmp, append(raw, '\n'), 0644) == nil, "cannot write file")
	check(os.Rename(tmp, file) == nil, "rename error")
}
func printValues(v []string) { fmt.Println(strings.Join(v, " ")) }
func run(db DB, a []string, reportMode bool) bool {
	check(len(a) >= 2, "command and name required")
	c, name := a[0], a[1]
	ar := func(n int) { check(len(a) == n+2, "wrong argument count") }
	val := func(i int) string { return a[i+2] }
	p, ok := db[name]
	original := c
	if c == "MUPDATE" {
		c = "MSET"
	}
	if c == "MDEL_AT" {
		c = "MDEL"
	}
	if c[0] == 'F' || c[0] == 'L' {
		aliases := map[string]string{
			"PUSH_FRONT": "PUSHHEAD", "PUSH_BACK": "PUSHTAIL",
			"POP_FRONT": "DELHEAD", "POP_BACK": "DELTAIL",
			"INSERT_BEFORE_VAL": "PUSHBEFORE", "INSERT_AFTER_VAL": "PUSHAFTER",
			"DEL_VALUE": "DEL", "DEL_BEFORE": "DELBEFORE", "DEL_AFTER": "DELAFTER",
		}
		if op, found := aliases[c[1:]]; found {
			c = c[:1] + op
		}
	}
	printRemoved := func(value string) {
		if !reportMode || original == c {
			fmt.Println(value)
		}
	}

	if c == "PRINTJSON" {
		ar(0)
		check(ok, "unknown container")
		v := p.d.values()
		if v == nil {
			v = []string{}
		}
		raw, e := json.Marshal(v)
		check(e == nil, "cannot encode JSON")
		fmt.Println(string(raw))
		return false
	}
	if c == "PRINT" {
		ar(0)
		check(ok, "unknown container")
		values := p.d.values()
		if !reportMode {
			printValues(values)
			return false
		}
		fmt.Print(name, ": ")
		switch p.t {
		case 'M':
			fmt.Print("[ ")
			for _, value := range values {
				fmt.Print(value, " ")
			}
			fmt.Println("]")
		case 'F', 'L':
			arrow := " -> "
			if p.t == 'L' {
				arrow = " <-> "
			}
			for _, value := range values {
				fmt.Print(value, arrow)
			}
			fmt.Println("NULL")
		case 'S':
			printValues(p.d.(*Stack).reverse())
		default:
			printValues(values)
		}
		return false
	}
	t := c[0]
	op := c[1:]
	isCreate := op == "CREATE"
	if !ok {
		check(isCreate || op == "PUSH" || ((t == 'F' || t == 'L') && (op == "PUSHHEAD" || op == "PUSHTAIL")), "unknown container")
		p = Entry{t, create(t)}
		db[name] = p
	}
	check(p.t == t, "wrong container type")
	if isCreate {
		ar(0)
		check(!ok, "already exists")
		return true
	}
	if t == 'M' {
		m := p.d.(*Array)
		switch op {
		case "PUSH":
			ar(1)
			m.append(val(0))
		case "INSERT":
			ar(2)
			m.insert(index(val(0)), val(1))
		case "DEL":
			ar(1)
			printRemoved(m.erase(index(val(0))))
		case "DEL_VALUE":
			ar(1)
			for i, value := range m.values() {
				if value == val(0) {
					m.erase(i)
					return true
				}
			}
			return false
		case "SET":
			ar(2)
			i := index(val(0))
			m.at(i)
			m.a[i] = val(1)
		case "GET":
			ar(1)
			fmt.Println(m.at(index(val(0))))
			return false
		case "LEN":
			ar(0)
			fmt.Println(m.n)
			return false
		default:
			panic("unknown command")
		}
		return true
	}
	if t == 'S' || t == 'Q' {
		var x interface {
			push(string)
			pop() string
			peek() string
		}
		if t == 'S' {
			x = p.d.(*Stack)
		} else {
			x = p.d.(*Queue)
		}
		switch op {
		case "PUSH":
			ar(1)
			x.push(val(0))
			if t == 'S' && !reportMode {
				fmt.Println(val(0))
			}
		case "POP":
			ar(0)
			fmt.Println(x.pop())
		case "PEEK":
			ar(0)
			fmt.Println(x.peek())
			return false
		default:
			panic("unknown command")
		}
		return true
	}
	var l *List
	if t == 'F' {
		l = &p.d.(*ForwardList).List
	} else {
		l = &p.d.(*DoublyList).List
	}
	target := func() *Node { q := l.find(val(0)); check(q != nil, "value not found"); return q }
	switch op {
	case "INSERT_BEFORE_AT", "INSERT_AFTER_AT":
		ar(2)
		node := l.at(index(val(0)))
		if op == "INSERT_BEFORE_AT" {
			node = l.prev(node)
		}
		l.after(node, val(1))
	case "DEL_AT":
		ar(1)
		l.eraseAfter(l.prev(l.at(index(val(0)))))
	case "DEL_ALL":
		ar(1)
		value := val(0)
		var previous *Node
		for node := l.head; node != nil; node = node.next {
			if node.s == value {
				l.eraseAfter(previous)
			} else {
				previous = node
			}
		}
	case "PUSH", "PUSHTAIL":
		ar(1)
		l.append(val(0))
	case "PUSHHEAD":
		ar(1)
		l.after(nil, val(0))
	case "PUSHBEFORE", "PUSHAFTER":
		ar(2)
		q := target()
		if op == "PUSHBEFORE" {
			q = l.prev(q)
		}
		l.after(q, val(1))
	case "DELHEAD", "DELTAIL":
		ar(0)
		check(l.head != nil, "empty list")
		var q *Node
		if op == "DELTAIL" {
			q = l.prev(l.tail)
		}
		printRemoved(l.eraseAfter(q))
	case "DEL", "DELBEFORE", "DELAFTER":
		ar(1)
		q := l.find(val(0))
		if q == nil && original != c && op == "DEL" {
			return false
		}
		check(q != nil, "value not found")
		if op == "DELBEFORE" {
			q = l.prev(q)
			check(q != nil, "no predecessor")
		}
		if op != "DELAFTER" {
			q = l.prev(q)
		}
		printRemoved(l.eraseAfter(q))
	case "GET":
		ar(1)
		if reportMode {
			found := "FALSE"
			if l.find(val(0)) != nil {
				found = "TRUE"
			}
			fmt.Println(found)
		} else {
			fmt.Println(l.at(index(val(0))).s)
		}
		return false
	case "FIND":
		ar(1)
		i := 0
		q := l.head
		for q != nil && q.s != val(0) {
			q = q.next
			i++
		}
		if q == nil {
			i = -1
		}
		fmt.Println(i)
		return false
	case "READ", "REVERSE":
		ar(0)
		if op == "READ" {
			printValues(l.values())
		} else {
			printValues(l.reverse())
		}
		return false
	default:
		panic("unknown command")
	}
	return true
}
