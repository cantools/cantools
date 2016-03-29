#!/usr/bin/env python

from setuptools import setup
import bitstruct

setup(name='bitstruct',
      version=bitstruct.__version__,
      description=('This module performs conversions between Python values '
                   'and C bit field structs represented as Python '
                   'byte strings.'),
      long_description=open('README.rst', 'r').read(),
      author='Erik Moqvist, Ilya Petukhov',
      author_email='erik.moqvist@gmail.com',
      license='MIT',
      classifiers=[
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 3',
      ],
      keywords=['bit field', 'bit parsing', 'bit unpack', 'bit pack'],
      url='https://github.com/eerimoq/bitstruct',
      py_modules=['bitstruct'],
      test_suite="tests")
