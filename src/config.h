#ifndef G2_CONFIG_H
#define G2_CONFIG_H

// core
//#define G2_CONFIG_CORE_GL
#define G2_CONFIG_CORE_SKIA
//#define G2_CONFIG_CORE_SOFT

// gl
#define G2_CONFIG_GL_VERSION 		(0x0)
#define G2_CONFIG_GL_APICALL 		__tb_cdecl__

// float
#define G2_CONFIG_FLOAT_FIXED

// size_t for skia new
#define G2_CONFIG_SIZE_T 			tb_ulong_t

#endif
// config
#ifndef gbox2_AUTO_CONFIG_H
#define gbox2_AUTO_CONFIG_H
#define gbox2_CONFIG_DEBUG 1
#endif
