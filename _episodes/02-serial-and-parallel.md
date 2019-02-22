---
title: "Serial and Parallel Regions"
teaching: 20
exercises: 20
questions:
- "When is parallel computing useful?"
- "How fast can my code run?"
objectives:
- "Understanding theoretical limitations on parallel algorithms"
keypoints:
- "Algorithms can have parallellisable and non-parallellisable sections"
- "A highly parallel algorithm may be slower on a single processor"
- "The theoretical maximum speed is determined by the serial sections"
- "The other main restriction is communication speed between the processes"
---

## Serial and Parallel Execution

An algorithm is a series of steps to solve a problem.
Let us imagine these steps as a familiar scene in car manufacturing.
Each step adds a component to an existing structure or adjusts a component (like tightening bolt)
to form a car as the conveyor belt moves the result of each steps.
As a designer of these processes, you would carefully order the way you add components
so that you don't have to disassemble already constructed parts.
How long does it take to build a car in this way?
Well, it will be the time difference between the beginning of the steps and the ending of the steps (of course, each step may take a different amount of time to complete).

Now, you would like to build cars faster since there are impatient customers waiting in line!
What do you do? You build two lines of conveyor belts and hire twice number of people to do the work.
Now two cars per the same amount of time!
If you build three conveyor belts and hire thrice the number of people,
you get three cars for the same amount of time.
Or you can identify the steps which don't intefere with each other and can be executed at the same time
(for example, the front part of the car can be constructed independently of the rear part of the car)
and let two people work at the same time.
If you find more steps which can be done without interfering each other,
these steps can be executed at the same time.
Overall time is saved.
More cars for the same amount of time!

If your problem can be solved like the first case, you are in the luck.
You just need more CPU's (or cores) to do the work.
This is called "Embarrasingly Parallel (EP)" problem.
It is the easiest problem to parallelize.
In the second case, the most important thing to consider is the indepedence of a step
(that is, whether a step can be executed without interfering other steps).
This independency is called "atomicity" of an operation.

In the analogy with car manufacturing, the speed of conveyor belt is the "clock" of CPU, parts are "input", doing something is "operation", and the car structure on the conveyor belt is "output". 

So, we can think of an algorithm as a series of black boxes with an input and an output.
![Input -> Algorithm -> Output]({{ page.root }}{% link files/serial_task_flow.png %})
To see where parallel processing is useful, we need to take a closer look at the algorithm.

A program generally runs in successive steps, each processing some input and producing the output.
![Input -> Task 1 -> Task 2 -> Task 3 -> Output]({{ page.root }}{% link files/serial_multi_task_flow.png %})
If each task actually just uses the same input,
or takes no input at all, as our example in the previous lesson,
we can run them all in parallel.
![Input -> Task 1 / Task 2 / Task 3 -> Output]({{ page.root }}{% link files/parallel_simple_flow.png %})
This would run as fast as the slowest of the tasks.

Life is rarely that simple, and if the tasks were actually completely separate,
we would have written three separate programs instead of one.
Usually some of the tasks in a program can be run in parallel and some cannot.
These are called parallel and serial regions.
Let's say now that the output of task 1 is needed in both task 2 and task 3.
There are at least two ways of parallellising this algorithm.

We can run task 1 on the first rank, send the result to a second rank to run task 2
and start task 3 on the first rank.
![Input -> Task 1 -> Task 2 / Task 3 -> Output]({{ page.root }}{% link files/parallel_complicated_flow.png %})
The other option is to run task 1 twice.
![Input -> ( Task 1 -> Task 2 / task1 -> Task 3 ) -> Output]({{ page.root }}{% link files/parallel_more_complicated_flow.png %})

>## When to communicate
>
> When would the second option be better?
>
> > ## Solution
> >
> > Wether the first or the second option is better depends on
> > * The amount of data task 2 needs from task 1 in the first option
> > * The amount of input data task 1 needs in the second option
> > * How fast you can transfer data
> > * How much work task 1 is
> > * Is the second core doing anything useful
> >
> > We often need to perform the same small tasks on all the ranks. This might seem wasteful,
> > but if communicating the result takes more time, it may not be.
> >
> {: .solution}
{: .discussion}

Any algorithm will have parallel regions and serial regions.
The parallel algorithm can never run faster than the sum of the serial regions.
Fortunately, the parallel part is often much larger than the serial part.

>## Serial and Parallel regions
>
> Identify serial and parallel regions the code below
>
> > ## C
> > ~~~
> > #include <stdio.h>
> > 
> > main(int argc, char** argv) {
> >   int N = 10;
> >   int vector_1[N], vector_2[N], vector_3[N];
> >
> >   vector_1[0] = 1;
> >   vector_1[1] = 1;
> >   for( int i=2; i<N; i++ )  {
> >     vector_1[i] = vector_1[i-1] + vector_1[i-2];
> >   }
> >
> >   for( int i=0; i<N; i++ )  {
> >     vector_1[i] = i;
> >   }
> >
> >   for( int i=0; i<N; i++ ) {
> >     vector_3[i] = vector_2[i] + vector_1[i];
> >     print("The sum of the vectors is.", vector_3[i]);
> >   }
> > }
> > ~~~
> > {: .output}
> {: .prereq .foldable}
>
> > ## Solution in C
> > ~~~
> > #include <stdio.h>
> > 
> > main(int argc, char** argv) {
> >   // Serial
> >   int N = 10;
> >   int vector_1[N], vector_2[N], vector_3[N];
> >
> >   // Serial: Each iteration of the loop needs data from previous iterations
> >   vector_1[0] = 1;
> >   vector_1[1] = 1;
> >   for( int i=2; i<N; i++ )  {
> >     vector_1[i] = vector_1[i-1] + vector_1[i-2];
> >   }
> >
> >   // Parallel: Each iteration is independent
> >   for( int i=0; i<N; i++ )  {
> >     vector_1[i] = i;
> >   }
> >
> >   // Parallel: Each itetration is independent
> >   for( int i=0; i<N; i++ ) {
> >     vector_3[i] = vector_2[i] + vector_1[i];
> >     print("The sum of the vectors is.", vector_3[i]);
> >   }
> > }
> > ~~~
> > {: .output}
> {: .solution}
>
>
{: .challenge}

## Amdahl's Law

The time it takes to execute the program is roughly
![T = T_{serial} + T_{parallel}/N_ranks + T_communication(N_ranks)]({{ page.root }}{% link fig/amdahl_equation.png %}){:height="50%" width="50%"}

The N<sub>ranks</sub> here is the number of ranks and
T<sub>communication</sub> represents the time spent communicating between the ranks.

![A figure showing the result described above for MAX=512 and MAX=2048]({{ page.root }}{% link fig/poisson_scaling_plot.png %})

The other significant factors in the speed of a parallel program are
communication speed, latency and of course the number of parallel processes.
When the number of ranks is small, time spent in communication is not significant
and the parallel regions get faster with the number of ranks.
But if we keep increasing the number of ranks the time spent in communication grows.

{% include links.md %}

