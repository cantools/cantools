#!/usr/bin/env python

from setuptools import setup

setup(name='cantools',
      version='1.0.0',
      description='CAN BUS tools.',
      long_description=open('README.rst', 'r').read(),
      author='Erik Moqvist',
      author_email='erik.moqvist@gmail.com',
      license='MIT',
      classifiers=[
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 3',
      ],
      keywords=['CAN BUS', 'candb', 'cantools'],
      url='https://github.com/eerimoq/cantools',
      packages=['cantools'],
      install_requires=['bitstruct>=0.7.1',
                        'pyparsing>=2.0.3'],
      test_suite="tests")
