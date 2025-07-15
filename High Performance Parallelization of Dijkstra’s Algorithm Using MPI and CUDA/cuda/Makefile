NVCC = nvcc
NVCCFLAGS = -O2 -arch=sm_60

all: dijkstra_cuda

dijkstra_cuda: dijkstra_cuda.cu
	$(NVCC) $(NVCCFLAGS) -o dijkstra_cuda dijkstra_cuda.cu

clean:
	rm -f dijkstra_cuda 