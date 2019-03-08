---
layout: reference
---

## Glossary

### MPI functions:

|-----------------|-----------------|
| `MPI_Init`      | Initialize MPI. Every rank must call in the beginning.        |
| `MPI_Finalize`  | Tear down and free memory. Every rank should call at the end. |
| `MPI_Comm_rank` | Find the number of the current running process.               |
| `MPI_Comm_size` | Find the total number of ranks started by the user.           |
| `MPI_Send`      | Send data to one specified rank.                              |
| `MPI_Recv`      | Receive data from one specified rank.                         |
| `MPI_Isend`     | Start sending data to one specified rank.                     |
| `MPI_Irecv`     | Start receiving data from one specified rank.                 |
| `MPI_Wait`      | Wait for a transfer to complete.                              |
| `MPI_Test`      | Check if a transfer is complete.                              |
| `MPI_Barrier`   | Wait for all the ranks to arrive at this point.               |
| `MPI_Bcast`     | Send the same data to all other ranks.                        |
| `MPI_Scatter`   | Send different data to all other ranks.                       |
| `MPI_Gather`    | Collect data from all other ranks.                            |
| `MPI_Reduce`    | Perform a reduction on data from all ranks and communicate the result to one rank.  |
| `MPI_Allreduce` | Perform a reduction on data from all ranks and communicate the result to all ranks. |

### MPI Types in C

|--|--|
| `char`           | `MPI_CHAR`            |
| `unsigned char`  | `MPI_UNSIGNED_CHAR`   |
| `signed char`    | `MPI_SIGNED_CHAR`     |
| `short`          | `MPI_SHORT`           |
| `unsigned short` | `MPI_UNSIGNED_SHORT`  |
| `int`            | `MPI_INT`             |
| `unsigned int`   | `MPI_UNSIGNED`        |
| `long`           | `MPI_LONG`            |
| `unsigned long`  | `MPI_UNSIGNED_LONG`   |
| `float`          | `MPI_FLOAT`           |
| `double`         | `MPI_DOUBLE`          |
| `long double`    | `MPI_LONG_DOUBLE`     |

### MPI Types in Fortran

|--|--|
| `character`         | `MPI_CHARACTER`         |
| `logical`           | `MPI_LOGICAL`           |
| `integer`           | `MPI_INTEGER`           |
| `real`              | `MPI_REAL`              |
| `double precision`  | `MPI_DOUBLE_PRECISION`  |
| `complex`           | `MPI_COMPLEX`           |
| `complex*16`        | `MPI_DOUBLE_COMPLEX`    |
| `integer*1`         | `MPI_INTEGER1`          |
| `integer*2`         | `MPI_INTEGER2`          |
| `integer*4`         | `MPI_INTEGER4`          |
| `real*2`            | `MPI_REAL2`             |
| `real*4`            | `MPI_REAL4`             |
| `real*8`            | `MPI_REAL8`             |

{% include links.md %}
