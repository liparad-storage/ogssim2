README for OGSSim v2.

# Introduction

OGSSim (Open and Generic storage System Simulation tool) is developped within
a collaboration project between the University of Versailles St Quentin and
the CEA.

OGSSim is developped by:
- S. Gougeaud (2014-)
- S. Zertal (2014-)
- M. Joulin (2018)

# Repository contents

Directories:
- env: examples of input files
- include: OGSSim headers
- gui: OGSS-GUI files, need to be used within Qt Creator
- src: OGSSim sources

Files:
- README.md
- Makefile
- OGSSim.7: manpage

# Library dependencies

Here is the list of needed libraries and their installation commands:

- cmake: build system
- libgoogle-glog & libgflags: logging tool
- libtinyxml2: XML parser/serializer
- libzmqpp: allow communication between processes by using sockets to transmit
    information
- python matplotlib: chart creation
- libboost-mpi: boost MPI binding

# Documentation

Compilation:
Many types of compilation are available:
- utest: use to launch the unitary tests
- debug: debug mode using threads
- release: release mode using threads
- mpidbg: debug mode using mpi
- mpi: release mode using mpi
$ make {utest|debug|release|mpidbg|mpi}

Execution:
The execution command is:
$ ./OGSSim configuration_file

More information are available in the manpage OGSSim.1:
$ man ./OGSSim.7
