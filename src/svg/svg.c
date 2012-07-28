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
 * @file		svg.c
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

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t g2_svg_element_svg_writ(g2_svg_element_t const* element, tb_gstream_t* gst)
{
	g2_svg_element_svg_t const* svg = (g2_svg_element_svg_t const*)element;
	tb_assert_and_check_return(svg);

	// x & y
	if (g2_nz(svg->x) || g2_nz(svg->y)) 
		tb_gstream_printf(gst, " x=\"%f\" y=\"%f\"", g2_float_to_tb(svg->x), g2_float_to_tb(svg->y));

	// width & height
	if (g2_nz(svg->width) || g2_nz(svg->height)) 
		tb_gstream_printf(gst, " width=\"%f\" height=\"%f\"", g2_float_to_tb(svg->width), g2_float_to_tb(svg->height));

	// viewbox
	if (g2_nz(svg->viewbox.w) || g2_nz(svg->viewbox.h))
		tb_gstream_printf(gst, " viewBox=\"%f %f %f %f\"", g2_float_to_tb(svg->viewbox.x), g2_float_to_tb(svg->viewbox.y), g2_float_to_tb(svg->viewbox.w), g2_float_to_tb(svg->viewbox.h));
}

/* ///////////////////////////////////////////////////////////////////////
 * initializer
 */
g2_svg_element_t* g2_svg_element_init_svg(tb_handle_t reader)
{
	// alloc 
	g2_svg_element_svg_t* element = tb_malloc0(sizeof(g2_svg_element_svg_t));
	tb_assert_and_check_return_val(element, TB_NULL);

	// init
	element->base.writ = g2_svg_element_svg_writ;

	// attributes
	tb_xml_node_t const* attr = tb_xml_reader_attributes(reader);
	for (; attr; attr = attr->next)
	{
		tb_char_t const* p = tb_pstring_cstr(&attr->data);
		if (!tb_pstring_cstricmp(&attr->name, "x"))
			g2_svg_parser_float(p, &element->x);	
		else if (!tb_pstring_cstricmp(&attr->name, "y"))
			g2_svg_parser_float(p, &element->y);	
		else if (!tb_pstring_cstricmp(&attr->name, "width"))
			g2_svg_parser_float(p, &element->width);	
		else if (!tb_pstring_cstricmp(&attr->name, "height"))
			g2_svg_parser_float(p, &element->height);	
		else if (!tb_pstring_cstricmp(&attr->name, "viewBox"))
		{
			p = g2_svg_parser_float(p, &element->viewbox.x); p = g2_svg_parser_separator_skip(p);
			p = g2_svg_parser_float(p, &element->viewbox.y); p = g2_svg_parser_separator_skip(p);
			p = g2_svg_parser_float(p, &element->viewbox.w); p = g2_svg_parser_separator_skip(p);
			p = g2_svg_parser_float(p, &element->viewbox.h);
		}
	}

	// ok
	return element;
}

