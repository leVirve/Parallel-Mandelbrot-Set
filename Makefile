CFLAGS = -O3 -std=c++11
EXE = ms_seq ms_openmp
OUTPUT = $(EXE)
header = src/display.cpp src/utils.cpp

all: clean mp

seq:
	$(eval TARGET := ms_seq)
	g++ src/mandelbort-seq.cpp $(header) -o $(TARGET) -lX11 $(LOG) $(CFLAGS)
mp:
	$(eval TARGET := ms_openmp)
	g++ src/mandelbort-openmp.cpp $(header) -o $(TARGET) -lX11 -fopenmp $(LOG) $(CFLAGS)

clean:
	-@rm $(OUTPUT) 2>/dev/null || true
