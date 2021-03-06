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
 * @file		text.c
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
static tb_void_t g2_svg_element_text_writ(g2_svg_element_t const* element, tb_gstream_t* gst)
{
	g2_svg_element_text_t const* text = (g2_svg_element_text_t const*)element;
	tb_assert_and_check_return(text);

	// id
	if (tb_pstring_size(&text->base.id))
		tb_gstream_printf(gst, " id=\"%s\"", tb_pstring_cstr(&text->base.id));

	// rect
	if (g2_nz(text->rect.x) || g2_nz(text->rect.y))
		tb_gstream_printf(gst, " x=\"%f\" y=\"%f\"", g2_float_to_tb(text->rect.x), g2_float_to_tb(text->rect.y));
	if (g2_nz(text->rect.w) || g2_nz(text->rect.h))
		tb_gstream_printf(gst, " dx=\"%f\" dy=\"%f\"", g2_float_to_tb(text->rect.w), g2_float_to_tb(text->rect.h));

	// style 
	g2_svg_writer_style(gst, &text->style); 

	// transform 
	g2_svg_writer_transform(gst, &text->transform); 
}
static tb_void_t g2_svg_element_text_exit(g2_svg_element_t* element)
{
	g2_svg_element_text_t* text = (g2_svg_element_text_t*)element;
	if (text)
	{
		// exit text
		tb_pstring_exit(&text->text);

		// exit style
		g2_svg_style_exit(&text->style);
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * initializer
 */
g2_svg_element_t* g2_svg_element_init_text(tb_handle_t reader)
{
	// alloc 
	g2_svg_element_text_t* element = tb_malloc0(sizeof(g2_svg_element_text_t));
	tb_assert_and_check_return_val(element, tb_null);

	// init
	element->base.exit = g2_svg_element_text_exit;
	element->base.writ = g2_svg_element_text_writ;

	// init text
	tb_pstring_init(&element->text);

	// init style
	g2_svg_style_init(&element->style);

	// init transform
	g2_matrix_clear(&element->transform);

	// attributes
	tb_xml_node_t const* attr = tb_xml_reader_attributes(reader);
	for (; attr; attr = attr->next)
	{
		tb_char_t const* p = tb_pstring_cstr(&attr->data);
		if (!tb_pstring_cstricmp(&attr->name, "id"))
			tb_pstring_strcpy(&element->base.id, &attr->data);
		else if (!tb_pstring_cstricmp(&attr->name, "x"))
			g2_svg_parser_float(p, &element->rect.x);
		else if (!tb_pstring_cstricmp(&attr->name, "y"))
			g2_svg_parser_float(p, &element->rect.y);
		else if (!tb_pstring_cstricmp(&attr->name, "dx"))
			g2_svg_parser_float(p, &element->rect.w);
		else if (!tb_pstring_cstricmp(&attr->name, "dy"))
			g2_svg_parser_float(p, &element->rect.h);
		else if (!tb_pstring_cstricmp(&attr->name, "fill"))
			g2_svg_parser_style_fill(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "stroke"))
			g2_svg_parser_style_stok(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "style"))
			g2_svg_parser_style(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "font-size"))
			g2_svg_parser_style_font_size(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "font-family"))
			g2_svg_parser_style_font_family(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "font-style"))
			g2_svg_parser_style_font_style(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "font-weight"))
			g2_svg_parser_style_font_weight(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "text-anchor"))
			g2_svg_parser_style_text_anchor(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "transform"))
			g2_svg_parser_transform(p, &element->transform);
	}

	// ok
	return element;
}

