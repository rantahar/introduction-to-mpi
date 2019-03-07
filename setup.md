---
title: Setup
---

## Install MPI

Make sure you can compile C or Fortran programs using a compiler or a development environment. You will need an implementation of the MPI (Message Passing Interface) library. Several implementations of MPI exist, but for example OpenMPI will work on Linux and OS X and Microsoft Distribution of MPICH will work on Windows.

#### Ubuntu / Linux
Most linux distributions will have an openmpi or openmpi-dev package.
On Ubuntu, open a terminal and run
```
sudo apt install openmpi-bin openmpi-common openmpi-doc
```
{: .source .language-bash}

Check that you OpenMPI is properly installed by runnin
```
mpicc --showme:version
```
{: .source .language-bash}
The output should be similar to this:
```
mpicc: Open MPI 2.1.1 (Language: C)
```
{: .output}

#### OS X
On Os X you can install openmpi for the command line using [homebrew](https://brew.sh). After installing homebrew, open the Terminal in Applications/Utilities and run
```
brew install open-mpi
```
{: .source .language-bash}

To check the installation run
```
mpicc --showme:version
```
{: .source .language-bash}
The output should be similar to this:
```
mpicc: Open MPI 2.1.1 (Language: C)
```
{: .output}


#### Windows
On Windows, if you use Visual Studio, you can install [Microsoft MPI](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi).
The download includes two files, msmpisetup.exe and msmpisdk.msi. Download and run both installers.
Follow [these instructions](https://blogs.technet.microsoft.com/windowshpc/2015/02/02/how-to-compile-and-run-a-simple-ms-mpi-program/)
to create a project with the mpi compiler and library.

You can verify that it works by compiling any C or Fortran program using the mpicc compiler

Other option include installing OpenMPI under [Cygwin](https://www.cygwin.com/) or
using the linux subsystem on Windows 10.
Under Cygwin, install the gcc, openmpi and the openmpi-devel packages.

## Install Cmocka or FRUIT

### Cmocka
Cmocka is a unit testing framework for C. You can find it at the [Cmocka website](https://cmocka.org/).

#### Ubuntu / Linux
On Ubuntu, all you need to do is install 'libcmocka-dev'. Other distributions may have a similar package, or may require a source installation (see below).

#### OS X
Run
```
brew install clib
clib install cmocka
```
{: .source .language-bash}

#### Windows / Source Installation
In Cygwin, first install git, make and cmake using the cygwin package manager. Then run
```
git clone git://git.cryptomilk.org/projects/cmocka.git
mkdir cmocka-build
cd cmocka-build
cmake ../cmocka
make
make install
```
{: .source .language-bash}


### FRUIT
FRUIT is a similar unit testing framework for Fortran. It can be downloaded from [sourcefourge](http://sourceforge.net/projects/fortranxunit/).
For the examples in this workshop, you will need the src/fruit.f90 file from the ZIP archive.


## Installing Scalasca

Scalasca is an open source application for profiling MPI programs.
We use scalasca in one of the afternoon lessons.
Downloading and installing at least the CubeGUI is useful.
If you have access to a cluster and Scalasca is installed, you don't
need to install scalasca itself on your laptop.

Scalasca consists of four parts
* Score-P for compiling an instrumented binary
* Scalasca for profiling and analysing
* CubeGUI, a graphical viewer
* The Cubelib library, used by Scalasca and CubeGUI

CubeGUI can be used to view profiling information produced on different platforms.
Binary packages are provided for Windows and OS X on the [Scalasca Downloads website](http://www.scalasca.org/software/cube-4.x/download.html).

To install it on Linux, download the Cube Bundle on the same website.
Compile and install with
~~~
tar -xf CubeBundle-2.0.tar.gz
cd CubeBundle-2.0
./configure
make
sudo make install
cd ..
~~~
{: .source .language-bash}


You may also wish to install Scalasca itself. This allows you to run the profiler on your laptop. You can use Scalasca to profile MPI programs on your local system

Start with Scorep. Download it from [the scorep website](https://www.vi-hps.org/projects/score-p/) and run
~~~
tar -xf scorep-*.tar.gz
cd scorep
./configure
make
sudo make install
cd ..
~~~
{: .source .language-bash}


Finally install Scalasca itself. Download it from the [Scalasca Downloads page](http://www.scalasca.org/software/cube-4.x/download.html).
Install in the same way,
~~~
tar -xf scalasca-*.tar.gz
cd scalasca
./configure
make
sudo make install
cd ..
~~~
{: .source .language-bash}

