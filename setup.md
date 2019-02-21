---
title: Setup
---

## Install MPI

Make sure you can compile your C programs using a Fortran, C or C++ compiler or a development environment. You will need to install the MPI (Message Passing Interface) library. Several implementations of MPI exist, but for example OpenMPI will work.

On Ubuntu install openmpi using
```
sudo apt install openmpi-bin openmpi-common openmpi-doc
```
{: .source .language-bash}

On Os X you can install openmpi for the command line using [homebrew](https://brew.sh). After installing homebrew, run
```
brew install open-mpi
```
{: .source .language-bash}


On Windows, you can install [Microsoft MPI](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi) for Visual studio


## Install Cmocka (for C) or FRUIT (for Fortran)

Cmocka is a unit testing framework for C. You can find it at the [Cmocka website](https://cmocka.org/).
On Ubuntu, all you need to do is install 'libcmocka-dev'.

FRUIT is a similar unit testing framework for Fortran. It can be downloaded from [sourcefourge](http://sourceforge.net/projects/fortranxunit/).

{% include links.md %}


## Installing Scalasca

Scalasca is an open source application for profiling MPI programs.
We use scalasca in one of the afternoon lessons.
If you have access to a cluster and scalasca is installed, you don't
need to install in on your laptop, but you might still consider
installing cubeGUI and cubelib.

Scalasca consists of four parts
* Scorep for compiling an instrumented binary
* Scalasca for profiling and analysing
* CubeGUI, a graphical viewer
* The cubelib library

All these packages are available as source and need to be compiled.
The process is straightforward but takes a few minutes.
They are supported on linux and OS X.
On Windows you can use the subsystem linux.

Start with Scorep:
~~~
cd scorep
./configure
make
sudo make install
cd ..
~~~

To install scalasca you need the cubelib library:
~~~
cd cubelib
./configure
make
sudo make install
cd ..
~~~

Finally install Scalasca itself:
~~~
cd scalasca
./configure
make
sudo make install
cd ..
~~~

### Installing cubeGUI

Installing cubeGUI is not necessary but it is useful.
If you have access to a cluster with scalasca installed and
with a fast internet connection,
it is possible to use X forwarding to use the GUI.



The default installation path is in /opt/cubelib/.
Add this to you PATH:

~~~
PATH=$PATH:/opt/cubelib/bin/
~~~

You also need qt5 to compile the GUI

~~~
sudo apt install qt5-default
~~~

Finally compile and install the GUI

~~~
cd cubeGUI
./configure
make
sudo make install
~~~

