FLAGS := -Wall -Wextra -fopenmp

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=obj/%.o)

all: target/serial

target/serial: $(OBJS)
	gcc $(OBJS) -lm -fopenmp -o $@

obj/%.o: src/%.c | obj target
	gcc $(FLAGS) -c $< -o $@

obj:
	mkdir obj

target:
	mkdir target

clean:
	rm -rf obj
	rm -rf target

.PHONY: all clean
