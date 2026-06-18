#!/bin/bash
rm -f dm8100-project.zip

zip -r dm8100-project.zip \
    Makefile \
    presentation/presentation.pdf \
    src \
    scripts/correctness.sh \
    scripts/plot.py \
    scripts/run_cuda.sh \
    scripts/run_mpi.sh \
    scripts/run_openmp.sh \
