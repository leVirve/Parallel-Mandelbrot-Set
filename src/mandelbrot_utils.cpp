#include "mandelbrot_utils.h"

#ifdef _MPI_SUPPORT_

void gui_display(int* results)
{
    create_display(0, 0, height, width);
    for (int r = 0; r < world_size; r++)
        for (int i = 0, x = r * job_width; i < job_width; ++i, ++x)
            for (int j = 0; j < height; ++j)
                draw_point(x, j, results[r * data_size + j * job_width + i]);
    flush();
}

void gui_draw(int col, int* color)
{
    for (int i = 0, x = col; i < job_width; ++i, ++x)
        for (int j = 0; j < height; ++j)
            draw_point(x, j, color[j * job_width + i]);
}

void _worker(int start, int* result)
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

#endif
