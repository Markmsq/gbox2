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
 * @file		fill.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fill.h"
#include "quad.h"
#include "cube.h"
#include "func.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t g2_cutter_fill_nonzero_done(g2_cutter_fill_t* cutter, tb_handle_t path)
{
}
static tb_void_t g2_cutter_fill_evenodd_done(g2_cutter_fill_t* cutter, tb_handle_t path)
{
	// init path itor
	if (g2_path_itor_init(path))
	{
		// walk path
		g2_point_t 	pt[3];
		tb_size_t 	co = G2_PATH_CODE_NONE;
		while (co = g2_path_itor_next(path, pt))
			cutter->base.func(cutter, co, pt);

		// exit path itor
		g2_path_itor_exit(path);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t g2_cutter_fill_init(g2_cutter_fill_t* cutter, tb_size_t rule, g2_cutter_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(cutter);

	// init
	cutter->base.func = func;
	cutter->base.data = data;
	cutter->rule = rule;
}
tb_void_t g2_cutter_fill_done(g2_cutter_fill_t* cutter, tb_handle_t path)
{
	// check
	tb_assert_and_check_return(cutter->base.func && path);

	// null?
	tb_check_return(!g2_path_null(path));

	// done 
	switch (cutter->rule)
	{
	case G2_STYLE_RULE_NONZERO:
		g2_cutter_fill_nonzero_done(cutter, path);
		break;
	case G2_STYLE_RULE_EVENODD:
	default:
		g2_cutter_fill_evenodd_done(cutter, path);
		break;
	}
}
tb_void_t g2_cutter_fill_exit(g2_cutter_fill_t* cutter)
{
}


