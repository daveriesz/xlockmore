#ifndef lint
static char sccsid[] = "@(#)hopalong.c 1.3 89/02/16";
#endif
/*-
 * hopalong.c - Real Plane Fractals from Sept 86 Scientific American
 *
 * Copyright (c) 1989 by Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * Comments and additions should be sent to the author:
 *
 *                     naughton@wind.Sun.COM
 *
 *                     Patrick J. Naughton
 *                     Window Systems Group, MS 14-40
 *                     Sun Microsystems, Inc.
 *                     2550 Garcia Ave
 *                     Mountain View, CA  94043
 *                     (415) 336-1080
 *
 * Revision History:
 * 31-Aug-88: Forked from xlock.c for modularity.
 */

#include <math.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static int centerx, centery;		/* center of the screen */
static double a, b, c, i, j;		/* hopalong parameters */
static int color;
static unsigned long pix = 0;
static Display *Dsp;
static Window Win;
static GContext Gc;
static XPoint *pointBuffer = 0;		/* pointer for XDrawPoints */
static int Npoints = 0;
static long startTime;
static int timeout;

long
seconds()
{
struct timeval foo;

    gettimeofday(&foo, (struct timezone *) 0);
    return(foo.tv_sec);
}

void
inithop(d, w, g, c, t, n, p, x, y, A, B, C)
Display *d;
Window w;
GContext g;
int c, t, n, p, x, y;
double A, B, C;
{
    i = j = 0.0;
    startTime = seconds();

    if ((pointBuffer) || (n != Npoints)) {
	if (pointBuffer) free(pointBuffer);
	pointBuffer = (XPoint *) malloc(n * sizeof(XPoint));
	Npoints = n;
    }

    Dsp = d;
    Win = w;
    Gc = g;
    color = c;
    timeout = t;
    if (p >= 0) pix = (unsigned long) p;
    centerx = x;
    centery = y;
    a = A;
    b = B;
    c = C;
    XClearWindow(Dsp, Win);
}


void
randomInithop(d, w, g, c, t, n)
Display *d;
Window w;
GContext g;
int c, t, n;
{
int range;
XWindowAttributes xgwa;
double A, B, C;
int x, y;

    XGetWindowAttributes(d, w, &xgwa);
    x = xgwa.width / 2;
    y = xgwa.height / 2;
    range = (int) sqrt((double)x*x+(double)y*y);
    A = random() % (range * 100) * (random()%2?-1.0:1.0) / 100.0;
    B = random() % (range * 100) * (random()%2?-1.0:1.0) / 100.0;
    C = random() % (range * 100) * (random()%2?-1.0:1.0) / 100.0;

    if (!(random()%3)) a /= 10.0;
    if (!(random()%2)) b /= 100.0;

    inithop(d, w, g, c, t, n, -1, x, y, A, B, C);
}


int
hopdone()
{
    return(seconds() - startTime > timeout);
}


void
hop()
{
register double oldj;
register int k = Npoints;
register XPoint *xp = pointBuffer;

    if (color) {
	XSetForeground(Dsp, Gc, pix++);
	pix %= 254;
    }
    while (k--) {
	oldj = j;
	j = a - i;
	i = oldj + (i<0?sqrt(fabs(b*i - c)):-sqrt(fabs(b*i - c)));
	xp->x = centerx + (int)(i+j);
	xp->y = centery - (int)(i-j);
	xp++;
    }
    XDrawPoints(Dsp, Win, Gc, pointBuffer, Npoints, CoordModeOrigin);
}
