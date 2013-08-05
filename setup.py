#! /usr/bin/env python

"""

Setup script for SimX

"""

from setuptools import setup, find_packages, Extension

setup(
    name = "simx",
    version = '1.0',
    description = 'Parallel simulation library for Python',
    install_requires = ["greenlet"],
    include_package_data = True,
    url = 'http://github.com/thulas/simx',
    author = 'Sunil Thulasidasan, Lukas Kroc',
    maintainer='Sunil Thulasidasan',
    maintainer_email = 'sunil@lanl.gov',
    license = "GNU LGPL",
    long_description = 
    "\nSimX is a library for developing parallel, distributed memory\n \
       simulations in Python. SimX is written in C++ and Python.",
    platforms = ['GNU/Linux','Unix','Mac OS-X'],
    classifiers = [
         'Development Status :: Beta',
         'Intended Audience :: Developers',
         'Intended Audience :: Science/Research',
         'License :: OSI Approved :: GNU Lesser General Public License (LGPLv2)',
         'Operating System :: OS Independent',
         'Programming Language :: Python :: 2',
         'Programming Language :: Python :: 2.4',
         'Programming Language :: Python :: 2.5',
         'Programming Language :: Python :: 2.6',
         'Programming Language :: Python :: 2.7',
         'Topic :: Scientific/Engineering',
         'Topic :: Scientific/Engineering :: Artifical Life',
         'Topic :: Sociology',
         'Topic :: Software Development :: Libraries :: Python Modules',
         'Topic :: Software Development :: Libraries :: Application Frameworks',
         'Topic :: System :: Distributed Computing'],
    zip_safe=False,
    packages = ['simx'],
    package_data = {'simx':['core.so']}
    )
