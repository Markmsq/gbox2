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
 * @file		malloc.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_MALLOC_H
#define TB_MEMORY_MALLOC_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_debug__
#	define tb_malloc_malloc(size) 				tb_malloc_malloc_impl(size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_malloc_malloc0(size) 				tb_malloc_malloc0_impl(size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_malloc_nalloc(item, size) 		tb_malloc_nalloc_impl(item, size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_malloc_nalloc0(item, size) 		tb_malloc_nalloc0_impl(item, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_malloc_ralloc(data, size) 		tb_malloc_ralloc_impl(data, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_malloc_free(data) 				tb_malloc_free_impl(data, __tb_func__, __tb_line__, __tb_file__)
#else
#	define tb_malloc_malloc(size) 				tb_malloc_malloc_impl(size)
#	define tb_malloc_malloc0(size) 				tb_malloc_malloc0_impl(size)

#	define tb_malloc_nalloc(item, size) 		tb_malloc_nalloc_impl(item, size)
#	define tb_malloc_nalloc0(item, size) 		tb_malloc_nalloc0_impl(item, size)

#	define tb_malloc_ralloc(data, size) 		tb_malloc_ralloc_impl(data, size)
# 	define tb_malloc_free(data) 				tb_malloc_free_impl(data)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_bool_t 		tb_malloc_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

// exit
tb_void_t 		tb_malloc_exit();

// dump
tb_void_t 		tb_malloc_dump();

// malloc
#ifndef __tb_debug__
tb_pointer_t 	tb_malloc_malloc_impl(tb_size_t size);
#else
tb_pointer_t 	tb_malloc_malloc_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// malloc0
#ifndef __tb_debug__
tb_pointer_t 	tb_malloc_malloc0_impl(tb_size_t size);
#else
tb_pointer_t 	tb_malloc_malloc0_impl(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc
#ifndef __tb_debug__
tb_pointer_t  	tb_malloc_nalloc_impl(tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_malloc_nalloc_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc0
#ifndef __tb_debug__
tb_pointer_t  	tb_malloc_nalloc0_impl(tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_malloc_nalloc0_impl(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// ralloc
#ifndef __tb_debug__
tb_pointer_t 	tb_malloc_ralloc_impl(tb_pointer_t data, tb_size_t size);
#else
tb_pointer_t 	tb_malloc_ralloc_impl(tb_pointer_t data, tb_size_t size, tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
#endif

// free
#ifndef __tb_debug__
tb_bool_t 		tb_malloc_free_impl(tb_pointer_t data);
#else
tb_bool_t 		tb_malloc_free_impl(tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

#endif

