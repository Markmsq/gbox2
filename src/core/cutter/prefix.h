/*!The Graphic 2D Box Library
 * 
 * GBox2 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * GBox2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GBox2; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		prefix.h
 *
 */
#ifndef G2_CORE_CUTTER_PREFIX_H
#define G2_CORE_CUTTER_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the cutter func type
struct __g2_cutter_t;
typedef tb_void_t (*g2_cutter_func_t)(struct __g2_cutter_t* cutter, g2_point_t const* pt);

// the cutter type
typedef struct __g2_cutter_t
{
	// the func
	g2_cutter_func_t 	func;

	// the data
	tb_pointer_t 		data;

}g2_cutter_t;


#endif


