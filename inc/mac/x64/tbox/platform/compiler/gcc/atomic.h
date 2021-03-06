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
 * @file		atomic.h
 *
 */
#ifndef TB_PLATFORM_COMPILER_GCC_ATOMIC_H
#define TB_PLATFORM_COMPILER_GCC_ATOMIC_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define tb_atomic_get(a) 					tb_atomic_get_sync(a)
#define tb_atomic_set(a, v) 				tb_atomic_set_sync(a, v)
#define tb_atomic_set0(a) 					tb_atomic_set0_sync(a)
#define tb_atomic_pset(a, p, v) 			tb_atomic_pset_sync(a, p, v)

#define tb_atomic_fetch_and_set0(a) 		tb_atomic_fetch_and_set0_sync(a)
#define tb_atomic_fetch_and_set(a, v) 		tb_atomic_fetch_and_set_sync(a, v)
#define tb_atomic_fetch_and_pset(a, p, v) 	tb_atomic_fetch_and_pset_sync(a, p, v)
#define tb_atomic_fetch_and_inc(a) 			tb_atomic_fetch_and_inc_sync(a)
#define tb_atomic_fetch_and_dec(a) 			tb_atomic_fetch_and_dec_sync(a)
#define tb_atomic_fetch_and_add(a, v) 		tb_atomic_fetch_and_add_sync(a, v)
#define tb_atomic_fetch_and_sub(a, v) 		tb_atomic_fetch_and_sub_sync(a, v)
#define tb_atomic_fetch_and_or(a, v) 		tb_atomic_fetch_and_or_sync(a, v)
#define tb_atomic_fetch_and_and(a, v) 		tb_atomic_fetch_and_and_sync(a, v)

#define tb_atomic_inc_and_fetch(a) 			tb_atomic_inc_and_fetch_sync(a)
#define tb_atomic_dec_and_fetch(a) 			tb_atomic_dec_and_fetch_sync(a)
#define tb_atomic_add_and_fetch(a, v) 		tb_atomic_add_and_fetch_sync(a, v)
#define tb_atomic_sub_and_fetch(a, v) 		tb_atomic_sub_and_fetch_sync(a, v)
#define tb_atomic_or_and_fetch(a, v) 		tb_atomic_or_and_fetch_sync(a, v)
#define tb_atomic_and_and_fetch(a, v) 		tb_atomic_and_and_fetch_sync(a, v)

// FIXME: ios armv6: no defined refernece?
#if !(defined(TB_CONFIG_OS_IOS) && TB_CONFIG_ARM_VERSION < 7)
# 	define tb_atomic_fetch_and_xor(a, v) 		tb_atomic_fetch_and_xor_sync(a, v)
# 	define tb_atomic_xor_and_fetch(a, v) 		tb_atomic_xor_and_fetch_sync(a, v)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * get & set
 */
static __tb_inline__ tb_size_t tb_atomic_get_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_val_compare_and_swap(a, 0, 0);
}
static __tb_inline__ tb_void_t tb_atomic_set_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	__sync_lock_test_and_set(a, v);
}
static __tb_inline__ tb_void_t tb_atomic_set0_sync(tb_atomic_t* a)
{
	tb_assert(a);
	 __sync_lock_test_and_set(a, 0);
}
static __tb_inline__ tb_void_t tb_atomic_pset_sync(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);
	__sync_val_compare_and_swap(a, p, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set0_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_lock_test_and_set(a, 0);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_set_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_lock_test_and_set(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_pset_sync(tb_atomic_t* a, tb_size_t p, tb_size_t v)
{
	tb_assert(a);
	return __sync_val_compare_and_swap(a, p, v);
}

/* ///////////////////////////////////////////////////////////////////////
 * fetch and ...
 */
static __tb_inline__ tb_long_t tb_atomic_fetch_and_inc_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_fetch_and_add(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_dec_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_fetch_and_sub(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add_sync(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_fetch_and_add(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_sub_sync(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_fetch_and_sub(a, v);
}

#if !(defined(TB_CONFIG_OS_IOS) && (TB_CONFIG_ARM_VERSION < 7))
static __tb_inline__ tb_size_t tb_atomic_fetch_and_xor_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_xor(a, v);
}
#endif
static __tb_inline__ tb_size_t tb_atomic_fetch_and_and_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_and(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_fetch_and_or_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_fetch_and_or(a, v);
}

/* ///////////////////////////////////////////////////////////////////////
 * ... and fetch
 */
static __tb_inline__ tb_long_t tb_atomic_inc_and_fetch_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_add_and_fetch(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_dec_and_fetch_sync(tb_atomic_t* a)
{
	tb_assert(a);
	return __sync_sub_and_fetch(a, 1);
}
static __tb_inline__ tb_long_t tb_atomic_add_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_add_and_fetch(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_sub_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
	tb_assert(a);
	return __sync_sub_and_fetch(a, v);
}

#if !(defined(TB_CONFIG_OS_IOS) && (TB_CONFIG_ARM_VERSION < 7))
static __tb_inline__ tb_size_t tb_atomic_xor_and_fetch_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_xor_and_fetch(a, v);
}
#endif

static __tb_inline__ tb_size_t tb_atomic_and_and_fetch_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_and_and_fetch(a, v);
}
static __tb_inline__ tb_size_t tb_atomic_or_and_fetch_sync(tb_atomic_t* a, tb_size_t v)
{
	tb_assert(a);
	return __sync_or_and_fetch(a, v);
}

#endif
