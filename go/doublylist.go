package main

type DoublyList struct{ List }

func newDoublyList() *DoublyList { return &DoublyList{List: List{doub: true}} }
