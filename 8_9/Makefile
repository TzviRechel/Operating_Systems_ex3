CXX = clang++
CXXFLAGS = -Wall -g -std=c++17

KosarajuProactor: KosarajuProactor.o mylib.a
	$(CXX) -pthread $^ -o $@

#create lib
mylib.a: Proactor.o
	ar -rcs mylib.a Proactor.o

Proactor.o: Proactor.cpp Proactor.hpp
	$(CXX) -pthread $(CXXFLAGS) -c $< -o $@	

KosarajuProactor.o: KosarajuProactor.cpp
	$(CXX) -pthread $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean: 
	rm -f KosarajuProactor *.o *.a