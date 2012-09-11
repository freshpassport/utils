#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json

def read_json():
	#s = '{"udv_name":"udv1","udv_dev":"/dev/sde1"}'
	s = '{"status":true,"udv_name":"udv2","udv_dev":"/dev/sdg2"}'
	udv_info = json.loads(s)
	print len(udv_info)
	print 'status: ', udv_info['status']
	print 'udv_name: ', udv_info['udv_name']
	print 'udv_dev: ', udv_info['udv_dev']
	return

if __name__ == '__main__':
	read_json()
