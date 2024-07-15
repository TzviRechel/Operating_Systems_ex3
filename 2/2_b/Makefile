.PHONY: all clean profiling

all: KosarjuAdjList KosarjuAdjMat 

KosarjuAdjList: KosarjuAdjList.cpp
	clang++ -pg $^ -o $@

KosarjuAdjMat: KosarjuAdjMat.cpp
	clang++ -pg $^ -o $@	

giveInput: giveInput.c
	gcc $^ -o $@
	./giveInput

profiling: KosarjuAdjList KosarjuAdjMat
# KosarjuAdjList:
	./KosarjuAdjList < input.txt > List.txt
	gprof KosarjuAdjList > adjList.txt
# KosarjuAdjMat:
	./KosarjuAdjMat < input.txt > mat.txt
	gprof KosarjuAdjMat > adjMat.txt

# Cleaning:
clean:
	rm -f KosarjuAdjList KosarjuAdjMat 