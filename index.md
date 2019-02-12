---
layout: lesson
root: .  # Is the only page that doesn't follow the pattern /:path/index.html
permalink: index.html  # Is the only page that doesn't follow the pattern /:path/index.html
---

MPI provides a straight-forward interface to write software that can use multiple cores of a computer, and multiple computers in a cluster or nodes in a supercomputer. Using MPI you can write code that uses all of the cores and all of the nodes in a multicore computer cluster, and that will run faster as more cores and more compute nodes become available.

MPI is a well-established, standard method of writing parallel programs. It was first released in 1992, and is currently on version 2. MPI is implemented as a library, which is available for nearly all computer platforms (e.g. Linux, Windows, OS X), and with interfaces for many popular languages (e.g. C, C++, Fortran, Python).

MPI stands for “Message Passing Interface”, and it parallelises computational work by providing tools that let you use a team of processes to solve the problem, and for the team to then share the solution by passing messages amongst one another. MPI can be used to parallelise programs that run locally, by having all processes in the team run locally, or it can be used to parallelise programs across a compute cluster, by running one or more processes per node. MPI can be combined with other parallel programming technologies, e.g. OpenMP. This course is presented as a companion to my OpenMP course, with both this and the OpenMP course following a similar structure and presenting similar examples. If you want to compare MPI and OpenMP, then please click on the Compare with OpenMP links on each page.

You can read about the history of MPI at its Wikipedia page, or by going to one of the many MPI websites. The best book to learn MPI in my opinion is Using MPI: Portable Parallel Programming with the Message-Passing Interface (I used it to learn MPI).

<!-- this is an html comment -->

{% comment %} This is a comment in Liquid {% endcomment %}

> ## Prerequisites
>
> This course assumes you are familiar with C, C++ or Fortran. It is useful to bring your own code, either a serial code you wish to make parallel or a parallel code you wish to understand better.
{: .prereq}

{% include links.md %}
