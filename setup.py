#! /usr/bin/env python

"""

Author: Sunil Thulasidasan

Setup script for SimX. For installating SimX, type
python setup.py install
or
python setup.py install --user
(this will install SimX under the user's home directory)

"""

from setuptools import setup#, find_packages, Extension
import distutils.command.build as _build
import setuptools.command.install as _install

import sys
import os
import os.path as op
import distutils.spawn as ds
import distutils.dir_util as dd

#import distutils
#import distutils.dist

#################
# CMake function
#################
#def run_cmake(cmake_args="-DSIMX_USE_PRIME=1 -DSIMX_USE_MPI=1"):
def run_cmake(use_prime=1,use_mpi=1):
    """
    Runs CMake to determine configuration for this build
    """
    if ds.find_executable('cmake') is None:
        print "CMake  is required to build SimX"
        print "Please install cmake version >= 2.6 and re-run setup"
        sys.exit(-1)
        
    print "Configuring SimX build with CMake.... "
    new_dir = op.join(op.split(__file__)[0],'build')
    dd.mkpath(new_dir)
    os.chdir(new_dir)
    # construct argument string
    cmake_args ="-DSIMX_USE_PRIME="+str(use_prime) \
        +" -DSIMX_USE_MPI="+str(use_mpi)
    try:
        ds.spawn(['cmake','../']+cmake_args.split())
    except ds.DistutilsExecError:
        print "Error while running cmake"
        print "run 'setup.py build --help' for build options"
        print "You may also try editing the settings in CMakeLists.txt file and re-running setup"
        sys.exit(-1)

        
###########################
# Custom 'install' command
###########################
class install(_install.install):
    def run(self):
        try:
            import config as C
        except ImportError:
            cwd = os.getcwd()
            run_cmake()
            os.chdir(cwd)
            import config as C
        self.distribution.ext_modules = C.extension_list
        # calling _install.install.run(self) does not fetch  required packages
        # and instead performs an old-style install. see command/install.py in
        # setuptools. So, calling do_egg_install() manually here.
        self.do_egg_install()


#########################
# Custom 'build' command
#########################
class  build(_build.build):

    user_options=_build.build.user_options + \
        [('without-ssf',
          None,
          "Uses SimEngine instead of SSF for message passing and synchronization. \
 This will only work with multithreaded MPI"),
         ('without-mpi',
          None,
          "Build without MPI support. Parallel simulations are disabled in this case. This option cannot be used together with the --without-ssf option. ")]

    def initialize_options(self):
        _build.build.initialize_options(self)
        self.without_ssf = 0
        self.without_mpi = 0
        
    def run(self):
        cwd = os.getcwd()
        # if self.without_ssf and self.without_mpi:
        #     print "--without-ssf and --without-mpi cannot be used together"
        #     print "run setup.py build --help for options to build command"
        #     sys.exit(-1)
        run_cmake(use_prime=not self.without_ssf,
                  use_mpi = not self.without_mpi)
        # if 
        # if self.without_ssf:
        #     run_cmake("-DSIMX_USE_PRIME=0")
        # else:
        #     run_cmake()
        #try:
        #    import config as C
        #    with open('build/src/minissf/ssf_config.h'): pass
        #except (ImportError, IOError): 
        # either config.py file or ssf_config.h not found. Run CMake
        os.chdir(cwd)
        import config as C
        # Now populate the extension module  attribute.
        self.distribution.ext_modules = C.extension_list
        _build.build.run(self)


###################
# setup
###################

with open('README.txt') as file:
    simx_long_desc = file.read()

setup(
    name = "simx",
    version = '0.1',
    description = 'Parallel simulation library for Python',
    requires = ["greenlet"],
    install_requires = ["greenlet"],
    include_package_data = True,
    url = 'http://github.com/sim-x/simx',
    author='Sunil Thulasidasan, Lukas Kroc and others',
    author_email = 'simx-dev@lanl.gov',
    license = "GNU LGPL",
    long_description = simx_long_desc,
    keywords = "simulation, parallel simulation, distributed simulation, \
discrete event simulation, agent simulation, simulation library",
    platforms = ['GNU/Linux','Unix','Mac OS-X'],
    classifiers = [
         'Development Status :: 4 - Beta',
         'Intended Audience :: Developers',
         'Intended Audience :: Science/Research',
         'License :: OSI Approved :: GNU Lesser General Public License v2 (LGPLv2)',
         'Operating System :: OS Independent',
         'Programming Language :: Python :: 2',
         'Programming Language :: Python :: 2.4',
         'Programming Language :: Python :: 2.5',
         'Programming Language :: Python :: 2.6',
         'Programming Language :: Python :: 2.7',
         'Topic :: Scientific/Engineering',
         'Topic :: Scientific/Engineering :: Artificial Life',
         'Topic :: Sociology',
         'Topic :: Software Development :: Libraries :: Python Modules',
         'Topic :: Software Development :: Libraries :: Application Frameworks',
         'Topic :: System :: Distributed Computing'],
    # ext_modules is not present here. This will be generated through CMake via the
    # build or install commands
    cmdclass={'install':install,'build': build},
    zip_safe=False,
    packages = ['simx'],
    )
