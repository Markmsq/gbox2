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
 * @file		circle.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"svg"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "element.h"
#include "parser/parser.h"
#include "writer/writer.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t g2_svg_element_circle_writ(g2_svg_element_t const* element, tb_gstream_t* gst)
{
	g2_svg_element_circle_t const* circle = (g2_svg_element_circle_t const*)element;
	tb_assert_and_check_return(circle);

	// circle
	if (g2_nz(circle->circle.c.x)) tb_gstream_printf(gst, " cx=\"%f\"", g2_float_to_tb(circle->circle.c.x));
	if (g2_nz(circle->circle.c.y)) tb_gstream_printf(gst, " cy=\"%f\"", g2_float_to_tb(circle->circle.c.y));
	if (g2_nz(circle->circle.r)) tb_gstream_printf(gst, " r=\"%f\"", g2_float_to_tb(circle->circle.r));

	// style 
	g2_svg_writer_style(gst, circle->style); 

	// transform 
	g2_svg_writer_transform(gst, &circle->matrix); 
}
static tb_void_t g2_svg_element_circle_exit(g2_svg_element_t* element)
{
	g2_svg_element_circle_t* circle = (g2_svg_element_circle_t*)element;
	if (circle)
	{
		// exit style
		if (circle->style) g2_style_exit(circle->style);
		circle->style = TB_NULL;
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * initializer
 */
g2_svg_element_t* g2_svg_element_init_circle(tb_handle_t reader)
{
	// alloc 
	g2_svg_element_circle_t* element = tb_malloc0(sizeof(g2_svg_element_circle_t));
	tb_assert_and_check_return_val(element, TB_NULL);

	// init
	element->base.exit = g2_svg_element_circle_exit;
	element->base.writ = g2_svg_element_circle_writ;

	// init style
	element->style = g2_style_init();

	// init matrix
	g2_matrix_clear(&element->matrix);

	// attributes
	tb_xml_node_t const* attr = tb_xml_reader_attributes(reader);
	for (; attr; attr = attr->next)
	{
		tb_char_t const* p = tb_pstring_cstr(&attr->data);
		if (!tb_pstring_cstricmp(&attr->name, "cx"))
			g2_svg_parser_float(p, &element->circle.c.x);
		else if (!tb_pstring_cstricmp(&attr->name, "cy"))
			g2_svg_parser_float(p, &element->circle.c.y);
		else if (!tb_pstring_cstricmp(&attr->name, "r"))
			g2_svg_parser_float(p, &element->circle.r);
		else if (!tb_pstring_cstricmp(&attr->name, "fill"))
			g2_svg_parser_style_fill(p, element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "stroke"))
			g2_svg_parser_style_stroke(p, element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "transform"))
			g2_svg_parser_transform(p, &element->matrix);

	}

	// ok
	return element;
}

