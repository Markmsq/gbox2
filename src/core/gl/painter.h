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
 * @file		painter.h
 *
 */
#ifndef G2_CORE_GL_PAINTER_H
#define G2_CORE_GL_PAINTER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "context.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the gl painter type
typedef struct __g2_gl_painter_t
{
	// the context
	g2_gl_context_t* 			context;

	// the matrix
	g2_matrix_t 				matrix;

	// the stack
	tb_stack_t* 				stack_matrix;
	tb_handle_t 				stack_path;
	tb_handle_t 				stack_style;
	tb_handle_t 				stack_clipper;

	// the cache
	tb_handle_t 				pcache;

}g2_gl_painter_t;


#endif
