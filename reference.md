---
layout: reference
---

## Glossary

### MPI functions:

|---------------|-----------------|
| MPI_Init      | Initialize MPI. Every rank must call in the beginning.        |
| MPI_Finalize  | Tear down and free memory. Every rank should call at the end. |
| MPI_Comm_rank | Find the number of the current running process.               |
| MPI_Comm_size | Find the total number of ranks started by the user.           |
|---------------|-----------------|
| MPI_Send      | Send data to one specified rank.                              |
| MPI_Recv      | Receive data from one specified rank.                         |
| MPI_ISend     | Start sending data to one specified rank.                     |
| MPI_IRecv     | Start receiving data from one specified rank.                 |
| MPI_Wait      | Wait for a transfer to complete.                              |
| MPI_Test      | Check if a transfer is complete.                              |
|---------------|-----------------|
| MPI_Barrier   | Wait for all the ranks to arrive at this point.               |
| MPI_Bcast     | Send the same data to all other ranks.                        |
| MPI_Scatter   | Send different data to all other ranks.                       |
| MPI_Gather    | Collect data from all other ranks.                            |
| MPI_Reduce    | Perform a reduction on data from all ranks and communicate the result to one rank.  |
| MPI_AllReduce | Perform a reduction on data from all ranks and communicate the result to all ranks. |

### MPI Types in C

|--|--|
| char           | MPI_CHAR            |
| unsigned char  | MPI_UNSIGNED_CHAR   |
| signed char    | MPI_SIGNED_CHAR     |
| short          | MPI_SHORT           |
| unsigned short | MPI_UNSIGNED_SHORT  |
| int            | MPI_INT             |
| unsigned int   | MPI_UNSIGNED        |
| long           | MPI_LONG            |
| unsigned long  | MPI_UNSIGNED_LONG   |
| float          | MPI_FLOAT           |
| double         | MPI_DOUBLE          |
| long double    | MPI_LONG_DOUBLE     |

### MPI Types in Fortran

|--|--|
| MPI_CHARACTER         | character         |
| MPI_LOGICAL           | logical           |
| MPI_INTEGER	        | integer           |
| MPI_REAL	            | real              |
| MPI_DOUBLE_PRECISION	| double precision  |
| MPI_COMPLEX	        | complex           |
| MPI_DOUBLE_COMPLEX    | complex*16        |
| MPI INTEGER1          | integer*1         |
| MPI INTEGER2          | integer*2         |
| MPI INTEGER4          | integer*4         |
| MPI REAL2             | real*2            |
| MPI REAL4             | real*4            |
| MPI REAL8             | real*8            |

{% include links.md %}
