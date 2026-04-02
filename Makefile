CC       := gcc
MPICC    := mpicc
FLAGS    := -Wall -Wextra -O2
OMPFLAGS := -fopenmp

all: target/main_serial target/main_openmp target/main_correctness target/openmpi | target

# Executables
target/main_serial: obj/main_serial.o obj/matrix.o obj/serial.o | target
	$(CC) $^ -lm -o $@

target/main_openmp: obj/main_openmp.o obj/matrix.o obj/openmp.o | target
	$(CC) $^ -lm $(OMPFLAGS) -o $@

target/main_correctness: obj/main_correctness.o obj/matrix.o obj/serial.o obj/openmp.o | target
	$(CC) $^ -lm $(OMPFLAGS) -o $@

target/openmpi: obj/openmpi.o obj/matrix.o obj/serial.o | target
	$(MPICC) $^ -lm -o $@

# Object files
obj/openmp.o: src/openmp.c | obj
	$(CC) $(FLAGS) $(OMPFLAGS) -c $< -o $@

obj/main_openmp.o: src/main_openmp.c | obj
	$(CC) $(FLAGS) $(OMPFLAGS) -c $< -o $@

obj/main_correctness.o: src/main_correctness.c | obj
	$(CC) $(FLAGS) $(OMPFLAGS) -c $< -o $@

obj/openmpi.o: src/openmpi.c | obj
	$(MPICC) $(FLAGS) -c $< -o $@

obj/%.o: src/%.c | obj
	$(CC) $(FLAGS) -c $< -o $@

obj:
	mkdir -p obj

target:
	mkdir -p target

clean:
	rm -rf obj target

.PHONY: all clean
