#include <mpi.h>
#include "utils.h"
#include "display.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

int world_size, data_size, rank_num;
const int MASTER = 0;
enum tag {ANY, RESULT, DATA, TERMINATE};

int calc_pixel(ComplexNum& c)
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
    int raw[data_size];
    create_display(0, 0, height, width);
    for (; actives < world_size; actives++, jobs++)
        MPI_Send(&jobs, 1, MPI_INT, actives, DATA, MPI_COMM_WORLD);
    int ccc = 0;
    do {
        MPI_Recv(raw, data_size, MPI_INT, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &stat);
        ccc++;
        int slave = stat.MPI_SOURCE, col = raw[0], *color = raw + 1;
        actives--;
        if (jobs < width) {
            MPI_Send(&jobs, 1, MPI_INT, slave, DATA, MPI_COMM_WORLD);
            jobs++;
            actives++;
        } else MPI_Send(&jobs, 1, MPI_INT, slave, TERMINATE, MPI_COMM_WORLD);
        if (gui) for (int i = 0; i < height; ++i) draw_point(col, i, color[i]);
    } while (actives > 1);
    INFO("fin counts: " << ccc);
    flush();
    sleep(3);
}

void slave()
{
    MPI_Status stat;
    ComplexNum c;
    int col, result[data_size], *color = result + 1;
    MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    while (stat.MPI_TAG == DATA) {
        c.real = col * dx + real_min;
        for (int y = 0; y < height; y++) {
            c.imag = y * dy + imag_min;
            color[y] = calc_pixel(c);
        }
        result[0] = col;
        MPI_Send(result, data_size, MPI_INT, MASTER, RESULT, MPI_COMM_WORLD);
        MPI_Recv(&col, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    }
}

void initial_MPI_env(int argc, char** argv)
{
    data_size = height + 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_num);

    if (rank_num == MASTER) master();
    else slave();
    MPI_Finalize();
}

int main(int argc, char** argv) {
    try {
        initial_env(argc, argv);
        initial_MPI_env(argc, argv);
    } catch (char const* err) {
        cerr << err << endl;
    }
    return 0;
}
