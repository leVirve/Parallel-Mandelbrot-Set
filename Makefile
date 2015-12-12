CFLAGS = -O3 -std=c++11 -D_LODE_BALANCE_ANALYSIS_0
MPI = -D_MPI_SUPPORT_
EXE = ms_*
OUTPUT = $(EXE) output/* errors/*
header = src/display.cpp src/utils.cpp src/mandelbrot_utils.cpp

all: clean mp mpi hybrid

seq:
	$(eval TARGET := ms_seq)
	g++ src/mandelbrot-seq.cpp $(header) -o $(TARGET) -lX11 $(LOG) $(CFLAGS)
mp:
	$(eval TARGET := ms_openmp)
	g++ src/mandelbrot-openmp-static.cpp $(header) -o $(TARGET)-static -lX11 -fopenmp $(LOG) $(CFLAGS)
	g++ src/mandelbrot-openmp-dynamic.cpp $(header) -o $(TARGET)-dynamic -lX11 -fopenmp $(LOG) $(CFLAGS)
mpi:
	$(eval TARGET := ms_mpi)
	mpic++ src/mandelbrot-mpi-static.cpp $(header) -o $(TARGET)-static -lX11 $(LOG) $(CFLAGS) $(MPI)
	mpic++ src/mandelbrot-mpi-dynamic.cpp $(header) -o $(TARGET)-dynamic -lX11 $(LOG) $(CFLAGS) $(MPI)
hybrid:
	$(eval TARGET := ms_hybrid)
	mpic++ src/mandelbrot-hybrid-static.cpp $(header) -o $(TARGET)-static -lX11 -fopenmp $(LOG) $(CFLAGS) $(MPI)
	mpic++ src/mandelbrot-hybrid-dynamic.cpp $(header) -o $(TARGET)-dynamic -lX11 -fopenmp $(LOG) $(CFLAGS) $(MPI)

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
