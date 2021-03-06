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
#ifndef G2_SVG_PAINTER_H
#define G2_SVG_PAINTER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "element.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the svg painter type
typedef struct __g2_svg_painter_t
{
	// the element 
	g2_svg_element_t const* element;

	// the painter
	tb_handle_t 			painter;

	// the style
	tb_handle_t 			style;

	// the pool
	tb_handle_t 			pool;

	// the hash
	tb_hash_t* 				hash;

}g2_svg_painter_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t			g2_draw_svg(tb_handle_t painter, g2_svg_element_t const* element);

#endif


