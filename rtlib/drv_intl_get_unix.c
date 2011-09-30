/* get i18n data */

#include <stddef.h>
#include <langinfo.h>
#include "fb.h"

/*:::::*/
const char *fb_DrvIntlGet( eFbIntlIndex Index )
{
    switch ( Index ) {
    case eFIL_DateDivider:
        return "/";
    case eFIL_TimeDivider:
        return ":";
    case eFIL_NumDecimalPoint:
        return nl_langinfo( RADIXCHAR );
    case eFIL_NumThousandsSeparator:
        return nl_langinfo( THOUSEP );
    }
    return NULL;
}
