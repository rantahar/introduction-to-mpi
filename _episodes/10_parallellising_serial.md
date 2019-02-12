---
title: "Parallellising a Serial Code"
teaching: 30
exercises: 30
questions:
- "What is the best way to write a parallel code?"
- "How do I parallellise my serial code?"
objectives:
- "Go through practical steps for going from a serial code to a parallel code"
- "Allow students to work on their own code or on example codes"
keypoints:
- "Start from a working serial code"
- "Write a parallel implementation for each function or parallel region"
- "Connect the parallel regions with a minimal amount of communication"
- "Continuously compare with the working serial code"
---

> ## Parallel Regions, review
>
> Download the [C]({{ page.root }}{% link code/poisson/poisson.c %})
> or [Fortran]({{ page.root }}{% link code/poisson/poisson.f %}) code
> provided and read trough the code.
> Identify any potential parallel regions.
>
> You may also wish to work with your own serial code. If so,
> do this exercise with a small part of your own code.
>
{: .challenge}

{% include links.md %}

