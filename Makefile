CFLAGS = -O3 -std=c++11
EXE = ms_*
OUTPUT = $(EXE)
header = src/display.cpp src/utils.cpp

all: clean mpi

seq:
	$(eval TARGET := ms_seq)
	g++ src/mandelbrot-seq.cpp $(header) -o $(TARGET) -lX11 $(LOG) $(CFLAGS)
mp:
	$(eval TARGET := ms_openmp)
	g++ src/mandelbrot-openmp-static.cpp $(header) -o $(TARGET)-static -lX11 -fopenmp $(LOG) $(CFLAGS)
	g++ src/mandelbrot-openmp-dynamic.cpp $(header) -o $(TARGET)-dynamic -lX11 -fopenmp $(LOG) $(CFLAGS)

mpi:
	$(eval TARGET := ms_mpi)
	mpic++ src/mandelbrot-mpi-static.cpp $(header) -o $(TARGET)-static -lX11 $(LOG) $(CFLAGS)
	mpic++ src/mandelbrot-mpi-dynamic.cpp $(header) -o $(TARGET)-dynamic -lX11 $(LOG) $(CFLAGS)

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
