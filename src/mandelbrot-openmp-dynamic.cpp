#include <omp.h>
#include "mandelbrot_utils.h"
using namespace std;

int num_thread, width, height;
double dx, dy, real_min, imag_min;

void calc()
{
    ComplexNum c;
#ifdef _LODE_BALANCE_ANALYSIS_
    double timer[num_thread];
#else
    double s = omp_get_wtime();
#endif
    #pragma omp parallel for schedule(dynamic, 10) private(c) collapse(2)
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
        #ifdef _LODE_BALANCE_ANALYSIS_
            double s = omp_get_wtime();
        #endif
            c.real = i * dx + real_min;
            c.imag = j * dy + imag_min;
            int repeats = calc_pixel(c);
        #ifdef _LODE_BALANCE_ANALYSIS_
            timer[omp_get_thread_num()] += omp_get_wtime() - s;
        #endif
            #pragma omp critical
            if (gui) draw_point(i, j, repeats);
        }
    }
#ifdef _LODE_BALANCE_ANALYSIS_
    for (int i = 0; i < num_thread; ++i)
        cout << fixed << "#" << i << ": " << timer[i] << endl;
#else
    cout << omp_get_wtime() - s << endl;
#endif
    if (gui) flush();
}

int main(int argc, char** argv) {
    int x = 0, y = 0;
    try {
        initial_env(argc, argv);
        omp_set_num_threads(num_thread);
        create_display(x, y, height, width);
        calc();
    } catch (char const* err) {
        cerr << err << endl;
    }
    return 0;
}
