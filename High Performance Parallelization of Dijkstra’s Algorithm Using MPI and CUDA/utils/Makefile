CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

all: graph_generator adjacency_matrix

graph_generator: graph_generator.cpp
	$(CXX) $(CXXFLAGS) -o graph_generator graph_generator.cpp

adjacency_matrix: adjacency_matrix.cpp
	$(CXX) $(CXXFLAGS) -o adjacency_matrix adjacency_matrix.cpp

clean:
	rm -f graph_generator adjacency_matrix 