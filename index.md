---
layout: lesson
root: .  # Is the only page that doesn't follow the pattern /:path/index.html
permalink: index.html  # Is the only page that doesn't follow the pattern /:path/index.html
---

As processors develop, it's getting harder to increase the their clock speed. Instead, new processors tend to have more processing units.
To take advantage of the increased resources, programs need to be written to run in parallel.

In High Performance Computing (HPC), a large number of state-of-the-art computers are joined together with a fast network. 
Using an HPC system efficiently requires a well designed parallel
algorithm.

MPI stands for Message Passing Interface.
It is a straightforward standard for communicating between the
individual processes that make up a program.
There are several implementations of the standard for nearly most platforms
(Linux, Windows, OS X...)
and many popular languages
(C, C++, Fortran, Python...).

This workshop introduces general concepts in parallel programming and 
the most important functions of the Message Passing Interface.


<!-- this is an html comment -->

{% comment %} This is a comment in Liquid {% endcomment %}

> ## Prerequisites
>
> This course assumes you are familiar with C, or Fortran. It is useful to bring your own code, either a serial code you wish to make parallel or a parallel code you wish to understand better.
{: .prereq}

{% include links.md %}
