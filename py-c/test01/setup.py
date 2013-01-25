#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.core import setup, Extension

setup(name = 'warp_my_c_function', version = '1.0', ext_modules = [Extension('warp', ['warp.c'])])

