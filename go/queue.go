package main

type Queue struct{ List }

func newQueue() *Queue {
	return &Queue{}
}

func (q *Queue) push(v string) {
	q.append(v)
}

func (q *Queue) peek() string {
	check(q.head != nil, "empty container")
	return q.head.s
}

func (q *Queue) pop() string {
	q.peek()
	return q.eraseAfter(nil)
}
