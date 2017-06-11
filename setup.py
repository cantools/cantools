#!/usr/bin/env python

from setuptools import setup
import cantools

setup(name='cantools',
      version=cantools.__version__,
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
      keywords=['can', 'can bus', 'dbc'],
      url='https://github.com/eerimoq/cantools',
      packages=['cantools'],
      install_requires=['bitstruct>=0.7.1',
                        'pyparsing>=2.0.3'],
      test_suite="tests",
      entry_points = {
          'console_scripts': ['cantools=cantools.__init__:main']
      })
