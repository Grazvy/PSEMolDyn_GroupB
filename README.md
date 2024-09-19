# Molecular Dynamics Simulator

As part of our practical course at TUM, we built this project that simulates the 
physical interactions between simple particles.

It takes an xml input of the initial state, which can be any layout of particles
with specific weights and velocities, in 2D or 3D space. The input supports cuboids, 
spheres and even final states from previous simulations, such as an equilibrium.

It supports various boundary types:

- **reflective** boundaries contain particles within the domain
- **outflow** boundaries allow particles to escape 
- **periodic** boundaries enable interaction with particles on the opposite border

We designed the architecture with focus on scalability, removing interference 
between threads, to utilize the 56 available threads on the Linux-Cluster, winning 
us the parallelization-contest within the course.

With this project, we got a first insight into High-Performance-Computing, 
we learned the basics of numerical simulation and gained valuable experience
in software engineering.

## Results


## Build / Usage
```
mkdir build && cd build
ccmake ..

#for executable
make

#for usage type -h or no arguments, yields:
Usage ./MolSim [-l<String>] [-p] [-o] -f<String>
 Info:              See the /input folder for the parameters.xsd schema, in which 
                    program arguments should be specified
 -f<String>:        gives the filename of an .xml file, that has to follow
                    the xsd schema defined in input/parameters.xsd.
                    from this file all programm arguments / options will be read(see README)
 -l<String>:        specifies the level of logging, e.g. how fine grained programm logs are.
                    can either be "off" "trace", "debug", "info", "error" or "critical".
                    The default level is "debug".
 -h                 prints a help message
 -p                 if the flag is set, the programm will measure the time for the execution.
                    therefore no vtk output and no logging will happen (specifing a log level at
                    the same time is undefined behaviour)

Returns:
                  several .vtu files that can be used for visualisation in Paraview

#for documentation
make doc_doxygen 
```

**Notes:**
Call ./MolSim with no arguments or the -h argument to get a help message about the
command line arguments and what is being returned by the executable. 





