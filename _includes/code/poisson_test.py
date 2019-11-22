>> import numpy as np
>> import math
>>
>> GRIDSIZE = 10
>>
>> u = np.zeros((GRIDSIZE+2, GRIDSIZE+2))
>> unew = np.zeros((GRIDSIZE+2, GRIDSIZE+2))
>> rho = np.zeros((GRIDSIZE+2, GRIDSIZE+2))
>>
>> # Set up parameters
>> h = 0.1
>>
>> # Run Setup
>> hsq = h**2
>>
>> # Initialise the u and rho field to 0
>> for j in range(GRIDSIZE+2):
>>     for i in range(GRIDSIZE+2):
>>         u[j][i] = 0.0
>>         rho[j][i] = 0.0
>>
>> # Test a configuration with u=10 at x=1 and y=1
>> u[1][1] = 10
>>
>> # Run a single iteration first
>> unorm = poisson_step(GRIDSIZE, u, unew, rho, hsq)
>>
>> if unorm == 112.5:
>>     print("TEST SUCCEEDED after 1 iteration")
>> else:
>>     print("TEST FAILED after 1 iteration")
>>
>> for i in range(1, 10):
>>     unorm = poisson_step(GRIDSIZE, u, unew, rho, hsq)
>>
>> if abs(unorm - 0.208634816) < 1e-6:
>>     print("TEST SUCCEEDED after 10 iteration")
>> else:
>>     print("TEST FAILED after 10 iteration")
