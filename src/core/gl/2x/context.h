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
 * @file		context.h
 *
 */
#ifndef G2_CORE_GL2x_CONTEXT_H
#define G2_CORE_GL2x_CONTEXT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "program.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the gl2x context type
typedef struct __g2_gl2x_context_t
{
	// the surface
	tb_handle_t 		surface;

	// the version
	tb_byte_t 			version;

	// the extensions
	tb_byte_t 			extensions[G2_GL_EXT_MAXN];

	// the programs
	tb_handle_t 		programs[G2_GL2X_PROGRAM_TYPE_MAXN];

	// the project matrix
	tb_float_t 			matrix[16];

}g2_gl2x_context_t;


#endif
