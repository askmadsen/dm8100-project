# TODO list


- Make the matmul version configurable in all main files and use a default (the fastest on large matrices) if not specified.
- Add documentation to all c files.
- Clean up all files.
- Test divide and conqouerer matmul on MPI so we split the matrix into n rank chunks and let each rank divide the matrix up further
- Test cuda by giving a block to each cuda thread instead of each cuda thread only computes one elemetns of the final matrix.
- Start on the presentation
