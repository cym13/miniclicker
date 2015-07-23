#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
    int x;
    int y;
} position;

int myatoi(const char *str, int *ret) {
    char *p;
    int   i;
    i = strtod(str, &p);
    if (p == str)
        return 0;
    *ret = i;
    return 1;
}

position getposition(Display* display, Window root) {
    int      tmp;
    position pos;
    Window   fromroot, tmpwin;

    XQueryPointer(display, root, &fromroot, &tmpwin,
                  &(pos.x), &(pos.y), &tmp, &tmp, (unsigned int*)&tmp);
    return pos;
}


void moveto(position pos, Display *display, Window root) {
    XWarpPointer(display, None, root, 0, 0, 0, 0, pos.x, pos.y);
    XFlush(display);
}


void click(Display *display, Window root) {
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xbutton.button = Button1;
    event.xbutton.same_screen = 1;
    event.xbutton.subwindow = DefaultRootWindow(display);

    while (event.xbutton.subwindow) {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer( display,
                       event.xbutton.window,
                      &event.xbutton.root,
                      &event.xbutton.subwindow,
                      &event.xbutton.x_root,
                      &event.xbutton.y_root,
                      &event.xbutton.x,
                      &event.xbutton.y,
                      &event.xbutton.state);
    }

    event.type = ButtonPress;
    XFlush(display);
    usleep(1);

    event.type = ButtonRelease;
    XFlush(display);
    usleep(1);
}


int main(int argc, char *argv[])
{
    Display* display;
    Window   root;
    position clickpos, current;

    int i;
    int limit     = 0;
    int sleep_dur = 20;

    if (argc != 2 && argc != 3) {
        fprintf(stderr,
                "Usage: %s NUM [DEL]\n"
                "Clicks where the pointer is at script launch\n"
                "    Args:\n"
                "         NUM   Number of clicks, infinite if 0\n"
                "         DEL   Delay between clicks (mlls)\n",
                argv[0]);
        return 1;
    }

    myatoi(argv[1], &limit);

    if (argc == 3) {
        myatoi(argv[2], &sleep_dur);
    }
    sleep_dur *= 1000;

    if ((display = XOpenDisplay(NULL)) == NULL) {
        fprintf(stderr, "Cannot open local X-display.\n");
        exit(1);
    }

    root = DefaultRootWindow(display);

    clickpos = getposition(display, root);

    printf("Position: x=%i, y=%i\n", clickpos.x, clickpos.y);

    for(i=0 ; limit==0 || i<limit; ++i) {
        current = getposition(display, root);
        moveto(clickpos, display, root);
        click(display, root);
        moveto(current, display, root);
        usleep(sleep_dur);
    }

    return 0;
}
