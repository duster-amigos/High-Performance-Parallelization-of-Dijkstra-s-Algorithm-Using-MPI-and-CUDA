CXX = mpic++
CXXFLAGS = -std=c++17 -O2 -Wall

all: dijkstra_mpi

dijkstra_mpi: dijkstra_mpi.cpp
	$(CXX) $(CXXFLAGS) -o dijkstra_mpi dijkstra_mpi.cpp

clean:
	rm -f dijkstra_mpi 