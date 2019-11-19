"""a serial code for Poisson equation
   contact seyong.kim81@gmail.com for comments and questions
"""

import numpy as np
import math

def poisson_step(GRIDSIZE, u, unew, rho, hsq):

    # Calculate one timestep
    for j in range(1, GRIDSIZE+1):
        for i in range(1, GRIDSIZE+1):
            difference = u[j][i-1] + u[j][i+1] + u[j-1][i] + u[j+1][i]
            unew[j][i] = 0.25*(difference - hsq*rho[j][i])

    # Find the difference compared to the previous time step
    unorm = 0.0
    for j in range(1, GRIDSIZE+1):
        for i in range(1, GRIDSIZE+1):
            diff = unew[j][i] - u[j][i]
            unorm +=diff*diff

    # Overwrite u with the new field
    for j in range(1, GRIDSIZE+1):
        for i in range(1, GRIDSIZE+1):
            u[j][i] = unew[j][i]

    return unorm

GRIDSIZE = 10

u = np.zeros((GRIDSIZE+2, GRIDSIZE+2))
unew = np.zeros((GRIDSIZE+2, GRIDSIZE+2))
rho = np.zeros((GRIDSIZE+2, GRIDSIZE+2))

# Set up parameters
h = 0.1
hsq = h*h
residual = 1e-5

# Initialise the u and rho field to 0
for j in range(GRIDSIZE+2):
    for i in range(GRIDSIZE+2):
        u[j][i] = 0.0
        rho[j][i] = 0.0

# Create a start configuration with the field
# u=10 at x=0
for j in range(GRIDSIZE+2):
    u[j][0] = 10.0

# Run iterations until the field reaches an equilibrium
for i in range(10000):
    unorm = poisson_step(GRIDSIZE, u, unew, rho, hsq)
    print("Iteration {:d}: Residue {:g}".format(i, unorm))

    if math.sqrt(unorm) < math.sqrt(residual):
        break

print("Run completed with residue", unorm)
