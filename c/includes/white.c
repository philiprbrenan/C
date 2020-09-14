#line 1 "/home/phil/c/x/tests/white.c"
// A simple Xlib program.
#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
 {Display *d = XOpenDisplay(NULL);
  int white = WhitePixel(d, DefaultScreen(d));
  Window w = XCreateSimpleWindow(d, DefaultRootWindow(d), 0, 0, 640, 480, 0, white, white);
  XMapWindow(d, w);
  XFlush(d);
  sleep(2);
  XCloseDisplay(d);
  return 0;
 }
