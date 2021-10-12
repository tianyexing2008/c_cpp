
#ifndef __INFRA_DEFS_H__
#define __INFRA_DEFS_H__

#include "Types.h"
#include <stddef.h>


// WIN32 Dynamic Link Library
#ifdef _MSC_VER

#ifdef INFRA_DLL_BUILD
#define  INFRA_API _declspec(dllexport)
#elif defined INFRA_DLL_USE
#define  INFRA_API _declspec(dllimport)
#else
#define INFRA_API
#endif

#else

#define INFRA_API

#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef __GNUC__
#define __attribute__(x)
#endif

//////////////////////////////////////////////////////////////////////////
// useful definition

#ifndef MAX_PATH
#define MAX_PATH				(260)
#endif

#ifndef BITMSK
#define BITMSK(bit)				(int)(1 << (bit))
#endif
//////////////////////////////////////////////////////////////////////////
// Join two variables

#define MACRO_JOIN( X, Y ) MACRO_DO_JOIN( X, Y )
#define MACRO_DO_JOIN( X, Y ) MACRO_DO_JOIN2(X,Y)
#define MACRO_DO_JOIN2( X, Y ) X##Y


//////////////////////////////////////////////////////////////////////////
// use the unified 'DEBUG' macro

#if (!defined(NDEBUG)) && !defined(DEBUG)
#	define DEBUG
#endif



//////////////////////////////////////////////////////////////////////////
// 以下为组件化旧接口，非组件化代码请勿使用

/* Windows specific #includes and #defines */
#ifdef _MSC_VER
	#define	snprintf		_snprintf
#endif


#endif //__DEFS_H__
