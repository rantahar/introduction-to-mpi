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

Profilers help you find out where the program is spending its time
and pinpoint places where optimising it makes sense.
In this lesson we will use Scalasca, but many other profilers exist.
Scalasca is specifically an MPI profiler; it will give you
information on communication efficiency and bottlenecks.

Scalasca is an open source application developed by three German research centers. For more information check the [project website](http://scalasca.org).

It is used together with the [Score-P](https://www.vi-hps.org/projects/score-p/) utility, the Cube library
and the Cube viewer.
Since it is open source, you can install it in your home directory on any HPC cluster 
that does not already have it.
The process is as described in [setup](setup), but you need to add 
`--prefix=$HOME/scalasca/` to the `./configure` command to install in
your home folder.

### Summary profile

It is advisable to create a short version of you program, limiting the
runtime to a few minutes.
You may be able to reduce the problem size or required precision,
as long as this does not change the algorithm itself.
We will use the Poisson solver from the previous lesson as an example.
The example program used here (and at the end of the previous section)
is
[here](../code/poisson/poisson_profiling.c){: .show-c} 
[here](../code/poisson/poisson_profiling.f08){: .show-fortran}.

Later we will see how we can limit the scope of the profiler, but first
we need to run a summary of the whole program.
Start by recompiling the application, replacing `mpicc` with the version
provided by the Score-P utility.

Assuming the final version is saved into `poisson.c` it is compiled with
{: .show-c}
~~~
scorep mpicc -o poisson poisson.c
~~~
{: .source .language-bash .show-c}

Assuming the final version is saved into `poisson.f08` it is compiled with
{: .show-fortran}
~~~
scorep mpifort -o poisson poisson.f08
~~~
{: .source .language-bash .show-fortran}

This will produce an instrumented binary.
When you execute, it will track function calls and the time spent in each
function.

Next, run the new executable using Scalasca to analyse the data gathered
~~~
scalasca -analyze mpirun -n 2 poisson
~~~
{: .source .language-bash}

Finally, you can examine the results in the GUI
~~~
scalasca -examine scorep_poisson_2_sum
~~~
{: .source .language-bash}

This will open the cubeGUI.
If you are running the GUI on a cluster over a normal internet connection,
it may be slow and unresponsive.
It's usually faster to copy the data folder (here `scorep_poisson_2_sum`)
to your computer and run the GUI there.

>## Profile Your Poisson Code
>
> Compile, run and analyse your mpi version of the poisson code.
> How closely does it match the performance below?
> What are the main differences?
>
{: .challenge}

![A picture of the cube GUI]({{ page.root }}{% link fig/cube.png %})

The GUI has three panels: a metric view, a function view and a system view.
In the metric view you can pick a metric to be displayed.
You can dig into the details of some of the metrics in a tree view.
The second panels shows functions either as a call tree, displaying
which functions have called which, or as a flat view.
It also displays the metric you have chosen on a function level.
The system view displays data on the chosen function on a deeper level.

Right clicking on the view and choosing "Info" replaces the system view with an info view.
This gives information on the chosen metric.

If you are running on a cluster you may wish to use the command line instead.
To supress the GUI, use the `-s` parameter.
~~~
scalasca -examine -s scorep_poisson_2_sum
cat scorep_poisson_2_sum/scorep.score
~~~
{: .source .language-bash}
The command line version will produce output that looks something like this:
~~~
Estimated aggregate size of event trace:                   608200 bytes
Estimated requirements for largest trace buffer (max_tbc): 304100 bytes
(hint: When tracing set SCOREP_TOTAL_MEMORY > max_tbc to avoid intermediate flushes
 or reduce requirements using file listing names of USR regions to be filtered.)

flt type         max_tbc         time      % region
     ALL          304100        28.53  100.0 ALL
     MPI          264080         0.96    3.4 MPI
     COM           40020        27.57   96.6 COM

     MPI          100000         0.83    2.9 MPI_Allreduce
     MPI           82000         0.06    0.2 MPI_Recv
     MPI           82000         0.01    0.0 MPI_Send
     COM           40000        27.56   96.6 poisson_step
     COM              20         0.01    0.0 main
     MPI              20         0.06    0.2 MPI_Init
     MPI              20         0.00    0.0 MPI_Finalize
     MPI              20         0.00    0.0 MPI_Comm_size
     MPI              20         0.00    0.0 MPI_Comm_rank
~~~
{: .output}

Either way we find that in our case, the program is waiting for MPI
communication for 36% of the total runtime.
This is not completely out of the ordinary, but we could do better.
53% of the time is spent in `poisson_step`.
This is not counting the time spent in functions called by poisson step.
This essentially means that 53% of the time is spent in actual computation.

If you are working on a terminal and prefer not to use the GUI, Cube
can print most of the information for you
For example the following will print out the time spent in each function:
~~~
cube_stat -p scorep_poisson_2_sum/summary.cubex
~~~
{: .source .language-bash}
This outputs
~~~
Routine                  time
INCL(main)          28.531245
  EXCL(main)         0.013468
  MPI_Init           0.055302
  MPI_Comm_rank      0.000004
  MPI_Comm_size      0.000002
  poisson_step      28.460277
  MPI_Finalize       0.002192
~~~
{: .output}

The INCL(main) includes function calls inside the main function, while
EXCL(main) counts only the time spent in the main function itself.

This tells us clearly that poisson_step is the most important function
to optimise.

You can also print a call tree using
~~~
cube_calltree -m time scorep_poisson_2_sum/summary.cubex
~~~
{: .source .language-bash}
This will print a tree view of function calls with timings:
~~~
Reading scorep_poisson_2_sum/summary.cubex... done.
0.013468        main
0.0553023         + MPI_Init
3.848e-06         + MPI_Comm_rank
1.858e-06         + MPI_Comm_size
27.5588           + poisson_step
0.832237          |   + MPI_Allreduce
0.0568715         |   + MPI_Recv
0.0123662         |   + MPI_Send
0.00219179        + MPI_Finalize
~~~
{: .output}

The numbers on the left give the amount of time spent in each function.

> ## Options
>
> Try adding the `-i` flag to `cube_calltree`.
> What does this do?
>
>> ## Solution
>> The percentages have changed to inclusive ones.
>> They now report the whole time spent in the function,
>> including in subfunction calls.
>{: .solution}
>
{: .challenge}

>## Note on the Windows Subsystem for Linux
>
> Unfortunately the kernel interface provided by the Windows Subsystem for Linux 
> does not support hardware level profiling.
> If you are running inside the Subsystem you have significantly less information.
> In a moment we will see ways of getting more information even in this case,
> but the best option is to run the profiler on an HPC cluster and examine
> the results on your personal computer.
>
{: .callout}

### Filtering

You can probably see that in a larger program the call tree would be large
and hard to manage.
The graphical interface makes this a bit easier, but the real solution is filtering.

Filtering can be used to turn off measurements in less important parts of
the code or to restrict measurements to the most interesting parts
of the code.
The filter needs to be saved as a file.
The following will filter out everything except `poisson_step`, effectively
restricting the measurements to this function.
~~~
SCOREP_REGION_NAMES_BEGIN
  EXCLUDE
    *
  INCLUDE
    poisson_step
SCOREP_REGION_NAMES_END
~~~
{: .source}

Save this to a file called `poisson.filter` and run
~~~
scalasca -examine -s -f poisson.filter scorep_poisson_2_sum
cat scorep_poisson_2_sum/scorep.score_poisson.filter
~~~
{: .source .language-bash}

The output is similar to before, but it now includes a filter column
~~~
Estimated aggregate size of event trace:                   813kB
Estimated requirements for largest trace buffer (max_buf): 407kB
Estimated memory requirements (SCOREP_TOTAL_MEMORY):       4097kB
(hint: When tracing set SCOREP_TOTAL_MEMORY=4097kB to avoid intermediate flushes
 or reduce requirements using USR regions filters.)

flt     type max_buf[B] visits time[s] time[%] time/visit[us]  region
 -       ALL    416,120 16,010   28.53   100.0        1782.09  ALL
 -       MPI    368,096 12,008    0.96     3.4          79.86  MPI
 -       COM     48,024  4,002   27.57    96.6        6889.62  COM

 *       ALL    416,096 16,008   28.52   100.0        1781.47  ALL-FLT
 -       MPI    368,096 12,008    0.96     3.4          79.86  MPI-FLT
 *       COM     48,000  4,000   27.56    96.6        6889.70  COM-FLT
 +       FLT         24      2    0.01     0.0        6734.00  FLT

 -       MPI    132,000  4,000    0.83     2.9         208.06  MPI_Allreduce
 -       MPI    118,000  4,000    0.06     0.2          14.22  MPI_Recv
 -       MPI    118,000  4,000    0.01     0.0           3.09  MPI_Send
 -       COM     48,000  4,000   27.56    96.6        6889.70  poisson_step
 +       COM         24      2    0.01     0.0        6734.00  main
 -       MPI         24      2    0.06     0.2       27651.13  MPI_Init
 -       MPI         24      2    0.00     0.0        1095.89  MPI_Finalize
 -       MPI         24      2    0.00     0.0           0.93  MPI_Comm_size
 -       MPI         24      2    0.00     0.0           1.92  MPI_Comm_rank

~~~
{: .output}
The `+` and `-` sign denote that are included and filtered out.
The `*` character denotes a region that is partially filtered.

Once you are happy with the filter, it can then be passed to Score-P with the
`SCOREP_FILTERING_FILE` environment variable
~~~
export SCOREP_FILTERING_FILE=poisson.filter
scorep mpicc -o poisson poisson_main_mpi.c poisson_step_mpi.c
~~~
{: .source .language-bash .show-c}
~~~
export SCOREP_FILTERING_FILE=poisson.filter
scorep mpifort -o poisson poisson_main_mpi.f90 poisson_step_mpi.f90
~~~
{: .source .language-bash .show-fortran}
Score-P will only instrument and measure the parts of the program not filtered out.
This make profiling faster and the results easier to analyse.

### Trace Analysis

Let's now run the application with the full trace using the `-t` flag
~~~
scalasca -analyze -t mpirun -n 2 poisson
~~~
{: .source .language-bash}
And examine the results written to the `scorep_poisson_2_trace` directory
~~~
scalasca -examine scorep_poisson_2_trace
~~~
{: .source .language-bash}

There are some new measurements on the left column.
Notably you will find a a measure of `MPI_Wait` states, how long the ranks spend
waiting for data.
In addition, you will see the number of MPI communications, and the time spent synchronising
ranks.

>## User defined regions
>
>By default Score-P only collects information about functions.
>You can also define your own regions using annotations 
>in the code.
>
>
>Include the Score-P header file
>~~~
> #include "scorep/SCOREP_User.h"
>~~~
>{:.source .language-c }
>
>First you need to declare the region as a variable:
>~~~
> SCOREP_USER_REGION_DEFINE( region_name );
>~~~
>{:.source .language-c }
>
> The region is then defined by the SCOREP_USER_REGION_BEGIN
> and SCOREP_USER_REGION_END functions:
>~~~
> SCOREP_USER_REGION_BEGIN( region_name, "<region_name>", SCOREP_USER_REGION_TYPE_LOOP );
> ...
> SCOREP_USER_REGION_END( region_name );
>~~~
>{:.source .language-c }
>
> Adding annotations to loops can be done in a single line
>~~~
>SCOREP_USER_REGION( "<loop_name>", SCOREP_USER_REGION_TYPE_LOOP )
>for(i = 0; i < 100; i++) {
>  ...
>}
>~~~
>{:.source .language-c }
>
>When compiling with Score-P, add `--user` to enable user defined regions.
>~~~
>scorep --user mpicc poisson_main_mpi.c poisson_step_mpi.c
>~~~
>{:.source .language-bash }
{: .callout .show-c}

>## User Defined Regions
>
> Using Score-P annotations requires processing the file
> with the C preprocessor.
> The Score-P compiler will do this automatically.
> Include the Score-P header file
>~~~
> #include "scorep/SCOREP_User.inc"
> ~~~
>{:.source .language-fortran}
>
> First you need to declare the region as a variable:
>~~~
> SCOREP_USER_REGION_DEFINE( region_name )
>~~~
>{:.source .language-fortran}
>
> The region is then defined using the SCOREP_USER_REGION_BEGIN
> and SCOREP_USER_REGION_END functions:
>~~~
> SCOREP_USER_REGION_BEGIN( region_name, "<region_name>", SCOREP_USER_REGION_TYPE_LOOP )
> ...
> SCOREP_USER_REGION_END( region_name )
>~~~
>{:.source .language-fortran}
>
>When compiling with Score-P, add `--user` to enable user defined regions.
>~~~
>scorep --user mpifort poisson_mpi.f99
>~~~
>
{: .callout .show-fortran}

>## Annotations
>
> Add a user defined region to poisson step function.
> Include the calculation of unorm, both the loop and
> the reduction.
>
> Use Scalasca to produce a call tree.
>
>>## Solution
>>
>>Here we have used `<norm>` as the region name.
>>
>>~~~
>>Reading scorep_a_4_sum/summary.cubex... done.
>>0.0203862       main                         /main
>>0.136475          + MPI_Init                 /main/MPI_Init
>>8.259e-06         + MPI_Comm_rank            /main/MPI_Comm_rank
>>4.191e-06         + MPI_Comm_size            /main/MPI_Comm_size
>>20.4657           + poisson_step             /main/poisson_step
>>7.8862            |   + <norm>               /main/poisson_step/<norm>
>>2.69831           |   |   + MPI_Allreduce    /main/poisson_step/<norm>/MPI_Allreduce
>>0.311995          |   + MPI_Recv             /main/poisson_step/MPI_Recv
>>0.0263682         |   + MPI_Send             /main/poisson_step/MPI_Send
>>0.00338705        + MPI_Finalize             /main/MPI_Finalize
>>~~~
>>{: .output}
>{: .solution}
{: .challenge}


>## Note on the Linux Subsystem
>
> The profiler will be able to track annotated regions even
> on the Windows Subsystem for Linux.
> If you don't have access to a cluster, you can still work
> iteratively on your program by annotating all interesting
> regions manually.
>
{: .callout}


## Optimisation Workflow

The general workflow for optimising a code, wether parallel or serial
is as follows
1. Profile
2. Optimise
3. Test correctness
4. Measure efficiency
5. Go to 1.


> ## Iterative Improvement
>
> In the Poisson code,
> try changing the location of the calls to `MPI_Send`. How does this affect performance?
>
{: .challenge}

{% include links.md %}

