---
title: Setup
---

## Install MPI

Make sure you can compile C or Fortran programs using a compiler or a development environment.
You will need an implementation of the MPI (Message Passing Interface) library. Several implementations
of MPI exist, but for example Open MPI will work on Linux and macOS, and the Microsoft Distribution of
MPICH will work on Windows.

#### Ubuntu / Linux
Most Linux distributions will have an `openmpi` or `openmpi-dev` package.
On Ubuntu, open a terminal and run
```
sudo apt install openmpi-bin openmpi-common openmpi-doc
```
{: .source .language-bash}

Check that you Open MPI is properly installed by running
```
mpicc --showme:version
```
{: .source .language-bash}
The output should be similar to this:
```
mpicc: Open MPI 2.1.1 (Language: C)
```
{: .output}

#### macOS
On macOS you can install Open MPI for the command line using [homebrew](https://brew.sh). 
After installing Homebrew, open the Terminal in Applications/Utilities and run
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
On Windows, if you use Visual Studio, you can install
[Microsoft MPI](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi).
The download includes two files, `msmpisetup.exe` and `msmpisdk.msi`. Download and run both installers.
Follow [these instructions](https://blogs.technet.microsoft.com/windowshpc/2015/02/02/how-to-compile-and-run-a-simple-ms-mpi-program/)
to create a project with the MPI compiler and library.

You can verify that it works by compiling any C or Fortran program using the `mpicc` compiler

Other option include installing Open MPI under [Cygwin](https://www.cygwin.com/) or
using the Windows Subsystem for Linux on Windows 10.
Under Cygwin, install the `gcc`, `openmpi` and the `openmpi-devel` packages.


## Installing Scalasca

Scalasca is an open source application for profiling MPI programs.
We use Scalasca in one of the afternoon lessons.
Downloading and installing at least the CubeGUI is useful.
If you have access to a cluster and Scalasca is installed, you don't
need to install Scalasca itself on your laptop.

Scalasca consists of four parts
* Score-P for compiling an instrumented binary
* Scalasca for profiling and analysing
* CubeGUI, a graphical viewer
* The Cubelib library, used by Scalasca and CubeGUI

CubeGUI can be used to view profiling information produced on different platforms.
Binary packages are provided for Windows and macOS on the [Scalasca Downloads website](http://www.scalasca.org/software/cube-4.x/download.html).

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

Start with Score-P. Download it from [the scorep website](https://www.vi-hps.org/projects/score-p/) and run
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

