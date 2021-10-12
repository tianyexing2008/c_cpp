

#ifndef __DH_TYPES_H__
#define __DH_TYPES_H__
#ifndef DAHUA_GENERAL_TYPES_GUARD
#define DAHUA_GENERAL_TYPES_GUARD
//////////////////////////////////////////////////////////////////////////
// base types definition
// compilers should be restricted to ensure the following equalities!
typedef signed char				schar;	///< sizeof(uchar) == sizeof(schar) == sizeof(char) == 1
typedef unsigned char			uchar;
typedef unsigned int			uint;	///< sizeof(uint) == sizeof(int) == 4
typedef unsigned short			ushort;	///< sizeof(ushort) == sizeof(short) == 2
typedef unsigned long			ulong;	///< sizeof(ulong) == sizeof(long) == 4
typedef signed char				int8;
typedef unsigned char			uint8;
typedef signed short			int16;
typedef unsigned short			uint16;
typedef signed  int			int32;
typedef unsigned int		uint32;
#ifndef _WIN32
typedef unsigned int			DWORD;
typedef unsigned int			PARAM;
typedef int						BOOL;
#endif
#ifdef _MSC_VER
typedef __int64					int64;	///< sizeof(int64) == sizeof(uint64) == 8
typedef unsigned __int64		uint64;
#elif defined(__GNUC__)
typedef long long				int64;
typedef unsigned long long		uint64;
#elif defined(__TCS__)
typedef signed   long long int	int64;
typedef unsigned long long int	uint64;
#endif


/// 矩形
typedef struct Rect
{
	int left;
	int top;
	int right;
	int bottom;
} Rect;

/// 点
typedef struct Point
{
	int x;
	int y;
} Point;

/// 尺寸
typedef struct Size
{
	int w;
	int h;
} Size;

/// 颜色
typedef struct Color
{
	uchar r;
	uchar g;
	uchar b;
	uchar a;
} Color;

/// 多边形类型
/// 因Windows的wingdi.h存在名为Polygon的函数，导致编译冲突。加宏保护。
#ifndef _WIN32
typedef struct Polygon
{
	uint							pointNumber;///< 顶点数目，必须不小于3
	Point*							points;		///< 多边形区域的点
}Polygon;
#endif

/// 多边形类型，因Polygon在Windows平台冲突，后续多边形都使用此类型
typedef struct DH_PAL_Polygon
{
	uint							pointNumber;///< 顶点数目，必须不小于3
	Point*							points;		///< 多边形区域的点
}DH_PAL_Polygon;

#endif // DAHUA_GENERAL_TYPES_GUARD

/// 版本信息
typedef struct Version
{
	char	name[32];	///< 组件名称
	int		major;		///< 主版本号，架构变动时增加
	int		minor;		///< 次版本号，接口变化或严重缺陷修正时增加
	int		revision;	///< 修订版本号，缺陷修正时增加
	int		svn;		///< svn版本号，记录上述3版本号变更时对应的svn版本号
	char	date[32];	///< 编译日期，使用__DATE__宏
} Version;

#endif// __DH_TYPES_H__

#ifndef LINE_GUARD
#define LINE_GUARD
/// 直线
typedef struct Line
{
	Point start;
	Point end;
} Line;
#endif // LINE_GUARD


