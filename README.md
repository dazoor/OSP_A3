# OSP_A3
Daniel Azoor - s3944498

COMPILATION AND RUNNING INSTRUCTIONS:

"make all" will compile both memory allocation methods at the same time, into the executables
"firstfit" and "bestfit".

"make firstfit" and "make bestfit" will compile each program individually, with the same executable
names as listed above.

all three programs compile on school servers with the flags -Wall -Werror -std=c++20 provided
devtoolset-11 is enabled via this command: $scl enable devtoolset-11 bash

"make clean" will clear all executables from the directory