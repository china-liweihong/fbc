/* print using function */

#include <stdlib.h>
#include <string.h>
#include "fb.h"

FBCALL int fb_PrintUsingInit( FBSTRING *fmtstr );

/*:::::*/
FBCALL int fb_LPrintUsingInit( FBSTRING *fmtstr )
{
    int res = fb_LPrintInit();
    if( res!=FB_RTERROR_OK )
        return res;
	return fb_PrintUsingInit( fmtstr );
}

