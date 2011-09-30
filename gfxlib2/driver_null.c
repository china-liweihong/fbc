/* null driver for non-displayed gfx handling */

#include "fb_gfx.h"

static void driver_dummy(void);


const GFXDRIVER __fb_gfxDriverNull =
{
	"Null",			/* char *name; */
	NULL,			/* int (*init)(int w, int h, char *title, int fullscreen); */
	NULL,			/* void (*exit)(void); */
	driver_dummy,	/* void (*lock)(void); */
	driver_dummy,	/* void (*unlock)(void); */
	NULL,			/* void (*set_palette)(int index, int r, int g, int b); */
	NULL,			/* void (*wait_vsync)(void); */
	NULL,			/* int (*get_mouse)(int *x, int *y, int *z, int *buttons); */
	NULL,			/* void (*set_mouse)(int x, int y, int cursor); */
	NULL,			/* void (*set_window_title)(char *title); */
	NULL,			/* int (*set_window_pos)(int x, int y); */
	NULL,			/* int *(*fetch_modes)(int depth, int *size); */
	NULL			/* void (*flip)(void); */
};


/*:::::*/
static void driver_dummy(void)
{
}

