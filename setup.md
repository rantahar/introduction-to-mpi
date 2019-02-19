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
