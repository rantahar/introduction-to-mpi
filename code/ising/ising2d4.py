"""serial code for ising model
   contact seyong.kim81@gmail.com for comments and questions
"""

# 2d Ising model using Metropolis update algorithm

import numpy as np
import random
import math

N1 = 320
N1d2 = N1 // 2
N2 = 320

VOLUME = N1 * N2
VOLUMEd2 = N1d2 * N2

xup = np.zeros(VOLUME, dtype=np.int32)
yup = np.zeros(VOLUME, dtype=np.int32)
xdn = np.zeros(VOLUME, dtype=np.int32)
ydn = np.zeros(VOLUME, dtype=np.int32)

s = np.zeros(VOLUME)

# Read parameters. Beta is the inverse of the temperature
fh = open("parameter", "r")
beta = float(fh.readline().split()[1])
num_iter = int(fh.readline().split()[1])
fh.close()

# Set random seed for erand
seed = 17
random.seed(seed)

# Initialize each point randomly
for i in range(VOLUME):
    if random.random() < 0.5:
        s[i] = 1.0
    else:
        s[i] = -1.0

# Create and index of neighbours
# The sites are partitioned to even an odd,
# with even sites first in the array
for j in range(N2):
    for i in range(N1):
        i2 = i // 2
        is_ind = i2 + j * N1d2

        if (i + j) % 2 == 0:
            xup[is_ind] = ((i + 1) // 2) % N1d2 + j * N1d2 + VOLUMEd2
            yup[is_ind] = i2 + ((j + 1) % N2) * N1d2 + VOLUMEd2
            xdn[is_ind] = ((i - 1 + N1) // 2) % N1d2 + j * N1d2 + VOLUMEd2
            ydn[is_ind] = i2 + ((j - 1 + N2) % N2) * N1d2 + VOLUMEd2
        else:
            xup[is_ind + VOLUMEd2] = ((i + 1) // 2) % N1d2 + j * N1d2
            yup[is_ind + VOLUMEd2] = i2 + ((j + 1) % N2) * N1d2
            xdn[is_ind + VOLUMEd2] = ((i - 1 + N1) // 2) % N1d2 + j * N1d2
            ydn[is_ind + VOLUMEd2] = i2 + ((j - 1 + N2) % N2) * N1d2

# Initialize the measurements
esumt = 0.0
magt = 0.0

# Run a number of iterations
for n in range(num_iter):
    esum = 0.0
    mag = 0.0

    # Loop over the lattice and try to flip each atom
    for i in range(VOLUME):
        neighbours = s[xup[i]] + s[yup[i]] + s[xdn[i]] + s[ydn[i]]
        stmp = -s[i]

        # Find the energy before and after the flip
        energy_now = -s[i] * neighbours
        new_energy = -stmp * neighbours
        deltae = new_energy - energy_now

        # Accept or reject the change
        if math.exp(-beta * deltae) > random.random():
            s[i] = stmp
            energy_now = new_energy

        # Measure magnetisation and energy
        mag = mag + s[i]
        esum = esum + energy_now

    # Calculate measurements and add to run averages
    esum = esum / VOLUME
    mag = mag / VOLUME
    esumt = esumt + esum
    magt = magt + abs(mag)

    print("average energy = {:f}, average magnetization = {:f}".format(esum, mag))

esumt = esumt / num_iter
magt = magt / num_iter
print("Over the whole simulation:")
print("average energy = {:f}, average magnetization = {:f}".format(esumt, magt))
