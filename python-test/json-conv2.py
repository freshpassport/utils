#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
JSONEncoder类负责编码，主要是通过其default函数进行转化，我们可以override该方法。同理对于JSONDecoder。
"""

import Person
import json

p = Person.Person('Peter', 22)

x = Person.Slash(10, 12)
p.slash_obj.append(x)

x = Person.Slash(5, 8)
p.slash_obj.append(x)

x = Person.Slash(15, 28)
p.slash_obj.append(x)

p.total = len(p.slash_obj)

class MyEncoder(json.JSONEncoder):
	def default(self, obj):
		# convert object to a dict
		d = {}
		#d['__class__'] = obj.__class__.__name__
		#d['__module__'] = obj.__module__
		d.update(obj.__dict__)
		return d

if __name__ == '__main__':
	d = MyEncoder().encode(p)
	print d
	print type(d)
