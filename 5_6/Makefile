CXX = clang++
CXXFLAGS = -Wall -g -std=c++17

KosarajuReactor: KosarajuReactor.o mylib.a
	$(CXX) $^ -o $@

#create lib
mylib.a: Reactor.o
	ar -rcs mylib.a Reactor.o

Reactor.o: Reactor.cpp Reactor.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@	

KosarajuReactor.o: KosarajuReactor.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean: 
	rm -f KosarajuReactor *.o *.a