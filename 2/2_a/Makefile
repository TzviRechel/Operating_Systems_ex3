.PHONY: all clean profiling

all: KosarjuList KosarjuStack 

KosarjuList: KosarjuList.cpp
	clang++ -pg $^ -o $@

KosarjuStack: KosarjuStack.cpp
	clang++ -pg $^ -o $@	

giveInput: giveInput.c
	gcc $^ -o $@
	./giveInput

profiling: KosarjuList KosarjuStack
# KosarjuList:
	./KosarjuList < input.txt
	gprof KosarjuList > list.txt
# KosarjuStack:
	./KosarjuStack < input.txt
	gprof KosarjuStack > stack.txt

# Cleaning:
clean:
	rm -f KosarjuList KosarjuStack 
