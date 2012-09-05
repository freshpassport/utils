#!/usr/bin/env python
# -*- coding: utf-8 -*-

class Slash:
	def __init__(self, x, y):
		self.slash_x = x
		self.slash_y = y

class Person(object):
	def __init__(self, name, age):
		self.name = name
		self.age = age
		self.lt = ['a', 'b', 'c', 'd']
		self.slash_obj = []
		self.total = 0
	def __repr__(self):
		return 'Persion Object name: %s, age: %d' % (self.name, self.age)
	def dump_test(self):
		print 'test'

if __name__ == '__main__':
	p = Person('Peter', 20)
	print p
	print p.__dict__
