---
title: "Profiling Parallel Applications"
teaching: 30
exercises: 30
questions:
- "It works, but what is taking time?"
objectives:
- "Use a profiler to find bottlenecks and hotspots and diagnose problems."
keypoints:
- "Use a profiler to find the most important functions and concentrate on those."
- "The profiler needs to understand MPI. Your cluster probably has one."
- "If a lot of time is spent in communication, maybe it can be rearranged."
---

## Profiling

Profilers help you find out where a program is spending its time
and pinpoint places where optimising it makes sense. Many different 
types of profiling tools exist, but for MPI application we need 
**parallel profilers**.

Some examples of parallel profilers are:
- [Scalasca](http://scalasca.org) - a free and open source parallel 
  profiler developed by three German research centers.
- [CrayPat](https://pubs.cray.com/content/S-2376/7.0.0/cray-performance-measurement-and-analysis-tools-user-guide/craypat) - performance analysis tool 
  offered by Cray for the XC platform.
- [TAU](https://www.cs.uoregon.edu/research/tau/home.php) - a portable 
  profiling and tracing toolkit for performance analysis of parallel 
  programs written in Fortran, C, C++, UPC, Java, Python.
- [etc...](https://arc.leeds.ac.uk/using-the-systems/advanced-topics/debugging-and-profiling-tools/)

In this lesson we will use a simple tool called ARM Performance
Reports which gives us an overview of how much time is spent in
compute, MPI calls and I/O. Performance Reports is part of the ARM
Forge (formerly Allinea Forge) suite of tools for parallel
applications and is developed by the semiconductor and software design
company ARM.  

The suite also comes with a debugger (ARM DDT) and a profiler (ARM
MAP). ARM MAP is a more advanced tool which allows the user to see how
much time each individual line of code takes, and why. ARM DDT
supports a wide range of parallel architectures and models, including
MPI, UPC, CUDA and OpenMP.

Version 19 and higher of ARM Forge supports Python, in addition to
Fortran and C/C++. PDC has a license for ARM MAP and ARM DDT and the
most recent versions are installed, but the installed versions of
Performance Reports only support Fortran and C/C++. To see which
versions are available, type:

~~~
$ module avail allinea
~~~
{: .source .language-bash}


### Further information

For more information on ARM Forge check the [product website](https://www.arm.com/products/development-tools/server-and-hpc/forge). Instructions for how to use ARM Forge at 
PDC can be found [on the support pages](https://www.pdc.kth.se/software/software/allinea-forge/index_general.html).


### Summary profile

It is advisable to create a short version of your program, limiting the
runtime to a few seconds or minutes.
You may be able to reduce the problem size or required precision,
as long as this does not change the algorithm itself.
We will use the Poisson solver from the previous lesson as an example.
The example program used here (and at the end of the previous section)
is
[here](../code/poisson/poisson_profiling.c){: .show-c}
[here](../code/poisson/poisson_profiling.F08){: .show-fortran}.

We first load the module for Performance Reports:
~~~
module load allinea-reports/18.1.1
~~~
{: .source .language-bash}

Assuming the final version is saved into `poisson.c` it is compiled with:
{: .show-c}
~~~
mpicc -o poisson poisson.c
~~~
{: .source .language-bash .show-c}

Assuming the final version is saved into `poisson.F08` it is compiled with:
{: .show-fortran}
~~~
mpifort -o poisson poisson.F08
~~~
{: .source .language-bash .show-fortran}

Next, we run the executable using Performance Reports 
to analyse the program execution:
~~~
perf-report mpirun -n 16 poisson
~~~
{: .source .language-bash}

This creates two files, one .txt file which can be viewed in the
terminal and one .html file which can be opened in a browser.
~~~
cat poisson_16p_1n_2019-11-26_09-37.txt
~~~
{: .source .language-bash}
~~~
Command:        mpirun -n 16 ./poisson
Resources:      1 node (24 physical, 48 logical cores per node)
Tasks:          16 processes
Machine:        tegner-login-1.pdc.kth.se
Start time:     Tue Nov 26 2019 09:37:22 (UTC+01)
Total time:     1 second
Full path:      /cfs/klemming/nobackup/k/kthw/introduction-to-mpi

Summary: poisson is MPI-bound in this configuration
Compute:                                     35.0% |===|
MPI:                                         65.0% |======|
I/O:                                          0.0% |
This application run was MPI-bound. A breakdown of this time and advice for investigating further is in the MPI section below.

...
~~~
{: .output}

The graphical output looks like this:
![A picture of the perf-report]({{ page.root }}{% link fig/perf_reports.png %})

The output shows that when run in this way, the application is 
MPI bound, i.e. most time is spent in MPI communication. This 
typically means that the application does not scale well to the 
given number of processes. Either the number of processes should 
be decreased, or the problem size increased so that more time is 
spent in the actual compute sections of the code.

>## Profile Your Poisson Code
>
> Compile, run and analyse your own MPI version of the poisson code.
> - How closely does it match the performance above? What are the main differences?
> - Try reducing the number of processes used, rerun and investigate the profile.
>   Is it still MPI-bound?
> - Increase the problem size, recompile, rerun and investigate the profile. What has 
>   changed now?
{: .challenge}


## Optimisation Workflow

The general workflow for optimising a code, wether parallel or serial
is as follows:
1. Profile.
2. Optimise.
3. Test correctness.
4. Measure efficiency.
5. Goto: 1.


> ## Iterative Improvement
>
> In the Poisson code,
> try changing the location of the calls to `MPI_Send`. How does this affect performance?
>
{: .challenge}

{% include links.md %}
