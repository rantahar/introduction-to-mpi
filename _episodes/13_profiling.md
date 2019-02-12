---
title: "Profiling Parallel Applications"
teaching: 30
exercises: 30
questions:
- "It works, what is taking time?"
objectives:
- "Use a profiler to find bottlenecks and hotspots and diagnose problems."
- "Reinforce the idea that profiling is an efficient way to analyse code."
keypoints:
- "Use a profiler to find the most important functions and concentrate on those."
- "The Profiler needs to understand MPI. Your cluster probably has one."
- "If a lot of time is spent in communication, maybe it can be rearranged."
---

> ## Profiling
>
> Use a profiler to study your research code or the code provided.
> Identify hotspots and decide how to improve the application.
>
{: .challenge}

> ## Iterative Improvement
>
> Try changing the location of the call to MPI_Send. How does this affect performance?
>
{: .challenge}

{% include links.md %}

