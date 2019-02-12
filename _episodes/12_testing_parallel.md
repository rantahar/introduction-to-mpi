---
title: "Testing Parallel Codes"
teaching: 20
exercises: 10
questions:
- "How to make sure the program works?"
- "How parallel execution affect testing?"
objectives:
- "Learn to build tests to verify the parallel program."
- "Point out expected differences between parallel and serial"
keypoints:
- "It's always easy to make mistakes"
- "Use tests to verify the parallel code against a serial code or known results"
---


> ## Tests
>
> Add a test for the max function in the example code.
>
{: .challenge}

FIXME : Floating point operations do not commute, the order of operations affects the least significant digits.

> ## Floating Point Ordering
>
> Add a test for the product function in the example code.
>
{: .challenge}

> ## Test results
>
> Run the test in FIXME. Can you find the errors in the code?
>
{: .challenge}

{% include links.md %}

