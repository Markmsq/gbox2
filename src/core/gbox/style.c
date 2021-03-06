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
 * @file		style.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../gbox2.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t g2_style_init()
{
	// alloc
	g2_style_t* gstyle = tb_malloc0(sizeof(g2_style_t));
	tb_assert_and_check_return_val(gstyle, tb_null);

	// init
	g2_style_clear(gstyle);

	// ok
	return gstyle;
}
tb_void_t g2_style_exit(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	if (gstyle) 
	{
		// exit shader
		if (gstyle->shader) g2_shader_dec(gstyle->shader);
		gstyle->shader = tb_null;

		// exit style
		tb_free(gstyle);
	}
}
tb_void_t g2_style_clear(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	// clear
	gstyle->mode 	= G2_STYLE_MODE_NONE;
	gstyle->flag 	= g2_quality() > G2_QUALITY_LOW? (G2_STYLE_FLAG_ANTI_ALIAS | G2_STYLE_FLAG_BITMAP_FILTER) : G2_STYLE_FLAG_NONE;
	gstyle->cap 	= G2_STYLE_CAP_BUTT;
	gstyle->join 	= G2_STYLE_JOIN_MITER;
	gstyle->rule 	= G2_STYLE_RULE_EVENODD;
	gstyle->width 	= G2_ONE;
	gstyle->color 	= G2_COLOR_DEFAULT;

	// clear shader
	if (gstyle->shader) g2_shader_dec(gstyle->shader);
	gstyle->shader = tb_null;
}
tb_void_t g2_style_copy(tb_handle_t style, tb_handle_t copy)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	g2_style_t* gcopy = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle && gcopy);

	// refn++
	if (gcopy->shader) g2_shader_inc(gcopy->shader);

	// refn--
	if (gstyle->shader) g2_shader_dec(gstyle->shader);

	// copy
	tb_memcpy(gstyle, copy, sizeof(g2_style_t));
}
tb_size_t g2_style_mode(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_STYLE_MODE_NONE);

	return gstyle->mode;
}
tb_void_t g2_style_mode_set(tb_handle_t style, tb_size_t mode)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->mode = mode;
}
tb_size_t g2_style_flag(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_STYLE_FLAG_NONE);

	return gstyle->flag;
}
tb_void_t g2_style_flag_set(tb_handle_t style, tb_size_t flag)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->flag = flag;
}
g2_color_t g2_style_color(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_COLOR_DEFAULT);

	return gstyle->color;
}
tb_void_t g2_style_color_set(tb_handle_t style, g2_color_t color)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->color = color;
}
tb_byte_t g2_style_alpha(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, 0xff);

	return gstyle->color.a;
}
tb_void_t g2_style_alpha_set(tb_handle_t style, tb_byte_t alpha)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->color.a = alpha;
}
g2_float_t g2_style_width(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_ONE);

	return gstyle->width;
}
tb_void_t g2_style_width_set(tb_handle_t style, g2_float_t width)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->width = width;
}
tb_size_t g2_style_cap(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_STYLE_CAP_BUTT);

	return gstyle->cap;
}
tb_void_t g2_style_cap_set(tb_handle_t style, tb_size_t cap)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->cap = cap;
}
tb_size_t g2_style_join(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_STYLE_JOIN_MITER);

	return gstyle->join;
}
tb_void_t g2_style_join_set(tb_handle_t style, tb_size_t join)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->join = join;
}
tb_size_t g2_style_rule(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, G2_STYLE_RULE_EVENODD);

	return gstyle->rule;
}
tb_void_t g2_style_rule_set(tb_handle_t style, tb_size_t rule)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	gstyle->rule = rule;
}
tb_handle_t g2_style_shader(tb_handle_t style)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return_val(gstyle, tb_null);

	return gstyle->shader;
}
tb_void_t g2_style_shader_set(tb_handle_t style, tb_handle_t shader)
{
	g2_style_t* gstyle = (g2_style_t*)style;
	tb_assert_and_check_return(gstyle);

	// ref--
	if (gstyle->shader) g2_shader_dec(gstyle->shader);

	// assign
	gstyle->shader = shader;

	// ref++
	if (shader) g2_shader_inc(shader);
}
