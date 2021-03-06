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
 * @file		clippath.c
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
static tb_void_t g2_svg_element_clippath_writ(g2_svg_element_t const* element, tb_gstream_t* gst)
{
	g2_svg_element_clippath_t const* clippath = (g2_svg_element_clippath_t const*)element;
	tb_assert_and_check_return(clippath);

	// id
	if (tb_pstring_size(&clippath->base.id))
		tb_gstream_printf(gst, " id=\"%s\"", tb_pstring_cstr(&clippath->base.id));

	// units
	if (clippath->units)
	{
		static tb_char_t const* units[] =
		{
			"userSpaceOnUse"
		, 	"objectBoundingBox"
		};
		if (clippath->units && clippath->units - 1 < tb_arrayn(units)) 
			tb_gstream_printf(gst, " clipPathUnits=\"%s\"", units[clippath->units - 1]);
	}

	// style 
	g2_svg_writer_style(gst, &clippath->style); 

	// transform 
	g2_svg_writer_transform(gst, &clippath->transform); 
}
static tb_void_t g2_svg_element_clippath_clip(g2_svg_element_t const* element, g2_svg_painter_t* painter, tb_size_t mode)
{
	// check
	g2_svg_element_clippath_t const* clippath = (g2_svg_element_clippath_t const*)element;
	tb_assert_and_check_return(clippath);

	// walk
	if (element->head)
	{
		g2_svg_element_t* next = element->head;
		while (next)
		{
			// clip 
			if (next->clip) next->clip(next, painter, (next == element->head)? mode : G2_CLIPPER_MODE_UNION);

			// next
			next = next->next;
		}
	}

	// clip the sub-clippath if exists
	if (clippath->style.mode & G2_SVG_STYLE_MODE_CLIPPATH
		&& clippath->style.clippath.mode == G2_SVG_STYLE_CLIPPATH_MODE_URL)
	{
		// url
		tb_char_t const* url = tb_pstring_cstr(&clippath->style.clippath.url);
		if (url && url[0] == '#')
		{
			// the sub-clippath
			g2_svg_element_t* subclippath = tb_hash_get(painter->hash, &url[1]);
			tb_assert_and_check_return(subclippath && subclippath->type == G2_SVG_ELEMENT_TYPE_CLIPPATH);

			// clip it
			if (subclippath->clip) subclippath->clip(subclippath, painter, G2_CLIPPER_MODE_INTERSECT);
		}
	}
}
static tb_void_t g2_svg_element_clippath_exit(g2_svg_element_t* element)
{
	g2_svg_element_clippath_t* clippath = (g2_svg_element_clippath_t*)element;
	if (clippath)
	{
		// exit style
		g2_svg_style_exit(&clippath->style);
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * initializer
 */
g2_svg_element_t* g2_svg_element_init_clippath(tb_handle_t reader)
{
	// alloc 
	g2_svg_element_clippath_t* element = tb_malloc0(sizeof(g2_svg_element_clippath_t));
	tb_assert_and_check_return_val(element, tb_null);

	// init
	element->base.exit = g2_svg_element_clippath_exit;
	element->base.writ = g2_svg_element_clippath_writ;
	element->base.clip = g2_svg_element_clippath_clip;
	
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
		else if (!tb_pstring_cstricmp(&attr->name, "clipPathUnits"))
			g2_svg_parser_style_units(p, &element->units);
		else if (!tb_pstring_cstricmp(&attr->name, "style"))
			g2_svg_parser_style(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "clip-path"))
			g2_svg_parser_style_clippath(p, &element->style);
		else if (!tb_pstring_cstricmp(&attr->name, "transform"))
			g2_svg_parser_transform(p, &element->transform);
	}

	// ok
	return element;
}

