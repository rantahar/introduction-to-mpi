---
title: "MPI_Send and MPI_Recv"
teaching: 15
exercises: 15
questions:
- "How do I send data from one process to another"
objectives:
- "Introduce the MPI_Send and MPI_Recv functions"
keypoints:
- "MPI_Send"
- "MPI_Recv"
- "Message tags"
- "These functions will block the program untill the message is received"
---

> ## Sending and Receiving
>
> Modify the Hello World code so that each process sends its
> message to rank 0. Have rank 0 print each message.
>
{: .challenge}

> ## Blocking
>
> - What happens in this code? Why?
> - How would you change the code to fix the problem?
>
{: .challenge}

FIXME

{% include links.md %}

