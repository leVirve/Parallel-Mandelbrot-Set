#include <mpi.h>
#include <omp.h>
#include "utils.h"
#include "display.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int world_size, job_width, data_size, rank_num;
int *result, *results;
const int MASTER = 0;

void gui_display()
{
    create_display(0, 0, height, width);
    for (int r = 0; r < world_size; r++)
        for (int i = 0, x = r * job_width; i < job_width; ++i, ++x)
            for (int j = 0; j < height; ++j)
                draw_point(x, j, results[r * data_size + j * job_width + i]);
    flush();
    sleep(2);
}

inline int calc_pixel(ComplexNum& c)
{
    int repeats = 0;
    double lengthsq = 0.0;
    ComplexNum z = {0.0, 0.0};
    while (repeats < 100000 && lengthsq < 4.0) {
        double temp = z.real * z.real - z.imag * z.imag + c.real;
        z.imag = 2 * z.real * z.imag + c.imag;
        z.real = temp;
        lengthsq = z.real * z.real + z.imag * z.imag;
        repeats++;
    }
    return repeats;
}

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
        if (rank_num == 0 && gui) gui_display();
    } catch (char const* err) {
        cerr << err << endl;
    }
    delete [] results;
    delete [] result;
    return 0;
}
