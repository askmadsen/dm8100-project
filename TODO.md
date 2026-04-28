# TODO list


- Change all matmul implementations to use intermediate variables for speedup.
- Make the matmul version configurable in all main files and use a default (the fastest on large matrices) if not specified.
- Throw error if dim is given but not thread count in openmp
- Try different strategies for openmp, mpi and cuda
- Start on the presentation
