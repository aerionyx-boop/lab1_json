package main

type Stack struct{ List }

func newStack() *Stack {
	return &Stack{List: List{doub: true}}
}

func (s *Stack) push(v string) {
	s.append(v)
}

func (s *Stack) peek() string {
	check(s.tail != nil, "empty container")
	return s.tail.s
}

func (s *Stack) pop() string {
	s.peek()
	return s.eraseAfter(s.tail.prev)
}
