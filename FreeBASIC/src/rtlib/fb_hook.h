/*
 *  libfb - FreeBASIC's runtime library
 *  Copyright (C) 2004-2006 Andre V. T. Vicentini (av1ctor@yahoo.com.br) and
 *  the FreeBASIC development team.
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
 *
 *  As a special exception, the copyright holders of this library give
 *  you permission to link this library with independent modules to
 *  produce an executable, regardless of the license terms of these
 *  independent modules, and to copy and distribute the resulting
 *  executable under terms of your choice, provided that you also meet,
 *  for each linked independent module, the terms and conditions of the
 *  license of that module. An independent module is a module which is
 *  not derived from or based on this library. If you modify this library,
 *  you may extend this exception to your version of the library, but
 *  you are not obligated to do so. If you do not wish to do so, delete
 *  this exception statement from your version.
 */

#ifndef __FB_HOOK_H__
#define __FB_HOOK_H__

typedef FBSTRING   *(*FB_INKEYPROC)     ( void );
typedef int         (*FB_GETKEYPROC)    ( void );
typedef int         (*FB_KEYHITPROC)    ( void );

FBCALL FBSTRING    *fb_Inkey            ( void );
FBCALL int          fb_Getkey           ( void );
FBCALL int          fb_KeyHit           ( void );

typedef void        (*FB_CLSPROC)       ( int mode );

FBCALL void         fb_Cls              ( int mode );

typedef int         (*FB_COLORPROC)     ( int fc, int bc );

FBCALL int          fb_Color            ( int fc, int bc );

typedef int         (*FB_LOCATEPROC)    ( int row, int col, int cursor );

FBCALL int          fb_LocateEx         ( int row, int col, int cursor, int *current_pos );
FBCALL int          fb_Locate           ( int row, int col, int cursor );
FBCALL int          fb_LocateSub        ( int row, int col, int cursor );

typedef void        (*FB_VIEWUPDATEPROC)( void );

FBCALL void         fb_ViewUpdate       ( void );

typedef int         (*FB_WIDTHPROC)     ( int cols, int rows );

FBCALL int          fb_Width            ( int cols, int rows );
FBCALL int          fb_WidthDev         ( FBSTRING *dev, int width );
FBCALL int          fb_WidthFile        ( int fnum, int width );

typedef int         (*FB_GETXPROC)      ( void );
typedef int         (*FB_GETYPROC)      ( void );
typedef void        (*FB_GETXYPROC)     ( int *col, int *row );
typedef void        (*FB_GETSIZEPROC)   ( int *cols, int *rows );

FBCALL int          fb_Pos              ( int dummy );
FBCALL int          fb_GetX             ( void );
FBCALL int          fb_GetY             ( void );
FBCALL void         fb_GetXY            ( int *col, int *row );
FBCALL void         fb_GetSize          ( int *cols, int *rows );

typedef unsigned int (*FB_READXYPROC)   ( int col, int row, int colorflag );
FBCALL unsigned int fb_ReadXY           ( int col, int row, int colorflag );

typedef void        (*FB_PRINTBUFFPROC) ( const void *buffer, size_t len, int mask );
typedef void        (*FB_PRINTBUFFWPROC)( const FB_WCHAR *buffer, size_t len, int mask );

typedef char        *(*FB_READSTRPROC)  ( char *buffer, int len );
        char        *fb_ReadString      ( char *buffer, int len, FILE *f );

typedef int         (*FB_LINEINPUTPROC) ( FBSTRING *text, void *dst, int dst_len,
										  int fillrem, int addquestion, int addnewline );
typedef int         (*FB_LINEINPUTWPROC)( const FB_WCHAR *text, FB_WCHAR *dst,
										  int max_chars, int addquestion, int addnewline );
FBCALL int          fb_LineInput        ( FBSTRING *text, void *dst, int dst_len,
										  int fillrem, int addquestion, int addnewline );
FBCALL int          fb_LineInputWstr    ( const FB_WCHAR *text, FB_WCHAR *dst,
										  int max_chars, int addquestion, int addnewline );
	   int 			fb_ConsoleLineInput	( FBSTRING *text, void *dst, int dst_len,
	   									  int fillrem, int addquestion, int addnewline );
       int          fb_ConsoleLineInputWstr ( const FB_WCHAR *text, FB_WCHAR *dst,
       										  int max_chars, int addquestion,
       										  int addnewline );

FBCALL int          fb_Multikey         ( int scancode );
FBCALL int          fb_GetMouse         ( int *x, int *y, int *z, int *buttons );
FBCALL int          fb_SetMouse         ( int x, int y, int cursor );
typedef int         (*FB_MULTIKEYPROC)  ( int scancode );
typedef int         (*FB_GETMOUSEPROC)  ( int *x, int *y, int *z, int *buttons );
typedef int         (*FB_SETMOUSEPROC)  ( int x, int y, int cursor );

FBCALL int          fb_In               ( unsigned short port );
FBCALL int          fb_Out              ( unsigned short port, unsigned char value );
typedef int         (*FB_INPROC)        ( unsigned short port );
typedef int         (*FB_OUTPROC)       ( unsigned short port, unsigned char value );

FBCALL void         fb_Sleep            ( int msecs );
FBCALL void         fb_Delay            ( int msecs );
FBCALL int          fb_SleepEx          ( int msecs, int kind );
       void         fb_ConsoleSleep     ( int msecs );
typedef void        (*FB_SLEEPPROC)     ( int msecs );

FBCALL int 			fb_IsRedirected		( int is_input );
       int 			fb_ConsoleIsRedirected( int is_input );
typedef int         (*FB_ISREDIRPROC)  	( int is_input );

typedef struct _FB_HOOKSTB {
    FB_INKEYPROC    		inkeyproc;
    FB_GETKEYPROC   		getkeyproc;
    FB_KEYHITPROC   		keyhitproc;
    FB_CLSPROC      		clsproc;
    FB_COLORPROC    		colorproc;
    FB_LOCATEPROC   		locateproc;
    FB_WIDTHPROC    		widthproc;
    FB_GETXPROC     		getxproc;
    FB_GETYPROC     		getyproc;
    FB_GETXYPROC    		getxyproc;
    FB_GETSIZEPROC  		getsizeproc;
    FB_PRINTBUFFPROC 		printbuffproc;
    FB_PRINTBUFFWPROC 		printbuffwproc;
    FB_READSTRPROC  		readstrproc;
    FB_MULTIKEYPROC 		multikeyproc;
    FB_GETMOUSEPROC 		getmouseproc;
    FB_SETMOUSEPROC 		setmouseproc;
    FB_INPROC       		inproc;
    FB_OUTPROC      		outproc;
    FB_VIEWUPDATEPROC 		viewupdateproc;
    FB_LINEINPUTPROC 		lineinputproc;
    FB_LINEINPUTWPROC 		lineinputwproc;
    FB_READXYPROC   		readxyproc;
    FB_SLEEPPROC    		sleepproc;
    FB_ISREDIRPROC			isredirproc;
} FB_HOOKSTB;

extern FB_HOOKSTB   fb_hooks;

#endif /* __FB_HOOK_H__ */
