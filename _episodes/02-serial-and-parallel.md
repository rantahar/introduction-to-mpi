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
Now you get two cars in the same amount of time!
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

In the analogy with car manufacturing, the speed of conveyor belt is the "clock" of CPU, parts are "input", doing something is an "operation", and the car structure on the conveyor belt is "output". 

So, we can think of an algorithm as a series of black boxes with an input and an output.
![Input -> Algorithm -> Output]({{ page.root }}{% link files/serial_task_flow.png %})
The algorithm itself consists of steps, operations we need to do to the data
to arrive at a solution.
In the car example the input are mechanical parts, the steps adding and adjusting individual
parts and the output is a new car.
![Input -> Step 1 -> Step 2 -> Step 3 -> Output]({{ page.root }}{% link files/serial_multi_task_flow.png %})

If we want to produce a single car faster, maybe we can do some of the work in parallel.
Let's say we can hire four workers ot attach each of the tires at the same time.
All of these steps have the same input, a car without tires,
and they work together to create an output, a car with tires.
![Input -> Step 1 / Step 2 / Step 3 -> Output]({{ page.root }}{% link files/parallel_simple_flow.png %})


### Data Dependency
Another example, and a common operation in scientific computation, is calculating the sum of a set
of numbers.
A simple implementation might look like this:
~~~
sum = 0
for number in numbers
   sum = sum + number
~~~
This is a very bad parallel algorithm!
Every step, or iteration of the for loop, uses the same sum variable.
To execute a step, the program needs to know the sum from the previous step.

A part of the program that cannot be run in parallel is called a "serial region" and
a part that can be run in parallel is called a "parallel region".
Any program will have some serial regions.
In a good parallel program, most of the time is spent in parallel regions.

The important factor that determines wether steps can be run in parallel is data dependencies.
In our sum example, every step depends on data from the previous step, the value of the sum variable.
When attaching tires to a car, attaching one tire does not depend on attaching the others, so these steps can be done at the same time.

However, attaching the front tires both require that the axis is there.
This step must be completed first, but the two tires can then be attached at the same time.
![Input -> Task 1 -> Task 2 / Task 3 -> Output]({{ page.root }}{% link files/parallel_complicated_flow.png %})


<!-- The other option is to run task 1 twice.
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
-->

Any algorithm will have parallel regions and serial regions.
The program can never run faster than the sum of the serial regions.
Fortunately, the parallel part is often much larger than the serial part.

>## Serial and Parallel regions
>
> Identify serial and parallel regions the following algorithm
>
> ~~~
>  vector_0[0] = 1;
>  vector_1[1] = 1;
>  for i in 2 ... 1000
>    vector_1[i] = vector_1[i-1] + vector_1[i-2];
>
>  for i in 0 ... 1000
>    vector_2[i] = i;
>
>  for i in 0 ... 1000
>    vector_3[i] = vector_2[i] + vector_1[i];
>    print("The sum of the vectors is.", vector_3[i]);
>~~~
>{: .output}
>
> ## Solution in C
> ~~~
>  serial   | vector_0[0] = 1;
>  serial   | vector_1[1] = 1;
>  serial   | for i in 2 ... 1000
>  serial   |   vector_1[i] = vector_1[i-1] + vector_1[i-2];
>
>  parallel | for i in 0 ... 1000
>  parallel |   vector_2[i] = i;
>
>  parallel | for i in 0 ... 1000
>  parallel |   vector_3[i] = vector_2[i] + vector_1[i];
>  parallel |   print("The sum of the vectors is.", vector_3[i]);
> ~~~
> {: .output}
>
> In the first loop, every iteration depends on data from the previous two.
> In the second two loops, nothing in a step depens on any of the other steps.
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

