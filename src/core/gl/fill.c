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
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"fill"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fill.h"
#include "shader.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the gl fill flag type
typedef enum __g2_gl_fill_flag_t
{
	G2_GL_FILL_FLAG_NONE 		= 0
,	G2_GL_FILL_FLAG_RECT 		= 1
,	G2_GL_FILL_FLAG_STENCIL 	= 2
,	G2_GL_FILL_FLAG_CONVEX 		= 4
,	G2_GL_FILL_FLAG_SCISSOR 	= 8

}g2_gl_fill_flag_t;

// the gl fill radial factor type
typedef struct __g2_gl_fill_radial_factor_t
{
	// the factor
	tb_float_t 			factor;

	// the rotate
	tb_float_t 			rotation;

	// the count
	tb_size_t 			count;
	
}g2_gl_fill_radial_factor_t;

// the gl fill type
typedef struct __g2_gl_fill_t
{
	// the painter
	g2_gl_painter_t* 	painter;

	// the context
	g2_gl_context_t* 	context;

	// the clipper 
	tb_handle_t 		clipper;

	// the program for gl >= 2.0
	tb_handle_t 		program;

	// the style 
	tb_handle_t 		style;

	// the shader
	g2_gl_shader_t* 	shader;

	// the flag
	tb_size_t 			flag;

	// the stencil pass value
	tb_byte_t 			pass;

	// the vertices, 4[points] x 2[xy]
	tb_float_t 			vertices[8];

	// the texcoords, 4[points] x 2[xy]
	tb_float_t 			texcoords[8];

	// the vertex matrix
	tb_float_t 			vmatrix[16];

	// the texture matrix
	tb_float_t 			tmatrix[16];

}g2_gl_fill_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_void_t g2_gl_fill_apply_antialiasing(g2_gl_fill_t* fill)
{
	if (g2_style_flag(fill->style) & G2_STYLE_FLAG_ANTI_ALIAS) g2_glEnable(G2_GL_MULTISAMPLE);
	else g2_glDisable(G2_GL_MULTISAMPLE);
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_color(g2_gl_fill_t* fill, g2_color_t color)
{
	if (fill->context->version < 0x20)
		g2_glColor4f((tb_float_t)color.r / 0xff, (tb_float_t)color.g / 0xff, (tb_float_t)color.b / 0xff, (tb_float_t)color.a / 0xff);
	else g2_glVertexAttrib4f(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_COLORS), (tb_float_t)color.r / 0xff, (tb_float_t)color.g / 0xff, (tb_float_t)color.b / 0xff, (tb_float_t)color.a / 0xff);
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_blend(g2_gl_fill_t* fill, tb_bool_t enable)
{
	if (enable) 
	{
		g2_glEnable(G2_GL_BLEND);
		g2_glBlendFunc(G2_GL_SRC_ALPHA, G2_GL_ONE_MINUS_SRC_ALPHA);
	}
	else g2_glDisable(G2_GL_BLEND);
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_solid(g2_gl_fill_t* fill)
{
	// apply blend
	g2_gl_fill_apply_blend(fill, g2_style_alpha(fill->style) != 0xff);

	// apply color
	g2_gl_fill_apply_color(fill, g2_style_color(fill->style));
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_wrap(g2_gl_fill_t* fill)
{
	// wrap
	static g2_GLuint_t wrap[] = 
	{
		G2_GL_CLAMP_TO_BORDER
	,	G2_GL_CLAMP_TO_BORDER
	,	G2_GL_CLAMP_TO_EDGE
	,	G2_GL_REPEAT
	,	G2_GL_MIRRORED_REPEAT
	};
	tb_assert(fill->shader->wrap < tb_arrayn(wrap));

	// wrap
	switch (fill->shader->type)
	{
	case G2_GL_SHADER_TYPE_BITMAP:
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_WRAP_S, wrap[fill->shader->wrap]);
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_WRAP_T, wrap[fill->shader->wrap]);
		break;
	case G2_GL_SHADER_TYPE_LINEAR:
	case G2_GL_SHADER_TYPE_RADIAL:
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_WRAP_S, wrap[fill->shader->wrap]);
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_WRAP_T, G2_GL_CLAMP_TO_EDGE);
		break;
	default:
		break;
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_filter(g2_gl_fill_t* fill)
{
	// filter?
	if (g2_style_flag(fill->style) & G2_STYLE_FLAG_BITMAP_FILTER)
	{
		// init filter
		tb_size_t filter = g2_quality() > G2_QUALITY_LOW? G2_GL_LINEAR : G2_GL_NEAREST;
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_MAG_FILTER, filter);
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_MIN_FILTER, filter);
	}
	else
	{
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_MAG_FILTER, G2_GL_NEAREST);
		g2_glTexParameteri(G2_GL_TEXTURE_2D, G2_GL_TEXTURE_MIN_FILTER, G2_GL_NEAREST);
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_vertices(g2_gl_fill_t* fill)
{
	if (fill->context->version < 0x20) g2_glVertexPointer(2, G2_GL_FLOAT, 0, fill->vertices);
	else g2_glVertexAttribPointer(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_VERTICES), 2, G2_GL_FLOAT, G2_GL_FALSE, 0, fill->vertices);
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_texcoords(g2_gl_fill_t* fill)
{
	// texcoords
	if (fill->context->version < 0x20) g2_glTexCoordPointer(2, G2_GL_FLOAT, 0, fill->texcoords);
	else g2_glVertexAttribPointer(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_TEXCOORDS), 2, G2_GL_FLOAT, G2_GL_FALSE, 0, fill->texcoords);
}
static __tb_inline__ tb_bool_t g2_gl_fill_apply_program(g2_gl_fill_t* fill)
{
	if (fill->context->version >= 0x20)
	{	
		// type
		tb_size_t type = G2_GL_PROGRAM_TYPE_COLOR;
		if (fill->shader)
		{
			switch (fill->shader->type)
			{
			case G2_GL_SHADER_TYPE_BITMAP:
				type = G2_GL_PROGRAM_TYPE_BITMAP;
				break;
			case G2_GL_SHADER_TYPE_LINEAR:
			case G2_GL_SHADER_TYPE_RADIAL:
				type = G2_GL_PROGRAM_TYPE_BITMAP;
				break;
			default:
				tb_assert_and_check_return_val(0, tb_false);
				break;
			}
		}

		// program
		fill->program = fill->context->programs[type];
		tb_assert_and_check_return_val(fill->program, tb_false);
		g2_gl_program_uses(fill->program);
	}

	// ok
	return tb_true;
}
static __tb_inline__ tb_void_t g2_gl_fill_enter_vertex_state(g2_gl_fill_t* fill)
{
	// apply antialiasing?
	g2_gl_fill_apply_antialiasing(fill);

	// apply vertices & texcoords
	if (fill->context->version < 0x20)
	{
		// enable vertices
		g2_glEnableClientState(G2_GL_VERTEX_ARRAY);
	}
	else
	{
		// enable vertices
		g2_glEnableVertexAttribArray(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_VERTICES));

		// apply project matrix
		g2_glUniformMatrix4fv(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_MATRIX_PROJECT), 1, G2_GL_FALSE, fill->context->matrix);
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_leave_vertex_state(g2_gl_fill_t* fill)
{
	// disable antialiasing
	g2_glDisable(G2_GL_MULTISAMPLE);

	// apply vertices & texcoords
	if (fill->context->version < 0x20)
	{
		// disable vertices
		g2_glDisableClientState(G2_GL_VERTEX_ARRAY);
	}
	else
	{
		// disable vertices
		g2_glDisableVertexAttribArray(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_VERTICES));
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_enter_texture_state(g2_gl_fill_t* fill)
{
	// enable texture
	g2_glEnable(G2_GL_TEXTURE_2D);

	// bind texture
	g2_glBindTexture(G2_GL_TEXTURE_2D, *fill->shader->texture);

	// apply vertices & texcoords
	if (fill->context->version < 0x20)
	{
		// enable texcoords
		g2_glEnableClientState(G2_GL_TEXTURE_COORD_ARRAY);

		// apply mode
		g2_glTexEnvi(G2_GL_TEXTURE_ENV, G2_GL_TEXTURE_ENV_MODE, G2_GL_MODULATE);
	}
	else
	{
		// enable texcoords
		g2_glEnableVertexAttribArray(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_TEXCOORDS));
	}

	// apply blend
	tb_byte_t 	alpha = g2_style_alpha(fill->style);
	g2_gl_fill_apply_blend(fill, fill->shader->flag & G2_GL_SHADER_FLAG_ALPHA || fill->shader->wrap == G2_SHADER_WRAP_BORDER || alpha != 0xff);

	// apply color
	g2_gl_fill_apply_color(fill, g2_color_make(alpha, 0xff, 0xff, 0xff));

	// apply wrap
	g2_gl_fill_apply_wrap(fill);

	// apply filter
	g2_gl_fill_apply_filter(fill);
}
static __tb_inline__ tb_void_t g2_gl_fill_leave_texture_state(g2_gl_fill_t* fill)
{
	// apply vertices & texcoords
	if (fill->context->version < 0x20)
	{
		// disable texcoords
		g2_glDisableClientState(G2_GL_TEXTURE_COORD_ARRAY);
	}
	else
	{
		// disable texcoords
		g2_glDisableVertexAttribArray(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_TEXCOORDS));
	}

	// disable texture
	g2_glDisable(G2_GL_TEXTURE_2D);
}
static __tb_inline__ tb_void_t g2_gl_fill_enter_vertex_matrix(g2_gl_fill_t* fill)
{
	if (fill->context->version < 0x20)
	{
		g2_glMatrixMode(G2_GL_MODELVIEW);
		g2_glPushMatrix();
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_vertex_matrix(g2_gl_fill_t* fill)
{
	if (fill->context->version < 0x20)
	{
		g2_glMatrixMode(G2_GL_MODELVIEW);
		g2_glLoadIdentity();
		g2_glMultMatrixf(fill->vmatrix);
	}
	else g2_glUniformMatrix4fv(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_MATRIX_MODEL), 1, G2_GL_FALSE, fill->vmatrix);
}
static __tb_inline__ tb_void_t g2_gl_fill_leave_vertex_matrix(g2_gl_fill_t* fill)
{
	if (fill->context->version < 0x20)
	{
		g2_glMatrixMode(G2_GL_MODELVIEW);
		g2_glPopMatrix();
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_enter_texture_matrix(g2_gl_fill_t* fill)
{
	// apply matrix
	if (fill->context->version < 0x20)
	{
		// enter the texture matrix mode
		g2_glMatrixMode(G2_GL_TEXTURE);
		g2_glPushMatrix();
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_apply_texture_matrix(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// check
	tb_assert_and_check_return(fill->shader);

	// init
	tb_float_t bx = bounds->x1;
	tb_float_t by = bounds->y1;
	tb_float_t bw = bounds->x2 - bounds->x1;
	tb_float_t bh = bounds->y2 - bounds->y1;
	tb_float_t sw = fill->shader->width;
	tb_float_t sh = fill->shader->height;

	// apply matrix
	if (fill->context->version < 0x20)
	{
		// apply the texture matrix	
		g2_glMatrixMode(G2_GL_TEXTURE);
		g2_glLoadIdentity();
		g2_glMultMatrixf(fill->shader->matrix_gl);

		// disable auto scale in the bounds
		g2_glScalef(bw / sw, bh / sh, 1.0f);

		// move viewport to global
		g2_glTranslatef(bx / bw, by / bh, 0.0f);
	}
	else 
	{
		// init matrix
		g2_gl_matrix_copy(fill->tmatrix, fill->shader->matrix_gl);

		// disable auto scale in the bounds
		g2_gl_matrix_scale(fill->tmatrix, bw / sw, bh / sh);

		// move viewport to global
		g2_gl_matrix_translate(fill->tmatrix, bx / bw, by / bh);

		// apply the texture matrix
		g2_glUniformMatrix4fv(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_MATRIX_TEXCOORD), 1, G2_GL_FALSE, fill->tmatrix);
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_leave_texture_matrix(g2_gl_fill_t* fill)
{
	if (fill->context->version < 0x20)
	{
		// leave the texture matrix mode
		g2_glMatrixMode(G2_GL_TEXTURE);
		g2_glPopMatrix();
		g2_glMatrixMode(G2_GL_MODELVIEW);
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_init(g2_gl_fill_t* fill)
{
	// has stencil or has clipper? init it
	if ( 	fill->flag & G2_GL_FILL_FLAG_STENCIL
		|| ( 	!(fill->flag & G2_GL_FILL_FLAG_SCISSOR)
			&& 	g2_clipper_size(fill->clipper)))
	{
		// enable stencil
		g2_glDisable(G2_GL_BLEND);
		g2_glDisable(G2_GL_TEXTURE_2D);
		g2_glDisable(G2_GL_DEPTH_TEST);
		g2_glEnable(G2_GL_STENCIL_TEST);

		// init stencil
		g2_glStencilFunc(G2_GL_ALWAYS, 0, 0);
		g2_glStencilOp(G2_GL_INVERT, G2_GL_INVERT, G2_GL_INVERT);
		g2_glColorMask(G2_GL_FALSE, G2_GL_FALSE, G2_GL_FALSE, G2_GL_FALSE);

		// use stencil 
		fill->flag |= G2_GL_FILL_FLAG_STENCIL;

		// init the pass value
		fill->pass = 0xff;
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_bounds(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// init vertices
	fill->vertices[0] = bounds->x1;
	fill->vertices[1] = bounds->y1;
	fill->vertices[2] = bounds->x2;
	fill->vertices[3] = bounds->y1;
	fill->vertices[4] = bounds->x1;
	fill->vertices[5] = bounds->y2;
	fill->vertices[6] = bounds->x2;
	fill->vertices[7] = bounds->y2;
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// draw 
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 4);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_rect(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// check
	tb_assert_and_check_return(item->type == G2_CLIPPER_ITEM_RECT);
	
	// init bounds
	g2_gl_rect_t bounds;
	bounds.x1 = g2_float_to_tb(item->u.rect.x);
	bounds.y1 = g2_float_to_tb(item->u.rect.y);
	bounds.x2 = g2_float_to_tb(item->u.rect.x + item->u.rect.w);
	bounds.y2 = g2_float_to_tb(item->u.rect.y + item->u.rect.h);
	tb_check_return(bounds.x1 < bounds.x2 && bounds.y1 < bounds.y2);

	// clip bounds
	g2_gl_fill_stencil_clip_bounds(fill, &bounds);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_triangle(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// check
	tb_assert_and_check_return(item->type == G2_CLIPPER_ITEM_TRIANGLE);
	
	// init vertices
	fill->vertices[0] = g2_float_to_tb(item->u.triangle.p0.x);
	fill->vertices[1] = g2_float_to_tb(item->u.triangle.p0.y);
	fill->vertices[2] = g2_float_to_tb(item->u.triangle.p1.x);
	fill->vertices[3] = g2_float_to_tb(item->u.triangle.p1.y);
	fill->vertices[4] = g2_float_to_tb(item->u.triangle.p2.x);
	fill->vertices[5] = g2_float_to_tb(item->u.triangle.p2.y);
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// draw 
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 3);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_path(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// check
	tb_assert_and_check_return(item->type == G2_CLIPPER_ITEM_PATH);

	// the clip path
	g2_gl_path_t* path = (g2_gl_path_t*)item->u.path;
	tb_assert_and_check_return(path);

	// null?
	tb_check_return(!g2_path_null(path));

	// like
	g2_gl_path_make_like(path);

	// like rect?
	if (path->like == G2_GL_PATH_LIKE_RECT)
	{
		// clip bounds
		g2_gl_fill_stencil_clip_bounds(fill, &path->rect);

		// ok
		return ;
	}
	// like triangle?
	else if (path->like == G2_GL_PATH_LIKE_TRIG)
	{
		// clip triangle
		g2_clipper_item_t clip = {0};
		clip.type = G2_CLIPPER_ITEM_TRIANGLE;
		clip.mode = item->mode;
		clip.u.triangle = path->trig;
		g2_gl_fill_stencil_clip_triangle(fill, &clip);

		// ok
		return ;
	}

	// make fill
	if (!g2_gl_path_make_fill(path)) return ;

	// check
	tb_assert(path->fill.data && tb_vector_size(path->fill.data));
	tb_assert(path->fill.size && tb_vector_size(path->fill.size));
	tb_check_return(path->fill.rect.x1 < path->fill.rect.x2 && path->fill.rect.y1 < path->fill.rect.y2);
	
	// init vertices
	if (fill->context->version < 0x20)
		g2_glVertexPointer(2, G2_GL_FLOAT, 0, tb_vector_data(path->fill.data));
	else g2_glVertexAttribPointer(g2_gl_program_location(fill->program, G2_GL_PROGRAM_LOCATION_VERTICES), 2, G2_GL_FLOAT, G2_GL_FALSE, 0, tb_vector_data(path->fill.data));

	// clip path
	tb_size_t 	head = 0;
	tb_size_t 	size = 0;
	tb_size_t 	itor = tb_iterator_head(path->fill.size);
	tb_size_t 	tail = tb_iterator_tail(path->fill.size);
	for (; itor != tail; itor++)
	{
		size = tb_iterator_item(path->fill.size, itor);
		g2_glDrawArrays(G2_GL_TRIANGLE_FAN, (g2_GLint_t)head, (g2_GLint_t)size);
		head += size;
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_circle(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// check
	tb_assert_and_check_return(item->type == G2_CLIPPER_ITEM_CIRCLE);

	// init shape
	g2_shape_t shape = {0};
	shape.type = G2_SHAPE_TYPE_CIRCLE;
	shape.u.circle = item->u.circle;

	// get path from pcache first
	tb_handle_t path = g2_pcache_get(fill->painter->pcache, &shape);
	if (!path)
	{
		// init path from cache
		path = g2_pcache_add(fill->painter->pcache, &shape);
		tb_assert_and_check_return(path);

		// add circle to path
		g2_path_add_circle(path, &item->u.circle);
	}
	
	// clip path
	g2_clipper_item_t clip = {0};
	clip.type = G2_CLIPPER_ITEM_PATH;
	clip.mode = item->mode;
	clip.u.path = path;
	g2_gl_fill_stencil_clip_path(fill, &clip);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_ellipse(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// check
	tb_assert_and_check_return(item->type == G2_CLIPPER_ITEM_ELLIPSE);
	
	// init shape
	g2_shape_t shape = {0};
	shape.type = G2_SHAPE_TYPE_ELLIPSE;
	shape.u.ellipse = item->u.ellipse;

	// get path from pcache first
	tb_handle_t path = g2_pcache_get(fill->painter->pcache, &shape);
	if (!path)
	{
		// init path from cache
		path = g2_pcache_add(fill->painter->pcache, &shape);
		tb_assert_and_check_return(path);

		// add ellipse to path
		g2_path_add_ellipse(path, &item->u.ellipse);
	}
	
	// clip path
	g2_clipper_item_t clip = {0};
	clip.type = G2_CLIPPER_ITEM_PATH;
	clip.mode = item->mode;
	clip.u.path = path;
	g2_gl_fill_stencil_clip_path(fill, &clip);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip_item(g2_gl_fill_t* fill, g2_clipper_item_t const* item)
{
	// the clipper impl
	static tb_void_t (*clip[])(g2_gl_fill_t* , g2_clipper_item_t const* ) = 
	{
		g2_gl_fill_stencil_clip_rect
	,	g2_gl_fill_stencil_clip_path
	,	g2_gl_fill_stencil_clip_circle
	,	g2_gl_fill_stencil_clip_ellipse
	,	g2_gl_fill_stencil_clip_triangle
	};
	tb_assert_and_check_return(item->type < tb_arrayn(clip));

	// apply stencil for draw path
	g2_glStencilFunc(G2_GL_ALWAYS, 0, 0);
	g2_glStencilOp(G2_GL_INVERT, G2_GL_INVERT, G2_GL_INVERT);

	// clip it
	clip[item->type](fill, item);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_clip(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// check
	tb_check_return(fill->flag & G2_GL_FILL_FLAG_STENCIL);

	// no scissor
	tb_check_return(!(fill->flag & G2_GL_FILL_FLAG_SCISSOR));

	// the clipper size
	tb_size_t size = g2_clipper_size(fill->clipper);
	tb_check_return(size);

	// force to done mask
	tb_bool_t fmask = (fill->shader && fill->shader->type == G2_GL_SHADER_TYPE_RADIAL)? tb_true : tb_false;

	/* clip stencil
	 *
	 * invisible: 	0x01 or 0xfe
	 * visible: 	0
	 * invalidate: 	no
	 */
	tb_size_t i = 0;
	for (i = 0; i < size; i++)
	{
		// the clip item
		g2_clipper_item_t const* item = g2_clipper_item(fill->clipper, i);
		if (item)
		{
			// prepare the stencil mask for union mode
			if (item->mode == G2_CLIPPER_MODE_UNION)
			{
				// 0x01 => 0xfe
				g2_glStencilFunc(G2_GL_EQUAL, 0x01, 0xff);
				g2_glStencilOp(G2_GL_KEEP, G2_GL_INVERT, G2_GL_INVERT);
				g2_gl_fill_stencil_clip_bounds(fill, bounds);
			}

			// save vetex matrix
			tb_float_t matrix0[16];
			g2_gl_matrix_copy(matrix0, fill->vmatrix);

			// clip matrix => vertex matrix
			g2_gl_matrix_from(fill->vmatrix, &item->matrix);

			// apply vetex matrix
			g2_gl_fill_apply_vertex_matrix(fill);

			// clip item
			g2_gl_fill_stencil_clip_item(fill, item);

			// restore vetex matrix
			g2_gl_matrix_copy(fill->vmatrix, matrix0);

			// apply vetex matrix
			g2_gl_fill_apply_vertex_matrix(fill);
			
			// filter the stencil mask
			switch (item->mode)
			{
			case G2_CLIPPER_MODE_REPLACE:
				{
					// done mask
					if (i + 1 < size || fmask)
					{
						// 0x00 => 0x01, 0xff => 0x00
						g2_glStencilFunc(G2_GL_EQUAL, 0xff, 0xff);
						g2_glStencilOp(G2_GL_INCR, G2_GL_ZERO, G2_GL_ZERO);
						g2_gl_fill_stencil_clip_bounds(fill, bounds);

						// pass: 0x00
						fill->pass = 0x00;
					}
					// last? using the pass value directly
					else fill->pass = 0xff;
				}
				break;
			case G2_CLIPPER_MODE_INTERSECT:
				{
					// done mask
					if (i + 1 < size || fmask)
					{
						// 0x00 => 0x01
						g2_glStencilFunc(G2_GL_EQUAL, 0x00, 0xff);
						g2_glStencilOp(G2_GL_KEEP, G2_GL_INCR, G2_GL_INCR);
						g2_gl_fill_stencil_clip_bounds(fill, bounds);

						// 0xff => 0x00
						g2_glStencilFunc(G2_GL_EQUAL, 0xff, 0xff);
						g2_glStencilOp(G2_GL_KEEP, G2_GL_ZERO, G2_GL_ZERO);
						g2_gl_fill_stencil_clip_bounds(fill, bounds);

						// pass: 0x00
						fill->pass = 0x00;
					}
					// last? using the pass value directly
					else fill->pass = 0xff;
				}
				break;
			case G2_CLIPPER_MODE_UNION:
				{
					// 0x01 => 0x00, 0xff => 0x00
					g2_glStencilFunc(G2_GL_NOTEQUAL, 0xfe, 0xff);
					g2_glStencilOp(G2_GL_KEEP, G2_GL_ZERO, G2_GL_ZERO);
					g2_gl_fill_stencil_clip_bounds(fill, bounds);

					// pass: 0x00
					fill->pass = 0x00;
				}
				break;
			case G2_CLIPPER_MODE_SUBTRACT:
				{
					// done mask
					if (i + 1 < size)
					{
						// 0xff => 0xfe
						g2_glStencilFunc(G2_GL_EQUAL, 0xff, 0xff);
						g2_glStencilOp(G2_GL_KEEP, G2_GL_DECR, G2_GL_DECR);
						g2_gl_fill_stencil_clip_bounds(fill, bounds);

						// pass: 0x00
						fill->pass = 0x00;
					}
					// last? using the pass value directly
					else fill->pass = 0x00;
				}
				break;
			default:
				tb_trace_impl("unknown clip mode: %lu", item->mode);
				break;
			}
		}
	}

	// apply stencil for draw path
	g2_glStencilFunc(G2_GL_ALWAYS, 0, 0);
	g2_glStencilOp(G2_GL_INVERT, G2_GL_INVERT, G2_GL_INVERT);

	// invert pass 
	fill->pass = ~fill->pass;
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_draw(g2_gl_fill_t* fill)
{
	// check
	tb_check_return(fill->flag & G2_GL_FILL_FLAG_STENCIL);

	// draw stencil
	g2_glStencilFunc(G2_GL_EQUAL, (g2_GLint_t)fill->pass, 0xff);
	g2_glStencilOp(G2_GL_ZERO, G2_GL_ZERO, G2_GL_ZERO);
	g2_glColorMask(G2_GL_TRUE, G2_GL_TRUE, G2_GL_TRUE, G2_GL_TRUE);
}
static __tb_inline__ tb_void_t g2_gl_fill_stencil_exit(g2_gl_fill_t* fill)
{
	// check
	tb_check_return(fill->flag & G2_GL_FILL_FLAG_STENCIL);

	// exit
	g2_glColorMask(G2_GL_TRUE, G2_GL_TRUE, G2_GL_TRUE, G2_GL_TRUE);
	g2_glDisable(G2_GL_STENCIL_TEST);
}
static __tb_inline__ tb_void_t g2_gl_fill_scissor_init(g2_gl_fill_t* fill)
{
	// only one rect and no rotate?
	if (g2_clipper_size(fill->clipper) == 1)
	{
		g2_clipper_item_t const* item = (g2_clipper_item_t const*)g2_clipper_item(fill->clipper, 0);
		if (item && item->type == G2_CLIPPER_ITEM_RECT && g2_ez(item->matrix.kx) && g2_ez(item->matrix.ky)) 
		{
			// the height
			tb_size_t height = g2_bitmap_height(fill->context->surface);
			tb_assert_and_check_return(height);

			// enable scissor
			fill->flag |= G2_GL_FILL_FLAG_SCISSOR;
			g2_glEnable(G2_GL_SCISSOR_TEST);
			
			// transform rect
			g2_float_t ox1 = item->u.rect.x;
			g2_float_t oy1 = item->u.rect.y;
			g2_float_t ox2 = item->u.rect.x + item->u.rect.w;
			g2_float_t oy2 = item->u.rect.y + item->u.rect.h;
			g2_float_t nx1 = g2_matrix_apply_x(&item->matrix, ox1, oy1);
			g2_float_t ny1 = g2_matrix_apply_y(&item->matrix, ox1, oy1);
			g2_float_t nx2 = g2_matrix_apply_x(&item->matrix, ox2, oy2);
			g2_float_t ny2 = g2_matrix_apply_y(&item->matrix, ox2, oy2);

			// scissor, @note y-position for gl
			g2_glScissor(g2_float_to_tb(nx1), (tb_float_t)height - g2_float_to_tb(ny2), g2_float_to_tb(nx2 - nx1), g2_float_to_tb(ny2 - ny1));
		}
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_scissor_exit(g2_gl_fill_t* fill)
{
	if (fill->flag & G2_GL_FILL_FLAG_SCISSOR) g2_glDisable(G2_GL_SCISSOR_TEST);
}
static __tb_inline__ tb_bool_t g2_gl_fill_context_init(g2_gl_fill_t* fill)
{
	// has fill?
	tb_check_return_val(g2_style_mode(fill->style) & G2_STYLE_MODE_FILL, tb_false);

	// apply program first
	if (!g2_gl_fill_apply_program(fill)) return tb_false;

	// enter vertex state
	g2_gl_fill_enter_vertex_state(fill);

	// apply solid if no shader
	if (!fill->shader) g2_gl_fill_apply_solid(fill);
	
	// ok
	return tb_true;
}
static __tb_inline__ tb_void_t g2_gl_fill_context_exit(g2_gl_fill_t* fill)
{
	// leave vertex state
	g2_gl_fill_leave_vertex_state(fill);

	// leave texture state
	g2_gl_fill_leave_texture_state(fill);

	// disable blend
	g2_glDisable(G2_GL_BLEND);
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw_color(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// apply solid
	g2_gl_fill_apply_solid(fill);

	// init vertices
	fill->vertices[0] = bounds->x1;
	fill->vertices[1] = bounds->y1;
	fill->vertices[2] = bounds->x2;
	fill->vertices[3] = bounds->y1;
	fill->vertices[4] = bounds->x1;
	fill->vertices[5] = bounds->y2;
	fill->vertices[6] = bounds->x2;
	fill->vertices[7] = bounds->y2;
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// draw
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 4);
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw_shader_bitmap(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// enter texture state
	g2_gl_fill_enter_texture_state(fill);

	// enter texture matrix
	g2_gl_fill_enter_texture_matrix(fill);

	// init texcoords
	fill->texcoords[0] = 0.0f;
	fill->texcoords[1] = 0.0f;
	fill->texcoords[2] = 1.0f;
	fill->texcoords[3] = 0.0f;
	fill->texcoords[4] = 0.0f;
	fill->texcoords[5] = 1.0f;
	fill->texcoords[6] = 1.0f;
	fill->texcoords[7] = 1.0f;

	// apply texcoords
	g2_gl_fill_apply_texcoords(fill);

	// init vertices
	fill->vertices[0] = bounds->x1;
	fill->vertices[1] = bounds->y1;
	fill->vertices[2] = bounds->x2;
	fill->vertices[3] = bounds->y1;
	fill->vertices[4] = bounds->x1;
	fill->vertices[5] = bounds->y2;
	fill->vertices[6] = bounds->x2;
	fill->vertices[7] = bounds->y2;
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// apply texture matrix
	g2_gl_fill_apply_texture_matrix(fill, bounds);

	// draw
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 4);

	// leave texture matrix
	g2_gl_fill_leave_texture_matrix(fill);

	// leave texture state
	g2_gl_fill_leave_texture_state(fill);
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw_shader_linear(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// enter texture state
	g2_gl_fill_enter_texture_state(fill);

	// enter texture matrix
	g2_gl_fill_enter_texture_matrix(fill);

	// init texcoords
	fill->texcoords[0] = 0.0f;
	fill->texcoords[1] = 0.0f;
	fill->texcoords[2] = 1.0f;
	fill->texcoords[3] = 0.0f;
	fill->texcoords[4] = 0.0f;
	fill->texcoords[5] = 1.0f;
	fill->texcoords[6] = 1.0f;
	fill->texcoords[7] = 1.0f;

	// apply texcoords
	g2_gl_fill_apply_texcoords(fill);

	// init vertices
	fill->vertices[0] = bounds->x1;
	fill->vertices[1] = bounds->y1;
	fill->vertices[2] = bounds->x2;
	fill->vertices[3] = bounds->y1;
	fill->vertices[4] = bounds->x1;
	fill->vertices[5] = bounds->y2;
	fill->vertices[6] = bounds->x2;
	fill->vertices[7] = bounds->y2;
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// apply texture matrix
	g2_gl_fill_apply_texture_matrix(fill, bounds);

	// draw
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 4);

	// leave texture matrix
	g2_gl_fill_leave_texture_matrix(fill);

	// leave texture state
	g2_gl_fill_leave_texture_state(fill);
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw_shader_radial(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// enter texture state
	g2_gl_fill_enter_texture_state(fill);

	// enter texture matrix
	g2_gl_fill_enter_texture_matrix(fill);

	// init texcoords
	fill->texcoords[0] = 0.0f;
	fill->texcoords[1] = 0.0f;
	fill->texcoords[2] = 1.0f;
	fill->texcoords[3] = 0.0f;
	fill->texcoords[4] = 0.0f;
	fill->texcoords[5] = 1.0f;
	fill->texcoords[6] = 1.0f;
	fill->texcoords[7] = 1.0f;

	// apply texcoords
	g2_gl_fill_apply_texcoords(fill);

	// init radial variables
	tb_float_t smatrix[16];
	g2_gl_matrix_from(smatrix, &fill->shader->matrix);
	tb_float_t cx = g2_float_to_tb(fill->shader->u.radial.cp.c.x);
	tb_float_t cy = g2_float_to_tb(fill->shader->u.radial.cp.c.y);
	tb_float_t x0 = g2_gl_matrix_apply_x(smatrix, cx, cy);
	tb_float_t y0 = g2_gl_matrix_apply_y(smatrix, cx, cy);

	// init scale factor
	tb_float_t sx = tb_fabs(smatrix[0]);
	tb_float_t sy = tb_fabs(smatrix[5]);
	tb_float_t fs = tb_min(sx, sy);
	if (fs < 1e-9) fs = 1e-9;

	// init maximum radius 
	tb_float_t n1 = (x0 - bounds->x1) * (x0 - bounds->x1) + (y0 - bounds->y1) * (y0 - bounds->y1);
	tb_float_t n2 = (x0 - bounds->x2) * (x0 - bounds->x2) + (y0 - bounds->y1) * (y0 - bounds->y1);
	tb_float_t n3 = (x0 - bounds->x1) * (x0 - bounds->x1) + (y0 - bounds->y2) * (y0 - bounds->y2);
	tb_float_t n4 = (x0 - bounds->x2) * (x0 - bounds->x2) + (y0 - bounds->y2) * (y0 - bounds->y2);
	if (n2 > n1) n1 = n2;
	if (n3 > n1) n1 = n3; 
	if (n4 > n1) n1 = n4; 
	tb_float_t rm = (tb_float_t)(tb_isqrti(tb_ceil(n1)) + 1) / fs;

	// the radial factor
	static g2_gl_fill_radial_factor_t factors[] =
	{
		{0.105396307f, 	12.0f, 	30} // rm * sin(6.05)
	,	{0.070626986f, 	8.0f, 	45} // rm * sin(4.05)
	,	{0.035771616f, 	4.0f, 	90} // rm * sin(2.05)
	};
	tb_assert(g2_quality() < tb_arrayn(factors));
	g2_gl_fill_radial_factor_t const* factor = &factors[g2_quality()];

	/* init fragment vertices
	 *        
	 *        fn
	 * *****|*****
	 *  *   |   *
	 *   *rm|  *
	 *    * | *
	 *     *|*
	 *      *
	 */
	tb_float_t fn = rm * factor->factor; // rm * sin(x.05)
	fill->vertices[0] = cx - fn;
	fill->vertices[1] = cy - rm;
	fill->vertices[2] = cx + fn;
	fill->vertices[3] = cy - rm;
	fill->vertices[4] = cx;
	fill->vertices[5] = cy;

	// init fragment bounds
	g2_gl_rect_t fbounds;
	g2_gl_rect_init(&fbounds, fill->vertices[0], fill->vertices[1]);
	g2_gl_rect_done(&fbounds, fill->vertices[2], fill->vertices[3]);
	g2_gl_rect_done(&fbounds, fill->vertices[4], fill->vertices[5]);
	
	// apply vertices
	g2_gl_fill_apply_vertices(fill);

	// apply texture matrix
	g2_gl_fill_apply_texture_matrix(fill, &fbounds);

	// save vetex matrix
	tb_float_t matrix0[16];
	g2_gl_matrix_copy(matrix0, fill->vmatrix);

	// apply shader matrix
	g2_gl_matrix_multiply(fill->vmatrix, smatrix);

	// init rotate matrix: rotate one degress
	tb_float_t matrix1[16];
	g2_gl_matrix_init_rotatep(matrix1, factor->rotation, cx, cy);

	// rotate for drawing all fragments
	tb_size_t n = factor->count;
	while (n--)
	{
		// rotate one degress
		g2_gl_matrix_multiply(fill->vmatrix, matrix1);

		// apply vetex matrix
		g2_gl_fill_apply_vertex_matrix(fill);

		// draw fragment
		g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 3);

	}

	// restore vetex matrix
	g2_gl_matrix_copy(fill->vmatrix, matrix0);

	// apply vetex matrix
	g2_gl_fill_apply_vertex_matrix(fill);

	// leave texture matrix
	g2_gl_fill_leave_texture_matrix(fill);

	// leave texture state
	g2_gl_fill_leave_texture_state(fill);
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw_shader(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// draw shader
	switch (fill->shader->type)
	{
	case G2_GL_SHADER_TYPE_BITMAP:
		g2_gl_fill_style_draw_shader_bitmap(fill, bounds);
		break;
	case G2_GL_SHADER_TYPE_LINEAR:
		g2_gl_fill_style_draw_shader_linear(fill, bounds);
		break;
	case G2_GL_SHADER_TYPE_RADIAL:
		g2_gl_fill_style_draw_shader_radial(fill, bounds);
		break;
		break;
	default:
		break;
	}
}
static __tb_inline__ tb_void_t g2_gl_fill_style_draw(g2_gl_fill_t* fill, g2_gl_rect_t const* bounds)
{
	// rect or stencil?
	tb_check_return(fill->flag & (G2_GL_FILL_FLAG_RECT | G2_GL_FILL_FLAG_STENCIL));

	// draw
	if (fill->shader) g2_gl_fill_style_draw_shader(fill, bounds);
	else g2_gl_fill_style_draw_color(fill, bounds);
}
static __tb_inline__ tb_bool_t g2_gl_fill_init(g2_gl_fill_t* fill, g2_gl_painter_t* painter, tb_size_t flag)
{
	// init fill
	fill->painter 	= painter;
	fill->context 	= painter->context;
	fill->style 	= g2_style(painter);
	fill->shader 	= g2_style_shader(fill->style);
	fill->clipper 	= g2_clipper(painter);
	fill->flag 		= flag;
	g2_gl_matrix_from(fill->vmatrix, &fill->painter->matrix);
	tb_assert_and_check_return_val(fill->painter && fill->context && fill->context->surface && fill->style && fill->clipper, tb_false);

	// use stencil?
	if (!(flag & G2_GL_FILL_FLAG_CONVEX)) fill->flag |= G2_GL_FILL_FLAG_STENCIL;
	else if (flag & G2_GL_FILL_FLAG_RECT)
	{ 
		if (fill->shader && fill->shader->type == G2_GL_SHADER_TYPE_RADIAL) fill->flag |= G2_GL_FILL_FLAG_STENCIL;
	}
	else if (fill->shader) fill->flag |= G2_GL_FILL_FLAG_STENCIL;

	// init context
	if (!g2_gl_fill_context_init(fill)) return tb_false;

	// enter vetex matrix
	g2_gl_fill_enter_vertex_matrix(fill);

	// apply vetex matrix
	g2_gl_fill_apply_vertex_matrix(fill);

	// init scissor
	g2_gl_fill_scissor_init(fill);

	// init stencil
	g2_gl_fill_stencil_init(fill);

	// ok
	return tb_true;
}
static __tb_inline__ tb_void_t g2_gl_fill_clip(g2_gl_fill_t* fill, g2_gl_rect_t* bounds)
{
	// clip stencil
	g2_gl_fill_stencil_clip(fill, bounds);
}
static __tb_inline__ tb_void_t g2_gl_fill_draw(g2_gl_fill_t* fill, g2_gl_rect_t* bounds)
{
	// draw stencil
	g2_gl_fill_stencil_draw(fill);

	// draw style
	g2_gl_fill_style_draw(fill, bounds);
}
static __tb_inline__ tb_void_t g2_gl_fill_exit(g2_gl_fill_t* fill)
{
	// exit scissor
	g2_gl_fill_scissor_exit(fill);

	// leave vetex matrix
	g2_gl_fill_leave_vertex_matrix(fill);

	// exit stencil
	g2_gl_fill_stencil_exit(fill);

	// exit context
	g2_gl_fill_context_exit(fill);
}
static tb_void_t g2_gl_fill_bounds(g2_gl_painter_t* painter, g2_gl_rect_t const* bounds)
{
	// init fill
	g2_gl_fill_t fill = {0};
	if (!g2_gl_fill_init(&fill, painter, G2_GL_FILL_FLAG_RECT | G2_GL_FILL_FLAG_CONVEX)) return ;

	// clip fill
	g2_gl_fill_clip(&fill, bounds);

	// use stencil?
	if (fill.flag & G2_GL_FILL_FLAG_STENCIL)
	{
		// init vertices
		fill.vertices[0] = bounds->x1;
		fill.vertices[1] = bounds->y1;
		fill.vertices[2] = bounds->x2;
		fill.vertices[3] = bounds->y1;
		fill.vertices[4] = bounds->x1;
		fill.vertices[5] = bounds->y2;
		fill.vertices[6] = bounds->x2;
		fill.vertices[7] = bounds->y2;
		
		// apply vertices
		g2_gl_fill_apply_vertices(&fill);

		// draw
		g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 4);
	}

	// draw fill
	g2_gl_fill_draw(&fill, bounds);

	// exit fill
	g2_gl_fill_exit(&fill);
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t g2_gl_fill_rect(g2_gl_painter_t* painter, g2_rect_t const* rect)
{
	// init bounds
	g2_gl_rect_t bounds;
	bounds.x1 = g2_float_to_tb(rect->x);
	bounds.y1 = g2_float_to_tb(rect->y);
	bounds.x2 = g2_float_to_tb(rect->x + rect->w);
	bounds.y2 = g2_float_to_tb(rect->y + rect->h);
	tb_check_return(bounds.x1 < bounds.x2 && bounds.y1 < bounds.y2);

	// fill bounds
	g2_gl_fill_bounds(painter, &bounds);
}
tb_void_t g2_gl_fill_path(g2_gl_painter_t* painter, g2_gl_path_t const* path)
{
	// check
	tb_assert(path->fill.data && tb_vector_size(path->fill.data));
	tb_assert(path->fill.size && tb_vector_size(path->fill.size));
	tb_check_return(path->fill.rect.x1 < path->fill.rect.x2 && path->fill.rect.y1 < path->fill.rect.y2);
	
	// like rect?
	if (path->like == G2_GL_PATH_LIKE_RECT)
	{
		// fill bounds
		g2_gl_fill_bounds(painter, &path->rect);

		// ok
		return ;
	}
	// like triangle?
	else if (path->like == G2_GL_PATH_LIKE_TRIG)
	{
		// fill triangle
		g2_gl_fill_triangle(painter, &path->trig);

		// ok
		return ;
	}

	// init fill
	g2_gl_fill_t fill = {0};
	if (!g2_gl_fill_init(&fill, painter, path->like == G2_GL_PATH_LIKE_CONX? G2_GL_FILL_FLAG_CONVEX : G2_GL_FILL_FLAG_NONE)) return ;

	// clip fill
	g2_gl_fill_clip(&fill, &path->fill.rect);

	// init vertices
	if (fill.context->version < 0x20)
		g2_glVertexPointer(2, G2_GL_FLOAT, 0, tb_vector_data(path->fill.data));
	else g2_glVertexAttribPointer(g2_gl_program_location(fill.program, G2_GL_PROGRAM_LOCATION_VERTICES), 2, G2_GL_FLOAT, G2_GL_FALSE, 0, tb_vector_data(path->fill.data));

	// draw path
	tb_size_t 	head = 0;
	tb_size_t 	size = 0;
	tb_size_t 	itor = tb_iterator_head(path->fill.size);
	tb_size_t 	tail = tb_iterator_tail(path->fill.size);
	for (; itor != tail; itor++)
	{
		size = tb_iterator_item(path->fill.size, itor);
		g2_glDrawArrays(G2_GL_TRIANGLE_FAN, (g2_GLint_t)head, (g2_GLint_t)size);
		head += size;
	}

	// draw fill
	g2_gl_fill_draw(&fill, &path->fill.rect);

	// exit fill
	g2_gl_fill_exit(&fill);
}
tb_void_t g2_gl_fill_triangle(g2_gl_painter_t* painter, g2_triangle_t const* triangle)
{
	// init vertices
	tb_float_t vertices[6];
	vertices[0] = g2_float_to_tb(triangle->p0.x);
	vertices[1] = g2_float_to_tb(triangle->p0.y);
	vertices[2] = g2_float_to_tb(triangle->p1.x);
	vertices[3] = g2_float_to_tb(triangle->p1.y);
	vertices[4] = g2_float_to_tb(triangle->p2.x);
	vertices[5] = g2_float_to_tb(triangle->p2.y);

	// init bounds
	g2_gl_rect_t bounds;
	g2_gl_rect_init(&bounds, vertices[0], vertices[1]);
	g2_gl_rect_done(&bounds, vertices[2], vertices[3]);
	g2_gl_rect_done(&bounds, vertices[4], vertices[5]);
	tb_check_return(bounds.x1 < bounds.x2 && bounds.y1 < bounds.y2);

	// init fill
	g2_gl_fill_t fill = {0};
	if (!g2_gl_fill_init(&fill, painter, G2_GL_FILL_FLAG_CONVEX)) return ;

	// clip fill
	g2_gl_fill_clip(&fill, &bounds);

	// draw vertices
	if (fill.context->version < 0x20) g2_glVertexPointer(2, G2_GL_FLOAT, 0, vertices);
	else g2_glVertexAttribPointer(g2_gl_program_location(fill.program, G2_GL_PROGRAM_LOCATION_VERTICES), 2, G2_GL_FLOAT, G2_GL_FALSE, 0, vertices);
	g2_glDrawArrays(G2_GL_TRIANGLE_STRIP, 0, 3);

	// draw fill
	g2_gl_fill_draw(&fill, &bounds);

	// exit fill
	g2_gl_fill_exit(&fill);
}

