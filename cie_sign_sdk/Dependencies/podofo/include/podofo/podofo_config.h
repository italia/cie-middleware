/* Template filled out by CMake */

/*
 * *** THIS HEADER IS INCLUDED BY PdfCompilerCompat.h ***
 * *** DO NOT INCLUDE DIRECTLY ***
 */
#ifndef _PDF_COMPILERCOMPAT_H
#error Please include PdfDefines.h instead
#endif

#define PODOFO_VERSION_MAJOR 0
#define PODOFO_VERSION_MINOR 9
#define PODOFO_VERSION_PATCH 6

/* PoDoFo configuration options */
#define PODOFO_MULTI_THREAD

/* somewhat platform-specific headers */
#define PODOFO_HAVE_STRINGS_H 1
#define PODOFO_HAVE_ARPA_INET_H 1
#define PODOFO_HAVE_WINSOCK2_H 0
#define PODOFO_HAVE_MEM_H 0
#define PODOFO_HAVE_CTYPE_H 1

/* Integer types - headers */
#define PODOFO_HAVE_STDINT_H 1
#define PODOFO_HAVE_BASETSD_H 0
#define PODOFO_HAVE_SYS_TYPES_H 1

/* Integer types - type names */
#define PDF_INT8_TYPENAME   char
#define PDF_INT16_TYPENAME  short
#define PDF_INT32_TYPENAME  int
#define PDF_INT64_TYPENAME  long
#define PDF_UINT8_TYPENAME  unsigned char
#define PDF_UINT16_TYPENAME unsigned short
#define PDF_UINT32_TYPENAME unsigned int
#define PDF_UINT64_TYPENAME unsigned long

/* Endianness */
//#cmakedefine TEST_BIG

/* Libraries */
//#define PODOFO_HAVE_JPEG_LIB
#define PODOFO_HAVE_PNG_LIB
//#define PODOFO_HAVE_TIFF_LIB
#define PODOFO_HAVE_FONTCONFIG
//#cmakedefine PODOFO_HAVE_LUA
//#cmakedefine PODOFO_HAVE_BOOST
//#cmakedefine PODOFO_HAVE_CPPUNIT

/* Platform quirks */
//#cmakedefine PODOFO_JPEG_RUNTIME_COMPATIBLE
