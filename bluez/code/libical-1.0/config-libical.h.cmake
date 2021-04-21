/* config.h.  Generated by cmake from config.h.cmake */

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <wctype.h> header file. */
#cmakedefine HAVE_WCTYPE_H 1

/* FIXME: 
   These are statically defined, as the configure check fails too often
   This prevents libical's snprintf() copy to be used, which is buggy and causes dbus 
   to crash in handle_error()
   In case there is a platform to be supported without working vsnprintf(),
     a) fix the configure check 
     b) fix libical's vsnprintf.c/provide a better snprintf() implementation 
*/
#define    HAVE_SNPRINTF 1
#define    HAVE_VSNPRINTF 1

/* Define to 1 if you have the `iswspace' function. */
#cmakedefine    HAVE_ISWSPACE 1

