#include "display.h"

Display *display;
Window window;
GC gc;
bool gui;
int screen, blackcolor, whitecolor;

void create_display(int x, int y, int height, int width) {
    if (!gui) return;
    int border_width = 0;
    long valuemask = 0;
    XGCValues values;

    display = XOpenDisplay(NULL);
    if (!display) throw "cannot open display";
    screen = DefaultScreen(display);
    blackcolor = BlackPixel(display, screen);
    whitecolor = WhitePixel(display, screen);
    window = XCreateSimpleWindow(
                display, RootWindow(display, screen),
                x, y, width, height, border_width, blackcolor, whitecolor
             );
    gc = XCreateGC(display, window, valuemask, &values);
    XSetForeground (display, gc, blackcolor);
    XSetBackground(display, gc, 0X0000FF00);
    XSetLineAttributes (display, gc, 1, LineSolid, CapRound, JoinRound);

    XMapWindow(display, window);
    XSync(display, 0);
}

void draw_point(int x, int y, int color) {
    XSetForeground (display, gc,  1024 * 1024 * (color % 256));
    XDrawPoint (display, window, gc, x, y);
}

void flush() {
    XFlush(display);
}
