#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN_WIDTH   80
#define WIN_HEIGHT  24
#define APP_NAME    "kbtoggle"
#define APP_CLASS   "Kbtoggle"

static const int kb_ids[] = {10, 11, 12, 13, 19};
static const int kb_count = (int)(sizeof(kb_ids) / sizeof(kb_ids[0]));
static int kb_enabled = 1;

static void apply_state(Display *dpy, Window win,
                        unsigned long col_on, unsigned long col_off)
{
    const char *verb = kb_enabled ? "enable" : "disable";
    char buf[64];
    for (int i = 0; i < kb_count; i++) {
        snprintf(buf, sizeof(buf), "xinput %s %d", verb, kb_ids[i]);
        system(buf);
    }
    XSetWindowBackground(dpy, win, kb_enabled ? col_on : col_off);
    XClearWindow(dpy, win);
    /* Expose event will follow XClearWindow and redraw the label */
}

static void draw_label(Display *dpy, Window win, GC gc)
{
    const char *label = kb_enabled ? "KB ON" : "KB OFF";
    int len = (int)strlen(label);
    /* Rough centre for the default ~6px-wide font */
    XDrawString(dpy, win, gc,
                WIN_WIDTH / 2 - len * 3,
                WIN_HEIGHT / 2 + 4,
                label, len);
}

int main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fputs("kbtoggle: cannot open display\n", stderr);
        return 1;
    }

    int scr      = DefaultScreen(dpy);
    Colormap cmap = DefaultColormap(dpy, scr);
    XColor xc;
    unsigned long col_on, col_off;

    XParseColor(dpy, cmap, "#44aa44", &xc);
    XAllocColor(dpy, cmap, &xc);
    col_on = xc.pixel;

    XParseColor(dpy, cmap, "#aa4444", &xc);
    XAllocColor(dpy, cmap, &xc);
    col_off = xc.pixel;

    Window win = XCreateSimpleWindow(
        dpy, RootWindow(dpy, scr),
        0, 0, WIN_WIDTH, WIN_HEIGHT,
        0, BlackPixel(dpy, scr), col_on);

    /* Name the window so FvwmButtons can swallow it */
    XClassHint *ch = XAllocClassHint();
    ch->res_name  = APP_NAME;
    ch->res_class = APP_CLASS;
    XSetClassHint(dpy, win, ch);
    XFree(ch);
    XStoreName(dpy, win, APP_NAME);

    XSelectInput(dpy, win, ExposureMask | ButtonPressMask);

    GC gc = XCreateGC(dpy, win, 0, NULL);
    XSetForeground(dpy, gc, WhitePixel(dpy, scr));

    XMapWindow(dpy, win);
    XFlush(dpy);

    for (;;) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        switch (ev.type) {
        case ButtonPress:
            kb_enabled ^= 1;
            apply_state(dpy, win, col_on, col_off);
            break;
        case Expose:
            if (ev.xexpose.count == 0)
                draw_label(dpy, win, gc);
            break;
        }
    }

    XFreeGC(dpy, gc);
    XCloseDisplay(dpy);
    return 0;
}
