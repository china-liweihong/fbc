/*
 *  libgfx2 - FreeBASIC's alternative gfx library
 *	Copyright (C) 2005 Angelo Mottola (a.mottola@libero.it)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * screen.c -- screen function and drivers handling
 *
 * chng: jan/2005 written [lillo]
 *
 */

#include "fb_gfx.h"

#define NUM_MODES		21


MODE *fb_mode = NULL;
void *(*fb_hMemCpy)(void *dest, const void *src, size_t size) = NULL;
void *(*fb_hMemSet)(void *dest, int value, size_t size) = NULL;
void (*fb_hPutPixel)(int x, int y, unsigned int color) = NULL;
unsigned int (*fb_hGetPixel)(int x, int y) = NULL;
void *(*fb_hPixelCpy)(void *dest, const void *src, size_t size) = NULL;
void *(*fb_hPixelSet)(void *dest, int color, size_t size) = NULL;
unsigned int *fb_color_conv_16to32 = NULL;



typedef struct MODEINFO
{
	int w;
	int h;
	int depth;
	int scanline_size;
	const PALETTE *palette;
	const FONT *font;
	int text_w;
	int text_h;
} MODEINFO;



static const MODEINFO mode_info[NUM_MODES] = {
 { 320, 200, 2, 1, &fb_palette_16,  &fb_font_8x8,   40, 25 },		/* CGA mode 1 */
 { 640, 200, 1, 2, &fb_palette_16,  &fb_font_8x8,   80, 25 },		/* CGA mode 2 */
 { -1 }, { -1}, { -1 }, { -1 },						/* Unsupported modes (3, 4, 5, 6) */
 { 320, 200, 4, 1, &fb_palette_16,  &fb_font_8x8,   40, 25 },		/* EGA mode 7 */
 { 640, 200, 4, 2, &fb_palette_16,  &fb_font_8x8,   80, 25 },		/* EGA mode 8 */
 { 640, 350, 4, 1, &fb_palette_64,  &fb_font_8x14,  80, 25 },		/* EGA mode 9 */
 { -1 },								/* Unsupported mode (10) */
 { 640, 480, 1, 1, &fb_palette_256, &fb_font_8x16,  80, 30 },		/* VGA mode 11 */
 { 640, 480, 4, 1, &fb_palette_256, &fb_font_8x16,  80, 30 },		/* VGA mode 12 */
 { 320, 200, 8, 1, &fb_palette_256, &fb_font_8x8,   40, 25 },		/* VGA mode 13 */

									/* New modes */
 { 320, 240, 8, 1, &fb_palette_256, &fb_font_8x8,   40, 30 },		/* 14: 320x240 */
 { 400, 300, 8, 1, &fb_palette_256, &fb_font_8x8,   50, 37 },		/* 15: 400x300 */
 { 512, 384, 8, 1, &fb_palette_256, &fb_font_8x16,  64, 24 },		/* 16: 512x384 */
 { 640, 400, 8, 1, &fb_palette_256, &fb_font_8x16,  80, 25 },		/* 17: 640x400 */
 { 640, 480, 8, 1, &fb_palette_256, &fb_font_8x16,  80, 30 },		/* 18: 640x480 */
 { 800, 600, 8, 1, &fb_palette_256, &fb_font_8x16, 100, 37 },		/* 19: 800x600 */
 {1024, 768, 8, 1, &fb_palette_256, &fb_font_8x16, 128, 48 },		/* 20: 1024x768 */
 {1280,1024, 8, 1, &fb_palette_256, &fb_font_8x16, 160, 64 },		/* 21: 1280x1024 */
};

static char window_title_buff[WINDOW_TITLE_SIZE] = "";
static char *window_title = NULL;


/*:::::*/
static void exit_proc(void)
{
	fb_GfxScreen(0, 0, 0, SCREEN_EXIT, 0);
}


/*:::::*/
static int set_mode(const MODEINFO *info, int mode, int depth, int num_pages, int refresh_rate, int flags)
{
	const GFXDRIVER *driver = NULL;
	int i, try;
	char *c, *driver_name;

	if (num_pages <= 0)
		num_pages = 1;

	if (fb_mode) {
		if (fb_mode->driver)
			fb_mode->driver->exit();
		if (fb_mode->page) {
			for (i = 0; i < fb_mode->num_pages; i++)
				free(fb_mode->page[i]);
			free(fb_mode->page);
		}
		if (fb_mode->line)
			free(fb_mode->line);
		if (fb_mode->device_palette)
			free(fb_mode->device_palette);
		if (fb_mode->palette)
			free(fb_mode->palette);
		if (fb_mode->color_association)
			free(fb_mode->color_association);
		if (fb_mode->dirty)
			free(fb_mode->dirty);
		if (fb_mode->key)
			free(fb_mode->key);
		free(fb_mode);
		if (fb_color_conv_16to32)
			free(fb_color_conv_16to32);
	}

	if (mode == 0) {
		memset(&fb_hooks, 0, sizeof(fb_hooks));
		fb_mode = NULL;
	}
	else {
		fb_hooks.inkeyproc = fb_GfxInkey;
		fb_hooks.getkeyproc = fb_GfxGetkey;
		fb_hooks.keyhitproc = fb_GfxKeyHit;
		fb_hooks.clsproc = fb_GfxClear;
		fb_hooks.colorproc = fb_GfxColor;
		fb_hooks.locateproc = fb_GfxLocate;
		fb_hooks.widthproc = fb_GfxWidth;
		fb_hooks.getxproc = fb_GfxGetX;
		fb_hooks.getyproc = fb_GfxGetY;
		fb_hooks.getxyproc = fb_GfxGetXY;
		fb_hooks.getsizeproc = fb_GfxGetSize;
		fb_hooks.printbuffproc = fb_GfxPrintBufferEx;
		fb_hooks.readstrproc = fb_GfxReadStr;
		fb_hooks.multikeyproc = fb_GfxMultikey;
		fb_hooks.getmouseproc = fb_GfxGetMouse;
		fb_hooks.setmouseproc = fb_GfxSetMouse;
		fb_mode = (MODE *)calloc(1, sizeof(MODE));
	}

	if (fb_mode) {
		fb_mode->mode_num = mode;
		fb_mode->w = info->w;
		fb_mode->h = info->h;
		fb_mode->depth = info->depth;
		if ((mode > 13) && ((depth == 8) || (depth == 15) || (depth == 16) || (depth == 24) || (depth == 32)))
			fb_mode->depth = depth;
		if (flags & DRIVER_OPENGL)
			fb_mode->depth = MAX(16, fb_mode->depth);
		fb_mode->default_palette = info->palette;
		fb_mode->scanline_size = info->scanline_size;
		fb_mode->font = (FONT *)info->font;

		switch (fb_mode->depth) {
			case 15:
			case 16:	fb_mode->color_mask = 0xFFFF; fb_mode->depth = 16; break;
			case 24:
			case 32:	fb_mode->color_mask = 0xFFFFFFFF; fb_mode->depth = 32; break;
			default:	fb_mode->color_mask = (1 << fb_mode->depth) - 1;
		}

		fb_mode->bpp = BYTES_PER_PIXEL(fb_mode->depth);
		fb_mode->pitch = fb_mode->target_pitch = fb_mode->w * fb_mode->bpp;
		fb_mode->page = (unsigned char **)malloc(sizeof(unsigned char *) * num_pages);
		for (i = 0; i < num_pages; i++)
			fb_mode->page[i] = (unsigned char *)calloc(1, (fb_mode->pitch * fb_mode->h));
		fb_mode->num_pages = num_pages;
		fb_mode->framebuffer = fb_mode->page[0];
		fb_mode->line = (unsigned char **)malloc(fb_mode->h * sizeof(unsigned char *));
		for (i = 0; i < fb_mode->h; i++)
			fb_mode->line[i] = fb_mode->framebuffer + (i * fb_mode->pitch);
		/* dirty lines array may be bigger than needed; this is to please the
		   gfx driver which is not aware of the scanline size */
		fb_mode->dirty = (char *)calloc(1, fb_mode->h * fb_mode->scanline_size);
		fb_mode->device_palette = (unsigned int *)calloc(1, sizeof(int) * 256);
		fb_mode->palette = (unsigned int *)calloc(1, sizeof(int) * 256);
		fb_mode->color_association = (unsigned char *)malloc(16);
		fb_mode->key = (char *)calloc(1, 128);
		fb_color_conv_16to32 = (unsigned int *)malloc(sizeof(int) * 512);

		fb_hSetupFuncs();
		fb_hSetupData();

		if (!window_title)
		{
			window_title = fb_hGetExeName( window_title_buff, WINDOW_TITLE_SIZE - 1 );
			if ((c = strrchr(window_title, '.')))
				*c = '\0';
		}

		driver_name = getenv("FBGFX");
		for (try = (driver_name ? 4 : 2); try; try--) {
			for (i = 0; fb_gfx_driver_list[i >> 1]; i++) {
				driver = fb_gfx_driver_list[i >> 1];
				if ((driver_name) && !(try & 0x1) && (strcasecmp(driver_name, driver->name)))
					continue;
				if (!driver->init(window_title, fb_mode->w, fb_mode->h * fb_mode->scanline_size, MAX(8, fb_mode->depth), (i & 0x1) ? 0 : refresh_rate, flags))
					break;
				driver->exit();
				driver = NULL;
			}
			if (driver)
				break;
			if (driver_name) {
				if (try == 2)
					flags ^= DRIVER_FULLSCREEN;
			}
			else
				flags ^= DRIVER_FULLSCREEN;
		}

		if (!driver) {
			exit_proc();
			return fb_ErrorSetNum(FB_RTERROR_ILLEGALFUNCTIONCALL);
		}
		fb_mode->driver = driver;
		fb_mode->driver_flags = flags;

		fb_GfxPalette(-1, 0, 0, 0);

		if (fb_mode->depth == 8)
			fb_mode->fg_color = 15;
		else
			fb_mode->fg_color = fb_mode->color_mask;

		fb_mode->view_w = fb_mode->w;
		fb_mode->view_h = fb_mode->max_h = fb_mode->h;
		fb_mode->text_w = info->text_w;
		fb_mode->text_h = info->text_h;

		atexit(exit_proc);
	}

	return FB_RTERROR_OK;
}


/*:::::*/
FBCALL int fb_GfxScreen(int mode, int depth, int num_pages, int flags, int refresh_rate)
{
    const MODEINFO *info = NULL;
    int res;

	if ((mode < 0) || (mode > NUM_MODES))
		return fb_ErrorSetNum(FB_RTERROR_ILLEGALFUNCTIONCALL);

	if (mode > 0) {
		info = &mode_info[mode - 1];
		if (info->w < 0)
			return fb_ErrorSetNum(FB_RTERROR_ILLEGALFUNCTIONCALL);
	}
    res = set_mode(info, mode, depth, num_pages, refresh_rate, flags);
    if( res==FB_RTERROR_OK )
        FB_HANDLE_SCREEN->line_length = 0;
    return res;
}


/*:::::*/
FBCALL int fb_GfxScreenRes(int w, int h, int depth, int num_pages, int flags, int refresh_rate)
{
    MODEINFO info;
    int res;

	if ((w <= 0) || (h <= 0))
		return fb_ErrorSetNum(FB_RTERROR_ILLEGALFUNCTIONCALL);
	if ((depth != 8) && (depth != 15) && (depth != 16) && (depth != 24) && (depth != 32))
		return fb_ErrorSetNum(FB_RTERROR_ILLEGALFUNCTIONCALL);
	info.w = w;
	info.h = h;
	info.depth = depth;
	info.scanline_size = 1;
	info.palette = &fb_palette_256;
	info.font = &fb_font_8x8;
	info.text_w = w / 8;
	info.text_h = h / 8;

    res = set_mode((const MODEINFO *)&info, -1, depth, num_pages, refresh_rate, flags);
    if( res==FB_RTERROR_OK )
        FB_HANDLE_SCREEN->line_length = 0;
    return res;
}


/*:::::*/
FBCALL void fb_GfxSetWindowTitle(FBSTRING *title)
{
	strncpy(window_title_buff, title->data, WINDOW_TITLE_SIZE - 1);
	window_title = window_title_buff;

	if ((fb_mode) && (fb_mode->driver->set_window_title))
		fb_mode->driver->set_window_title(window_title);

	/* del if temp */
	fb_hStrDelTemp( title );
}
