/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		stdio.h
 * @ingroup 	libc
 *
 */
#ifndef TB_LIBC_STDIO_H
#define TB_LIBC_STDIO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// vsnprintf format
#define tb_vsnprintf_format(s, n, fmt, r) \
do \
{ \
	tb_long_t __tb_ret = 0; \
	tb_va_list_t __tb_varg_list; \
    tb_va_start(__tb_varg_list, fmt); \
    __tb_ret = tb_vsnprintf(s, (n), fmt, __tb_varg_list); \
    tb_va_end(__tb_varg_list); \
	if (__tb_ret >= 0) s[__tb_ret] = '\0'; \
	if (r) *r = __tb_ret > 0? __tb_ret : 0; \
 \
} while (0) 

// vswprintf format
#define tb_vswprintf_format(s, n, fmt, r) \
do \
{ \
	tb_long_t __tb_ret = 0; \
	tb_va_list_t __tb_varg_list; \
    tb_va_start(__tb_varg_list, fmt); \
    __tb_ret = tb_vswprintf(s, (n), fmt, __tb_varg_list); \
    tb_va_end(__tb_varg_list); \
	if (__tb_ret >= 0) s[__tb_ret] = L'\0'; \
	if (r) *r = __tb_ret > 0? __tb_ret : 0; \
 \
} while (0) 

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! sprintf
 *
 * @param s 		the string data
 * @param fmt 		the format string
 * 
 * @return 			the real size
 */
tb_long_t 			tb_sprintf(tb_char_t* s, tb_char_t const* fmt, ...);

/*! snprintf
 *
 * @param s 		the string data
 * @param n 		the string size
 * @param fmt 		the format string
 * 
 * @return 			the real size
 */
tb_long_t 			tb_snprintf(tb_char_t* s, tb_size_t n, tb_char_t const* fmt, ...);

/*! vsnprintf
 *
 * @param s 		the string data
 * @param n 		the string size
 * @param fmt 		the format string
 * @param args 		the arguments
 * 
 * @return 			the real size
 */
tb_long_t 			tb_vsnprintf(tb_char_t* s, tb_size_t n, tb_char_t const* fmt, tb_va_list_t args);

/*! swprintf
 *
 * @param s 		the string data
 * @param n 		the string size
 * @param fmt 		the format string
 * 
 * @return 			the real size
 */
tb_long_t 			tb_swprintf(tb_wchar_t* s, tb_size_t n, tb_wchar_t const* fmt, ...);

/*! vswprintf
 *
 * @param s 		the string data
 * @param n 		the string size
 * @param fmt 		the format string
 * @param args 		the arguments
 * 
 * @return 			the real size
 */
tb_long_t 			tb_vswprintf(tb_wchar_t* s, tb_size_t n, tb_wchar_t const* fmt, tb_va_list_t args);

#endif
