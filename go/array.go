package main

type Array struct {
	a []string
	n int
}

func (m *Array) insert(i int, s string) {
	check(i <= m.n, "index out of range")
	if m.n == len(m.a) {
		k := len(m.a) * 2
		if k == 0 {
			k = 4
		}
		b := make([]string, k)
		copy(b, m.a)
		m.a = b
	}
	for j := m.n; j > i; j-- {
		m.a[j] = m.a[j-1]
	}
	m.a[i] = s
	m.n++
}

func (m *Array) append(s string) {
	m.insert(m.n, s)
}

func (m *Array) at(i int) string {
	check(i < m.n, "index out of range")
	return m.a[i]
}

func (m *Array) erase(i int) string {
	s := m.at(i)
	for j := i + 1; j < m.n; j++ {
		m.a[j-1] = m.a[j]
	}
	m.n--
	m.a[m.n] = ""
	return s
}

func (m *Array) values() []string {
	return m.a[:m.n]
}
