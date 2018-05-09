#!/usr/bin/env python

from setuptools import setup, find_packages

with open(path.join('cantools', '__init__.py'), 'r') as f:
    for line in f:
        if line.startswith('__version__'):
            version = line.split('=')[-1].strip().strip("'")
            break

setup(name='cantools',
      version=version,
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
          'bitstruct>=3.7.0',
          'pyparsing>=2.0.3',
          'python-can>=2.1.0'
      ],
      test_suite="tests",
      entry_points = {
          'console_scripts': ['cantools=cantools.__init__:_main']
      })
