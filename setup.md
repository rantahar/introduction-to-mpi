---
title: Setup
---

## Compilers and MPI

These instructions are based on installing compilers and MPI via the
conda package manager, as it provides a convenient way to install
binary packages.

The instructions also focus on installation on MacOS or Linux
computers. Installing compilers and MPI natively on Windows is
possible through [Cygwin](https://www.cygwin.com/) and the 
Microsoft Distribution of MPICH, but we recommend that you install the Windows
Subsystem for Linux (WSL), which is available for Windows 10 and later
versions. Instructions can be found [here](https://docs.microsoft.com/en-us/windows/wsl/install-win10) 
(WSL is also a good way to log in to PDC, see the [PDC support pages](https://www.pdc.kth.se/support/documents/login/windows_login.html#wsl-approach))

### Installing conda

Begin by installing Miniconda:

1. Download the 64-bit installer from [here](https://docs.conda.io/en/latest/miniconda.html) for your operating system
   - for MacOS, choose the bash installer
   - on Windows, open a Linux-WSL terminal and type: `wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh`. 
     If wget is not a recognised command, type `sudo apt-get install wget` 
     and provide the password you chose when installing WSL.
2. In a terminal, run the installer with `bash Miniconda3-latest-<operating-system>-x86_64.sh` (replace with correct name of installer)
3. Agree to the terms of conditions, specify the installation directory (the default is usually fine), and answer "yes" to the questions "Do you wish the installer to initialize Miniconda3 by running conda init?"

You now have miniconda and conda installed. Make sure that it works by
typing `which conda` and see that it points to where you installed
miniconda (you may have to open a new terminal first).

We recommend that you create an isolated conda environment (this is good practice in software development):
~~~
conda create --name mpi-intro python=3.7
conda activate mpi-intro
~~~
{: .source .language-bash}

### Python 

If you want to use Python for the exercises, you will need to install mpi4py. mpi4py can be installed either using pip or conda, but with pip you will need to install MPI yourself first (e.g. OpenMPI or MPICH), while conda will install its own MPI binaries (mpich). If you don’t already have MPI installed on your laptop, it will be easiest to use conda:
~~~
(mpi-intro) $ conda install mpi4py
~~~
{: .source .language-bash}

### C/C++ and Fortran

If you want to use C, C++ or Fortran for the exercises, you will need to install compilers and MPI libraries. This can also be done using conda:
~~~
(mpi-intro) $ conda install compilers
(mpi-intro) $ conda install mpich
~~~
{: .source .language-bash}


## ARM Forge

The ARM Forge tools (Performance Reports, MAP and DDT) are installed
on PDC clusters. These are graphical applications, and running them
over an ssh connection can become sluggish or unstable.  We therefore
recommend to install the ARM Forge Remote Client, which runs on your
local computer and can be used to connect to running processes on the
cluster.

Begin by downloading the [Remote Client](https://developer.arm.com/tools-and-software/server-and-hpc/arm-architecture-tools/downloads/download-arm-forge), 
and installing it.
Next you need to set up the connection to PDC:

- Open up the ARM Forge Client
- Click "Remote Launch", and select "Configure"
- Click "Add", and for "hostname" write: <username>@tegner.pdc.kth.se. 
  You can also give an optional Connection name.
- For "Remote installation directory", enter 
  `/pdc/vol/allinea-forge/19.1.3/amd64_co7/`
- Click on "Test Remote Launch" to see if the Remote Client GUI 
  can successfully connect to Tegner.

If connecting fails, you may need to replace the default ssh used by
Remote Client. First create the directory `~/.allinea`. In this
directory create a file called `remote-exec`. In this file, write
~~~
#!/bin/sh /correct/path/to/ssh [correct flags] $*
~~~
{: .source .language-bash}

- If you are on OSX with an ssh installed via MacPorts, 
  the correct ssh would be `/opt/local/bin/ssh`
- If you have not configured your `~/.ssh/config` file, you will need 
  to add the flags `GSSAPIDelegateCredentials=yes -o GSSAPIKeyExchange=yes -o GSSAPIAuthentication=yes`

