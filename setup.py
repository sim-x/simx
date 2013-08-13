#! /usr/bin/env python

"""

Setup script for SimX

For installation, type

python setup.py install

or

python setup.py install --user
(this will install SimX under the user's home directory)

"""

#Parts of the setup script are based on the setup.py of PyOpenCV

from setuptools import setup, find_packages, Extension

import sys
import os
import os.path as op
import distutils.spawn as ds
import distutils.dir_util as dd

if ds.find_executable('cmake') is None:
    print "CMake  is required to build SimX"
    print "Please install cmake version >= 2.6 and re-run setup"
    sys.exit(-1)

print "Building SimX core module.... "
cwd = os.getcwd()
new_dir = op.join(op.split(__file__)[0],'build')
dd.mkpath(new_dir)
os.chdir(new_dir)

try:
    ds.spawn(['cmake','..'])
except ds.DistutilsExecError:
    print "Error while running cmake"
    print "Try editing the settings in CMakeLists.txt file and re-running setup"
    os.chdir(cwd)
    sys.exit(-1)

try:
    ds.spawn(['make'])
except ds.DistutilsExecError:
    print "Error while building SimX"
    os.chdir(cwd)
    sys.exit(-1)

os.chdir(cwd)

setup(
    name = "simx",
    version = '1.0',
    description = 'Parallel simulation library for Python',
    install_requires = ["greenlet"],
    include_package_data = True,
    url = 'http://github.com/thulas/simx',
    author='Sunil Thulasidasan, Lukas Kroc and others',
    maintainer_email = 'simx-dev@lanl.gov',
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
