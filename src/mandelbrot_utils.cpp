#include "mandelbrot_utils.h"

void gui_display(int* results)
{
    create_display(0, 0, height, width);
    for (int r = 0; r < world_size; r++)
        for (int i = 0, x = r * job_width; i < job_width; ++i, ++x)
            for (int j = 0; j < height; ++j)
                draw_point(x, j, results[r * data_size + j * job_width + i]);
    flush();
    sleep(2);
}

void gui_draw(int col, int* color)
{
    for (int i = 0, x = col; i < job_width; ++i, ++x)
        for (int j = 0; j < height; ++j)
            draw_point(x, j, color[j * job_width + i]);
}
