#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sqlite3
import os
import sys

reload(sys)
sys.setdefaultencoding('utf-8')

def test_create():
	conn = sqlite3.connect('%s/example.db' % os.getcwd())
	c = conn.cursor()

	# create table
	c.execute('''CREATE TABLE stocks
		(date text, trans text, symbol text)''')
	# insert a row
	c.execute("INSERT INTO stocks VALUES ('2012-10-23', 'BUY', 'RHAT')")
	conn.commit()
	conn.close()

def test_list():
	conn = sqlite3.connect('%s/example.db' % os.getcwd())
	c = conn.cursor()
	t = ('RHAT',)
	c.execute('SELECT * FROM stocks WHERE symbol=?', t)
	x = c.fetchone()
	for a in x:
		print a
	conn.commit()
	conn.close()

def test_main():
	#test_create()
	test_list()

if __name__ == '__main__':
	test_main()
