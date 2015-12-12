#include <mpi.h>
#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;
Timer timer;
int world_size, job_width, data_size, rank_num, *result, ccc[12];

void inline _worker(int start, int* result)
{
    int *color = result + 1;
    ComplexNum c;
    for (int i = 0, x = start; i < job_width && x < width; ++i, ++x) {
        c.real = x * dx + real_min;
        for (int y = 0; y < height; y++) {
            c.imag = y * dy + imag_min;
            color[y * job_width + i] = calc_pixel(c);
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
        ccc[rank_num] += job_width * height;
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
    double s = MPI_Wtime();
    rank_num == MASTER ? master() : slave();
    cout << fixed << rank_num << ": " << MPI_Wtime() - s << endl;
    cout << "#" << rank_num << ", load= " << ccc[rank_num] << endl;
    MPI_Finalize();
}

int main(int argc, char** argv) {
    try {
        for (int i = 0; i < 12; ++i) ccc[i] = 0;
        initial_env(argc, argv);
        initial_MPI_env(argc, argv);
        start();
    } catch (char const* err) {
        cerr << err << endl;
    }
    delete [] result;
    return 0;
}
