FLAGS := -Wall -Wextra -fopenmp

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=obj/%.o)

all: target/serial

target/serial: $(OBJS)
	gcc $(OBJS) -o $@
	
obj/%.o: src/%.c | dirs
	gcc $(FLAGS) -c $< -o $@

dirs:
	mkdir obj
	mkdir target

clean: 
	rm -rf obj
	rm -rf target

.PHONY: all clean