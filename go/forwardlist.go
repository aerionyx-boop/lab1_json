package main

type Node struct {
	s          string
	next, prev *Node
}
type List struct {
	head, tail *Node
	doub       bool
	n          int
}

func (l *List) find(s string) *Node {
	for p := l.head; p != nil; p = p.next {
		if p.s == s {
			return p
		}
	}
	return nil
}

func (l *List) prev(p *Node) *Node {
	if l.doub {
		return p.prev
	}
	if p == l.head {
		return nil
	}
	for q := l.head; q != nil; q = q.next {
		if q.next == p {
			return q
		}
	}
	return nil
}

func (l *List) at(i int) *Node {
	check(i < l.n, "index out of range")
	p := l.head
	for ; i > 0; i-- {
		p = p.next
	}
	return p
}

func (l *List) after(p *Node, s string) {
	q := &Node{s: s}
	if p == nil {
		q.next = l.head
		l.head = q
	} else {
		q.next = p.next
		p.next = q
	}
	if l.doub {
		q.prev = p
		if q.next != nil {
			q.next.prev = q
		}
	}
	if q.next == nil {
		l.tail = q
	}
	l.n++
}

func (l *List) append(s string) {
	l.after(l.tail, s)
}

func (l *List) eraseAfter(p *Node) string {
	q := l.head
	if p != nil {
		q = p.next
	}
	check(q != nil, "no element")
	if p == nil {
		l.head = q.next
	} else {
		p.next = q.next
	}
	if l.doub && q.next != nil {
		q.next.prev = p
	}
	if q == l.tail {
		l.tail = p
	}
	l.n--
	return q.s
}

func (l *List) values() []string {
	v := make([]string, 0, l.n)
	for p := l.head; p != nil; p = p.next {
		v = append(v, p.s)
	}
	return v
}

func (l *List) reverse() []string {
	if l.doub {
		v := make([]string, 0, l.n)
		for p := l.tail; p != nil; p = p.prev {
			v = append(v, p.s)
		}
		return v
	}
	v := l.values()
	for i := 0; i < len(v)/2; i++ {
		v[i], v[len(v)-1-i] = v[len(v)-1-i], v[i]
	}
	return v
}

type ForwardList struct{ List }

func newForwardList() *ForwardList {
	return &ForwardList{}
}
