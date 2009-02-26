/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


/****************************************************************
	xerxes engine
	vid_manager.cpp
 ****************************************************************/

#include "xerxes.h"
#include <assert.h>

/***************************** data *****************************/

bool vid_initd = false;
bool vid_window = true;
int vid_bpp, vid_xres, vid_yres;
int transColor;
image *screen = 0;
AuxWindow *gameWindow;

/****************************************************************/

void   (*Flip) (void);
void   (*Blit) (int x, int y, image *src, image *dest);
void   (*TBlit) (int x, int y, image *src, image *dest);
void   (*AdditiveBlit) (int x, int y, image *src, image *dest);
void   (*AlphaBlit) (int x, int y, image *src, image *alpha, image *dest);
void   (*TAdditiveBlit) (int x, int y, image *src, image *dest);
void   (*SubtractiveBlit) (int x, int y, image *src, image *dest);
void   (*TSubtractiveBlit) (int x, int y, image *src, image *dest);
void   (*BlitTile) (int x, int y, quad *src, image *dest);
void   (*TBlitTile) (int x, int y, quad *src, image *dest);
void   (*PutPixel) (int x, int y, int color, image *dest);
void   (*Line) (int x, int y, int xe, int ye, int color, image *dest);
void   (*VLine) (int x, int y, int ye, int color, image *dest);
void   (*HLine) (int x, int y, int xe, int color, image *dest);
void   (*Box) (int x, int y, int xe, int ye, int color, image *dest);
void   (*DrawRect) (int x, int y, int xe, int ye, int color, image *dest);
void   (*Sphere) (int x, int y, int xradius, int yradius, int color, image *dest);
void   (*Circle) (int x, int y, int xradius, int yradius, int color, image *dest);
void   (*ScaleBlit) (int x, int y, int dw, int dh, image *src, image *dest);
void   (*TScaleBlit) (int x, int y, int dw, int dh, image *src, image *dest);
void   (*WrapBlit) (int x, int y, image *src, image *dst);
void   (*TWrapBlit) (int x, int y, image *src, image *dst);
void   (*Silhouette) (int x, int y, int c, image *src, image *dst);
void   (*RotScale) (int x, int y, float angle, float scale, image *src, image *dest);
void   (*Mosaic) (int xf, int yf, image *src);
void   (*BlitWrap) (int x, int y, image *src, image *dest);
void   (*ColorFilter) (int filter, image *img);
void   (*Triangle) (int x1, int y1, int x2, int y2, int x3, int y3, int c, image *dest);
void   (*FlipBlit) (int x, int y, int fx, int fy, image *src, image *dest);
image* (*ImageFrom8bpp) (byte *src, int width, int height, byte *pal);
image* (*ImageFrom24bpp) (byte *src, int width, int height);
image* (*ImageFrom32bpp) (byte *src, int width, int height);
void   (*vid_Close) (void);
// Overkill 2006-02-04
void   (*RectVGrad) (int x, int y, int xe, int ye, int color, int color2, image *dest);
// Overkill 2006-02-04
void   (*RectHGrad) (int x, int y, int xe, int ye, int color, int color2, image *dest);
// janus 2006-07-22
void   (*RectRGrad) (int x1, int y1, int x2, int y2, int color1, int color2, image *dest);
// janus 2006-07-22
void   (*Rect4Grad) (int x1, int y1, int x2, int y2, int color1, int color2, int color3, int color4, image *dest);
// Overkill (2007-05-04)
int    (*HSVtoColor) (int h, int s, int v);
// Overkill (2007-05-04)
void    (*GetHSV) (int color, int &h, int &s, int &v);
// Overkill (2007-05-04)
void    (*HueReplace) (int hue_find, int hue_tolerance, int hue_replace, image *img);
// Overkill (2007-05-04)
void	(*ColorReplace) (int color_find, int color_replace, image *img);

AuxWindow * (*vid_createAuxWindow)(void);
AuxWindow * (*vid_findAuxWindow)(int handle);

/***************************** code *****************************/


int vid_SetMode(int xres, int yres, int bpp, int window, int mode)
{
	int (*doModeSet)(int xres, int yres, int bpp, bool windowflag);
#ifdef __IPHONE__
	doModeSet = iphone_SetMode;
#elif __APPLE__
	doModeSet = sdl_SetMode;
#elif __LINUX__
	doModeSet = sdl_SetMode;
//mbg 9/5/05 adding psp support
#elif __PSP__
	doModeSet = psp_SetMode;
#elif __WII__
	doModeSet = wii_SetMode;
#else
	doModeSet = dd_SetMode;
#endif

	switch (mode)
	{
		case MODE_SOFTWARE:
			if( doModeSet(xres, yres, bpp, window?true:false) )
			{
				vid_initd = true;
				return 1;
			}
			return 0;
		default:
			return 0;
	}
	return 0;
}

static int imagesize = 0;


image::image(int xres, int yres) {
	width = pitch = xres;
	height = yres;
	cx1 = 0;
	cy1 = 0;
	cx2 = width - 1;
	cy2 = height - 1;
	shell = 0;
	
	imagesize += width*height*4;
	//log("Allocating %d image bytes; now up to %dK",width*height*4,imagesize/1024);
	data = new quad[width*height];

	//we are going to assume that pixels are 4byte aligned.
	assert((((int)data) & 3) == 0);

	//we're running out of memory sometimes
	assert(data);
}

void image::delete_data() {
	imagesize -= width*height*4;
	//log("Freeing %d image bytes; now down to %dK",width*height*4,imagesize/1024);
	delete[] (quad*)data;
}

image::~image() {
	if (data && !shell)
		delete_data();
}