"""parallel code for ising model
   contact seyong.kim81@gmail.com for comments and questions
"""

# 2d Ising model using Metropolis update algorithm
# periodic boundary condition for x- and y-direction
# version 3 : checker board partitioned
# MPI version
# assumes that 1-D ring topology in the y direction and no of ranks is
# a divisor of N2 and N2/n_ranks is an even integer for convenience

from mpi4py import MPI
import numpy as np
import random
import math
import sys

N1 = 320
N1d2 = N1 // 2
N2 = 320

VOLUME = N1 * N2
VOLUMEd2 = N1 * N2

xup = np.zeros(VOLUME, dtype=np.int32)
yup = np.zeros(VOLUME, dtype=np.int32)
xdn = np.zeros(VOLUME, dtype=np.int32)
ydn = np.zeros(VOLUME, dtype=np.int32)

s = np.zeros(VOLUME)
sendbuf = np.zeros(N1d2)
recvbuf = np.zeros(N1d2)

# Initialize MPI and set rank parameters
rank = MPI.COMM_WORLD.Get_rank()
n_ranks = MPI.COMM_WORLD.Get_size()

iroot = 0
subN2 = N2 // n_ranks
subVOLUME = VOLUME // n_ranks
subVOLUMEd2 = subVOLUME // 2
nextup = (rank + 1) % n_ranks
nextdn = (rank - 1 + n_ranks) % n_ranks

# Read parameters. Beta is the inverse of the temperature.
if rank == 0:
    fh = open("parameter", "r")
    beta = float(fh.readline().split()[1])
    num_iter = int(fh.readline().split()[1])
    fh.close()
    print("Beta = {:f}".format(beta))
    print("Iter = {:d}".format(num_iter))
else:
    beta = None
    num_iter = None

# Broadcast parameters to all ranks
beta = MPI.COMM_WORLD.bcast(beta, root=0)
num_iter = MPI.COMM_WORLD.bcast(num_iter, root=0)

# Set random seed for erand. Needs to be different for each rank
seed = 17 + rank
random.seed(seed)

# Initialize each point randomly
for i in range(subVOLUME):
    if random.random() < 0.5:
        s[i] = 1.0
    else:
        s[i] = -1.0

# Create and index of neighbours
# The sites are partitioned to even an odd,
# with even sites first in the array
for j in range(subN2):
    for i in range(N1):
        ij = i + j
        i2 = i // 2
        is_ind = i2 + j * N1d2
        if ij == 2 * (ij // 2):
            xup[is_ind] = ((i + 1) // 2) % N1d2 + j * N1d2 + subVOLUMEd2
            yup[is_ind] = i2 + ((j + 1) % subN2) * N1d2 + subVOLUMEd2
            xdn[is_ind] = (
                (i - 1 + N1 * 2) // 2) % N1d2 + j * N1d2 + subVOLUMEd2
            ydn[is_ind] = i2 + ((j - 1 + N2) % subN2) * N1d2 + subVOLUMEd2
        else:
            xup[is_ind + subVOLUMEd2] = ((i + 1) // 2) % N1d2 + j * N1d2
            yup[is_ind + subVOLUMEd2] = i2 + ((j + 1) % subN2) * N1d2
            xdn[is_ind + subVOLUMEd2] = ((i - 1 + N1) // 2) % N1d2 + j * N1d2
            ydn[is_ind + subVOLUMEd2] = i2 + ((j - 1 + N2) % subN2) * N1d2

# Initialize the measurements
esumt = 0.0
magt = 0.0

# Run a number of iterations
for n in range(num_iter):
    esum = 0.0
    mag = 0.0
    esumsub = 0.0
    magsub = 0.0

    # Do for even and odd sites
    for parity in range(2):
        other_parity = 1 - parity

        # Communicate and update the j=0 boundary
        itag = 11
        ii = parity * subVOLUMEd2
        for i in range(N1d2):
            sendbuf[i] = s[i + subVOLUMEd2 - N1d2 + other_parity * subVOLUMEd2]
        req = MPI.COMM_WORLD.irecv(source=nextdn, tag=itag)
        MPI.COMM_WORLD.send(sendbuf, dest=nextup, tag=itag)
        recvbuf = req.wait()

        for i in range(N1d2):
            neighbours = s[xup[i + ii]] + s[yup[i + ii]] + s[xdn[i + ii]] + recvbuf[i]
            stmp = -s[i + ii]
            energy_now = -s[i + ii] * neighbours
            new_energy = -stmp * neighbours
            deltae = new_energy - energy_now
            if math.exp(-beta * deltae) > random.random():
                s[i + ii] = stmp
                energy_now = new_energy
            magsub = magsub + s[i + ii]
            esumsub = esumsub + energy_now

        # Update the bulk of the lattice, everything but the boundaries
        for j in range(1, subN2 - 1):
            for i in range(N1d2):
                is_ind = i + N1d2 * j + ii
                neighbours = s[xup[is_ind]] + s[yup[is_ind]] + s[
                    xdn[is_ind]] + s[ydn[is_ind]]
                stmp = -s[is_ind]
                energy_now = -s[is_ind] * neighbours
                new_energy = -stmp * neighbours
                deltae = new_energy - energy_now
                if math.exp(-beta * deltae) > random.random():
                    s[is_ind] = stmp
                    energy_now = new_energy
                magsub = magsub + s[is_ind]
                esumsub = esumsub + energy_now

        # Update the j = subN2-1 boundary
        itag = 22
        for i in range(N1d2):
            sendbuf[i] = s[i + other_parity * subVOLUMEd2]
        req = MPI.COMM_WORLD.irecv(source=nextup, tag=itag)
        MPI.COMM_WORLD.send(sendbuf, dest=nextdn, tag=itag)
        recvbuf = req.wait()

        ii = ii + subVOLUMEd2 - N1d2
        for i in range(N1d2):
            neighbours = s[xup[i + ii]] + recvbuf[i] + s[xdn[i + ii]] + s[ydn[i + ii]]
            stmp = -s[i + ii]
            energy_now = -s[i + ii] * neighbours
            new_energy = -stmp * neighbours
            deltae = new_energy - energy_now
            if math.exp(-beta * deltae) > random.random():
                s[i + ii] = stmp
                energy_now = new_energy
            magsub = magsub + s[i + ii]
            esumsub = esumsub + energy_now

        # Sum the energy and magnetisation over the ranks
        esum = MPI.COMM_WORLD.reduce(esumsub, op=MPI.SUM, root=iroot)
        mag = MPI.COMM_WORLD.reduce(magsub, op=MPI.SUM, root=iroot)

    # Calculate average measurements and print
    if rank == 0:
        esum = esum / VOLUME
        mag = mag / VOLUME
        esumt = esumt + esum
        magt = magt + abs(mag)

        print("average energy = {:f}, average magnetization = {:f}".format(esum, mag))
        sys.stdout.flush()

esumt = esumt / num_iter
magt = magt / num_iter
if rank == 0:
    print("Over the whole simulation:")
    print("average energy = {:f}, average magnetization = {:f}".format(esumt, magt))
