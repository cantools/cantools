#!/usr/bin/env python

from setuptools import setup
from setuptools import find_packages
import re


def find_version():
    return re.search(r"^__version__ = '(.*)'$",
                     open('cantools/version.py', 'r').read(),
                     re.MULTILINE).group(1)


setup(name='cantools',
      version=find_version(),
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
      keywords=['can', 'can bus', 'dbc', 'kcd', 'automotive'],
      url='https://github.com/eerimoq/cantools',
      packages=find_packages(exclude=['tests']),
      install_requires=[
          'bitstruct>=6.0.0',
          'python-can>=2.2.0',
          'textparser>=0.21.1',
          'diskcache'
      ],
      test_suite="tests",
      entry_points = {
          'console_scripts': ['cantools=cantools.__init__:_main']
      })
