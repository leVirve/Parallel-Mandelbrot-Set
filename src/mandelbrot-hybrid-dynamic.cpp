#include <mpi.h>
#include <omp.h>
#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;
int world_size, job_width, data_size, rank_num, *result;

#ifdef _LODE_BALANCE_ANALYSIS_
    double timer[12][12];
#endif

void _worker(int start, int* result)
{
    int *color = result + 1;
    ComplexNum c;
    #pragma omp parallel for schedule(dynamic, 10) private(c) collapse(2)
    for (int i = 0; i < job_width; ++i) {
        for (int y = 0; y < height; y++) {
        #ifdef _LODE_BALANCE_ANALYSIS_
            double s = omp_get_wtime();
        #endif
            c.real = (i + start) * dx + real_min;
            c.imag = y * dy + imag_min;
            color[y * job_width + i] = calc_pixel(c);
        #ifdef _LODE_BALANCE_ANALYSIS_
            timer[rank_num][omp_get_thread_num()] += omp_get_wtime() - s;
        #endif
        }
    }
    result[0] = start;
}

void master()
{
    if (gui) create_display(0, 0, height, width);
    if (world_size == 1) {
        _worker(MASTER, result);
        if (gui) { gui_draw(result[0], result + 1); flush(); }
        return;
    }

    MPI_Status stat;
    int actives = 1, jobs = 0;
    for (; actives < world_size && jobs < width; actives++, jobs += job_width)
        MPI_Send(&jobs, 1, MPI_INT, actives, DATA, MPI_COMM_WORLD);
    do {
        MPI_Recv(result, data_size, MPI_INT, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &stat);
        int slave = stat.MPI_SOURCE, col = result[0], *color = result + 1;
        actives--;
        if (jobs < width) {
            MPI_Send(&jobs, 1, MPI_INT, slave, DATA, MPI_COMM_WORLD);
            jobs += job_width, actives++;
        } else MPI_Send(&jobs, 1, MPI_INT, slave, TERMINATE, MPI_COMM_WORLD);
        if (gui) gui_draw(col, color);
    } while (actives > 1);
    if (gui) flush();
}

void slave()
{
    MPI_Status stat;
    unsigned int col;
    MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    while (stat.MPI_TAG == DATA) {
        _worker(col, result);
        MPI_Send(result, data_size, MPI_INT, MASTER, RESULT, MPI_COMM_WORLD);
        MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    }
}

void initial_MPI_env(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_num);

    job_width = world_size == 1 ? width : 20;
    data_size = job_width * height + 1;
    result = new int[data_size];
}

void start()
{
    rank_num == MASTER ? master() : slave();
}

int main(int argc, char** argv) {
    try {
        initial_env(argc, argv);
        omp_set_num_threads(num_thread);
        initial_MPI_env(argc, argv);
        double s = omp_get_wtime();
        start();
    #ifdef _LODE_BALANCE_ANALYSIS_
        for (int j = 0; j < num_thread; ++j)
            cout << timer[rank_num][j] << endl;
    #else
        cout << fixed << rank_num << "\t" << omp_get_wtime() - s << endl;
    #endif
    } catch (char const* err) {
        cerr << err << endl;
    }
    MPI_Finalize();
    delete [] result;
    return 0;
}
