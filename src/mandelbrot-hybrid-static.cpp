#include <mpi.h>
#include <omp.h>
#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int world_size, job_width, data_size, rank_num, *result, *results;

#ifdef _LODE_BALANCE_ANALYSIS_
    double timer[12][12];
#endif

void start(int sz)
{
    ComplexNum c;
    #pragma omp parallel for schedule(static) private(c) collapse(2)
    for (int i = 0; i < sz; ++i) {
        for (int j = 0; j < height; j++) {
        #ifdef _LODE_BALANCE_ANALYSIS_
            double s = omp_get_wtime();
        #endif
            c.real = (i + rank_num * sz) * dx + real_min;
            c.imag = j * dy + imag_min;
            result[j * sz + i] = calc_pixel(c);
        #ifdef _LODE_BALANCE_ANALYSIS_
            timer[rank_num][omp_get_thread_num()] += omp_get_wtime() - s;
        #endif
        }
    }
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
}

int main(int argc, char** argv) {
    try {
        initial_env(argc, argv);
        initial_MPI_env(argc, argv);
        omp_set_num_threads(num_thread);
        double s = omp_get_wtime();
        start(job_width);
    #ifdef _LODE_BALANCE_ANALYSIS_
        for (int j = 0; j < num_thread; ++j)
            cout << timer[rank_num][j] << endl;
    #else
        cout << fixed << rank_num << "\t" << omp_get_wtime() - s << endl;
    #endif
        collect_results();
        if (rank_num == 0 && gui) gui_display(results);
    } catch (char const* err) {
        cerr << err << endl;
    }
    MPI_Finalize();
    delete [] results;
    delete [] result;
    return 0;
}
