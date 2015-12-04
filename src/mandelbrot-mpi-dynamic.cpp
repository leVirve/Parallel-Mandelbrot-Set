#include <mpi.h>
#include "utils.h"
#include "display.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int world_size, job_width, data_size, rank_num, *result;
const int MASTER = 0;
enum tag {RESULT, DATA, TERMINATE};

void gui_draw(int col, int* color)
{
    for (int i = 0, x = col; i < job_width; ++i, ++x)
        for (int j = 0; j < height; ++j)
            draw_point(x, j, color[j * job_width + i]);
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

void master()
{
    MPI_Status stat;
    int actives = 1, jobs = 0;
    if (gui) create_display(0, 0, height, width);
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
    if (gui) { flush(); sleep(3); }
}

void slave()
{
    MPI_Status stat;
    ComplexNum c;
    int col, *color = result + 1;
    Timer timer;
    timer.start();
    MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    while (stat.MPI_TAG == DATA) {
        for (int i = 0, x = col; i < job_width && x < width; ++i, ++x) {
            c.real = x * dx + real_min;
            for (int y = 0; y < height; y++) {
                c.imag = y * dy + imag_min;
                color[y * job_width + i] = calc_pixel(c);
            }
        }
        result[0] = col;
        MPI_Send(result, data_size, MPI_INT, MASTER, RESULT, MPI_COMM_WORLD);
        MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    }
    cout << "#" << rank_num << " runs in " << (double)(timer.stop()) / 1000 << " us" << endl;
}

void initial_MPI_env(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_num);

    job_width = 10;
    data_size = job_width * height + 1;
    result = new int[data_size];
}

void start()
{
    rank_num == MASTER ? master() : slave();
    MPI_Finalize();
}

int main(int argc, char** argv) {
    try {
        initial_env(argc, argv);
        initial_MPI_env(argc, argv);
        start();
    } catch (char const* err) {
        cerr << err << endl;
    }
    delete [] result;
    return 0;
}
