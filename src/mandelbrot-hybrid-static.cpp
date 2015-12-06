#include <mpi.h>
#include <omp.h>
#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int world_size, job_width, data_size, rank_num, *result, *results;

void start(int sz)
{
    ComplexNum c;
    Timer timer;
    timer.start();
    #pragma omp parallel for schedule(static) private(c) collapse(2)
    for (int i = 0; i < sz; ++i) {
        for (int j = 0; j < height; j++) {
            c.real = (i + rank_num * sz) * dx + real_min;
            c.imag = j * dy + imag_min;
            result[j * sz + i] = calc_pixel(c);
        }
    }
    cout << "#" << rank_num << " runs in " << (double)(timer.stop()) / 1000 << " us" << endl;
}

void initial_MPI_env(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_num);

    job_width = width / world_size;
    if (width % world_size) job_width += 1;
    data_size = job_width * height;
    result = new int [data_size];
}

void collect_results()
{
    if (rank_num == 0) results = new int [world_size * data_size];
    MPI_Gather(result, data_size, MPI_INT, results, data_size, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Finalize();
}

int main(int argc, char** argv) {
    try {
        initial_env(argc, argv);
        initial_MPI_env(argc, argv);
        omp_set_num_threads(num_thread);
        start(job_width);
        collect_results();
        if (rank_num == 0 && gui) gui_display(results);
    } catch (char const* err) {
        cerr << err << endl;
    }
    delete [] results;
    delete [] result;
    return 0;
}
