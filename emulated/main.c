#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
/*Linux users will need to add -ldl to the Makefile to compile 
*this example.
*/
Display *dis;
Window win;
XEvent report;
GC green_gc;
XColor green_col;
Colormap colormap;
/*
Try changing the green[] = below to a different color.
The color can also be from /usr/X11R6/lib/X11/rgb.txt, such as RoyalBlue4.
A # (number sign) is only needed when using hexadecimal colors.
*/
char green[] = "#00FF00";

int main() {
	dis = XOpenDisplay(NULL);
	win = XCreateSimpleWindow(dis, RootWindow(dis, 0), 1, 1, 500, 500, 0, BlackPixel (dis, 0), BlackPixel(dis, 0));
	XMapWindow(dis, win);
	colormap = DefaultColormap(dis, 0);
	green_gc = XCreateGC(dis, win, 0, 0);
	XParseColor(dis, colormap, green, &green_col);
	XAllocColor(dis, colormap, &green_col);
	XSetForeground(dis, green_gc, green_col.pixel);
	
	int x;
	
	XSelectInput(dis, win, KeyPressMask |  KeyReleaseMask);
	
	XDrawRectangle(dis, win, green_gc, 1, 1, 497, 497);
	XDrawRectangle(dis, win, green_gc, 50, 50, 398, 398);
	XFlush(dis);
	
	fprintf(stdout,"run\n");
	
	int a[100000],i;
	for(i=0;i<100000;i++)
		a[i]=-1;
	a[113]=1;
	a[119]=2;
	a[101]=3;
	a[114]=4;
	a[116]=5;
	a[121]=6;
	a[117]=7;
	a[105]=8;
	a[111]=9;
	
	int msec=15000;
	
	while (1)  {
		XNextEvent(dis, &report);
		char r;
		if(report.type==KeyPress) r='p';
		if(report.type==KeyRelease) r='r';
		
		x=XLookupKeysym(&report.xkey, 0);
		if((x>=65470)&&(x<=65481))
			fprintf(stdout,"%cf5%d\n",r,x-65469);
		else if((x>=49)&&(x<=56))
			fprintf(stdout,"%cf4%d\n",r,x-48);
		else if(x==65293)
			fprintf(stdout,"%cs5\n",r);
		else if(x==65506)
			fprintf(stdout,"%cs4\n",r);
		
		
		
		else if(x==98)
			fprintf(stdout,"%cs0\n",r);
		else if(x==110)
			fprintf(stdout,"%cs1\n",r);
		else if(x==109)
			fprintf(stdout,"%cs2\n",r);
		else if(x==44)
			fprintf(stdout,"%cs3\n",r);
		else if(x==46)
			fprintf(stdout,"%cs4\n",r);
		else if(x==47)
			fprintf(stdout,"%cs5\n",r);
		
		
		
		else if(x==92) //strum
		{
			fprintf(stdout,"%cs0\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs1\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs2\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs3\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs4\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs5\n",r); usleep(msec);
		}
		else if(x==65508) //power
		{
			fprintf(stdout,"%cs0\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs1\n",r); usleep(msec);
			fflush(stdout);
			fprintf(stdout,"%cs2\n",r); usleep(msec);
		}
		else if(x==65365) //buttons
		{
			fprintf(stdout,"%cb0\n",r);
		}
		else if(x==65366) //buttons
		{
			fprintf(stdout,"%cb1\n",r);
		}
		else
		{
			if((x<100000)&&(a[x]!=-1))
			{
				fprintf(stdout,"%cf0%d\n",r,a[x]);
				fprintf(stdout,"%cf1%d\n",r,a[x]);
				fprintf(stdout,"%cf2%d\n",r,a[x]);
			}
			else
				fprintf(stdout,"%co%d\n",r,x);
 		}
		fflush(stdout);
	}
	
	return 0;
}
