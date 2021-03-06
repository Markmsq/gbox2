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
 * @file		shader.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "shader.h"
#include "context.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the gradient texcoord size
#define G2_GL_SHADER_GRAD_TEXCOORD_SIZE 			(1024)

// the gradient stop maxn
#define G2_GL_SHADER_GRAD_STOP_MAXN 				(1024)

/* ///////////////////////////////////////////////////////////////////////
 * gl interfaces
 */

static __tb_inline__ g2_gl_shader_t* g2_gl_shader_init(tb_handle_t context, tb_size_t type, tb_size_t width, tb_size_t height, tb_size_t wrap)
{
	// check
	g2_gl_context_t* gcontext = (g2_gl_context_t*)context;
	tb_assert_and_check_return_val(gcontext && type, tb_null);

	// make texture
	g2_GLuint_t* texture = g2_gl_context_texture_alc(context);
	tb_assert_and_check_return_val(texture, tb_null);

	// make shader
	g2_gl_shader_t* shader = g2_gl_context_shader_alc(context);
	tb_assert_and_check_return_val(shader, tb_null);

	// init shader
	shader->type 		= type;
	shader->wrap 		= wrap;
	shader->refn 		= 1;
	shader->texture 	= texture;
	shader->context 	= context;
	shader->width 		= width;
	shader->height 		= height;
	shader->flag 		= G2_GL_SHADER_FLAG_NONE;

	// init matrix
	g2_matrix_clear(&shader->matrix);

	// init matrix for gl
	g2_gl_matrix_clear(shader->matrix_gl);

	// ok
	return shader;
}
static __tb_inline__ tb_void_t g2_gl_shader_exit(g2_gl_shader_t* shader)
{
	if (shader && shader->context)
	{
		// exit texture
		if (shader->texture) 
			g2_gl_context_texture_del(shader->context, shader->texture);

		// exit shader
		g2_gl_context_shader_del(shader->context, shader);
	}
}
static __tb_inline__ tb_size_t g2_shader_make_stops(g2_gradient_t const* gradient, g2_gl_stop_t* stops, tb_size_t maxn, tb_bool_t* alpha)
{
	// init
	tb_size_t 		stopn = 0;
	g2_float_t* 	radio = gradient->radio;
	g2_color_t* 	color = gradient->color;
	tb_size_t 		count = gradient->count;
	tb_assert(count < maxn);

	// no radio? make the average stops
	if (!radio)
	{
		tb_size_t i = 0;
		tb_bool_t a = tb_false;
		tb_size_t n = tb_min(count, maxn);
		if (n > 1)
		{
			for (i = 0; i < n; i++)
			{
				// add stop
				stops[i].radio = (i * G2_ONE) / (n - 1);
				stops[i].color = color[i];

				// has alpha?
				if (!a && color[i].a != 0xff) a = tb_true;
			}

			// stopn
			stopn = i;
		}
		else if (n)
		{
			// add stop
			stops[0].radio = 0;
			stops[0].color = color[0];

			// add stop
			stops[1].radio = G2_ONE;
			stops[1].color = color[0];

			// stopn
			stopn = 2;

			// has alpha?
			if (color[0].a != 0xff) a = tb_true;
		}

		// alpha?
		*alpha = a;
	}
	// check and patch radio
	else
	{
		tb_size_t 	i = 0;
		g2_float_t 	last = 0;
		tb_bool_t 	a = tb_false;
		for (i = 0; i < count && stopn < maxn; i++)
		{
			// check range, must be [0, 1], skip it if out of range
			if (g2_lz(radio[i]) || g2_b1(radio[i])) continue ;

			// check repeat, skip it if repeat
			if (i && radio[i] == last) continue ;

			// check order, discard whole sequence if not in ascending order
			if (radio[i] < last) 
			{
				stopn = 0;
				break ;
			}

			// add stop at offset 0 with same color if first not at 0
			if (!i && g2_nz(radio[0]))
			{
				stops[stopn].radio = 0;
				stops[stopn].color = color[0];
				stopn++;
			}

			// add stop
			stops[stopn].radio = radio[i];
			stops[stopn].color = color[i];
			stopn++;

			// has alpha?
			if (!a && color[i].a != 0xff) a = tb_true;

			// save the last radio
			last = radio[i];
		}

		// add stop at offset 1 with same color if last not at 1
		if (stopn && stopn < maxn && g2_n1(last))
		{
			stops[stopn].radio = G2_ONE;
			stops[stopn].color = color[count - 1];
			stopn++;
		}

		// alpha?
		*alpha = a;
	}


	// ok?
	return stopn;
}
static __tb_inline__ tb_void_t g2_shader_make_color_interpolation(tb_byte_t* pb, tb_byte_t* pe, g2_color_t cb, g2_color_t ce)
{
	// init width
	tb_long_t pw = (pe - pb) >> 2;
	tb_long_t rw = (ce.r - cb.r);
	tb_long_t gw = (ce.g - cb.g);
	tb_long_t bw = (ce.b - cb.b);
	tb_long_t aw = (ce.a - cb.a);
	
	// init scale
	tb_long_t rs = 1;
	tb_long_t gs = 1;
	tb_long_t bs = 1;
	tb_long_t as = 1;

	// adjust width & scale
	if (!pw) pw = 1;
	if (rw < 0)
	{
		rw = -rw;
		rs = -rs;
	}
	if (gw < 0) 
	{
		gw = -gw;
		gs = -gs;
	}
	if (bw < 0) 
	{
		bw = -bw;
		bs = -bs;
	}
	if (aw < 0) 
	{
		aw = -aw;
		as = -as;
	}

	// init step
	tb_long_t rn = 1;
	tb_long_t gn = 1;
	tb_long_t bn = 1;
	tb_long_t an = 1;
	if (rw > pw) rn = rw / pw + 1;
	if (gw > pw) gn = gw / pw + 1;
	if (bw > pw) bn = bw / pw + 1;
	if (aw > pw) bn = aw / pw + 1;

	// walk
	tb_long_t pi = 0;
	tb_long_t ri = 0;
	tb_long_t gi = 0;
	tb_long_t bi = 0;
	tb_long_t ai = 0;
	for (; pb < pe; pb += 4, pi++)
	{
		// do interpolation
		pb[0] = cb.r + rs * ri;
		pb[1] = cb.g + gs * gi;
		pb[2] = cb.b + bs * bi;
		pb[3] = cb.a + as * ai;

		// next
		if (rw * pi > ri * pw) ri += rn;
		if (gw * pi > gi * pw) gi += gn;
		if (bw * pi > bi * pw) bi += bn;
		if (aw * pi > ai * pw) ai += an;
	}
}
static __tb_inline__ tb_bool_t g2_shader_make_color(tb_byte_t* data, tb_size_t size, g2_gl_stop_t const* stops, tb_size_t stopn)
{
	// check
	tb_assert_and_check_return_val(data && size && stops && stopn, tb_false);

	// init
	tb_size_t 	i = 0;
	tb_byte_t* 	pb = data;
	tb_byte_t* 	pe = data;
	g2_color_t 	cb = stops[0].color;
	g2_color_t 	ce = cb;
	for (i = 1; i < stopn; ++i, cb = ce, pb = pe)
	{
		// [pb, pe)
		pe = data + tb_align4(g2_float_to_long(stops[i].radio * size));
		tb_assert(pb < pe && pe <= data + size);

		// [cb, ce)
		ce = stops[i].color;

		// do interpolation
		g2_shader_make_color_interpolation(pb, pe, cb, ce);
	}

	// ok
	return tb_true;
}
static __tb_inline__ tb_bool_t g2_shader_make_gradient(g2_gl_shader_t* shader, g2_gradient_t const* gradient, tb_byte_t* data, tb_size_t size, tb_bool_t* alpha)
{
	// make stops
	g2_gl_stop_t 	stops[G2_GL_SHADER_GRAD_STOP_MAXN];
	tb_size_t 		stopn = g2_shader_make_stops(gradient, stops, G2_GL_SHADER_GRAD_STOP_MAXN, alpha);
	tb_assert_and_check_return_val(stopn, tb_false);

	// make color
	return g2_shader_make_color(data, size, stops, stopn);
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t g2_shader_init_linear(tb_handle_t context, g2_point_t const* pb, g2_point_t const* pe, g2_gradient_t const* gradient, tb_size_t wrap)
{
	// check
	tb_assert_and_check_return_val(context && pb && pe && gradient && gradient->color && gradient->count && wrap, tb_null);

	// init shader
	g2_gl_shader_t* shader = g2_gl_shader_init(context, G2_GL_SHADER_TYPE_LINEAR, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, wrap);
	tb_assert_and_check_return_val(shader && shader->texture, tb_null);

	// init linear
	shader->u.linear.pb = *pb;
	shader->u.linear.pe = *pe;

	// make gradient
	tb_bool_t 		alpha = tb_false;
	tb_byte_t 		data[G2_GL_SHADER_GRAD_TEXCOORD_SIZE << 2];
	if (!g2_shader_make_gradient(shader, gradient, data, tb_arrayn(data), &alpha)) goto fail;

	// alpha?
	if (alpha) shader->flag |= G2_GL_SHADER_FLAG_ALPHA;

	// make texture
	g2_glBindTexture(G2_GL_TEXTURE_2D, *shader->texture);

	// init line alignment
	g2_glPixelStorei(G2_GL_UNPACK_ALIGNMENT, 4);
	
	// make data
	g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGBA, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, 1, 0, G2_GL_RGBA, G2_GL_UNSIGNED_BYTE, data);

	// init matrix
	g2_shader_matrix_set(shader, tb_null);
	
	// ok
	return shader;

fail:
	if (shader) g2_gl_shader_exit(shader);
	return tb_null;
}
tb_handle_t g2_shader_init_radial(tb_handle_t context, g2_circle_t const* cp, g2_gradient_t const* gradient, tb_size_t wrap)
{
	// check
	tb_assert_and_check_return_val(context && cp && gradient && gradient->color && gradient->count && wrap, tb_null);

	// init shader
	g2_gl_shader_t* shader = g2_gl_shader_init(context, G2_GL_SHADER_TYPE_RADIAL, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, wrap);
	tb_assert_and_check_return_val(shader && shader->texture, tb_null);

	// init radial
	shader->u.radial.cp = *cp;
	
	// make gradient
	tb_bool_t 		alpha = tb_false;
	tb_byte_t 		data[G2_GL_SHADER_GRAD_TEXCOORD_SIZE << 2];
	if (!g2_shader_make_gradient(shader, gradient, data, tb_arrayn(data), &alpha)) goto fail;

	// alpha?
	if (alpha) shader->flag |= G2_GL_SHADER_FLAG_ALPHA;

	// make texture
	g2_glBindTexture(G2_GL_TEXTURE_2D, *shader->texture);

	// init line alignment
	g2_glPixelStorei(G2_GL_UNPACK_ALIGNMENT, 4);
	
	// make data
	g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGBA, G2_GL_SHADER_GRAD_TEXCOORD_SIZE, 1, 0, G2_GL_RGBA, G2_GL_UNSIGNED_BYTE, data);

	// init matrix
	g2_shader_matrix_set(shader, tb_null);
	
	// ok
	return shader;

fail:
	if (shader) g2_gl_shader_exit(shader);
	return tb_null;
}
tb_handle_t g2_shader_init_bitmap(tb_handle_t context, tb_handle_t bitmap, tb_size_t wrap)
{
	// check
	tb_assert_and_check_return_val(context && bitmap && wrap, tb_null);

	// data & size
	tb_pointer_t 	data = g2_bitmap_data(bitmap);
	tb_size_t 		size = g2_bitmap_size(bitmap);
	tb_assert_and_check_return_val(data && size, tb_null);

	// width & height 
	tb_size_t 		width = g2_bitmap_width(bitmap);
	tb_size_t 		height = g2_bitmap_height(bitmap);
	tb_assert_and_check_return_val(width && height, tb_null);

	// lpitch
	tb_size_t 		lpitch = g2_bitmap_lpitch(bitmap);
	tb_assert_and_check_return_val(lpitch, tb_null);

	// pixfmt
	tb_size_t 		pixfmt = g2_bitmap_pixfmt(bitmap);
	tb_assert_and_check_return_val( 	(pixfmt == (G2_PIXFMT_RGBA8888 | G2_PIXFMT_BENDIAN))
									|| 	(pixfmt == (G2_PIXFMT_RGB565 | G2_PIXFMT_LENDIAN))
									|| 	(pixfmt == (G2_PIXFMT_RGB888 | G2_PIXFMT_BENDIAN))
									|| 	(pixfmt == (G2_PIXFMT_RGBA4444 | G2_PIXFMT_LENDIAN))
									|| 	(pixfmt == (G2_PIXFMT_RGBA5551 | G2_PIXFMT_LENDIAN)), tb_null);

	// init shader
	g2_gl_shader_t* shader = g2_gl_shader_init(context, G2_GL_SHADER_TYPE_BITMAP, width, height, wrap);
	tb_assert_and_check_return_val(shader && shader->texture, tb_null);

	// init width & height
	shader->u.bitmap.width 		= width;
	shader->u.bitmap.height 	= height;

	// alpha?
	if (g2_bitmap_flag(bitmap) & G2_BITMAP_FLAG_ALPHA)
		shader->flag |= G2_GL_SHADER_FLAG_ALPHA;

	// make texture
	g2_glBindTexture(G2_GL_TEXTURE_2D, *shader->texture);

	// init line alignment
	g2_glPixelStorei(G2_GL_UNPACK_ALIGNMENT, !(lpitch & 0x3)? 4 : 1);
	
	// make data
	switch (G2_PIXFMT(pixfmt))
	{
	case G2_PIXFMT_RGBA8888:
		g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGBA, width, height, 0, G2_GL_RGBA, G2_GL_UNSIGNED_BYTE, data);
		break;
	case G2_PIXFMT_RGB565:
		g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGB, width, height, 0, G2_GL_RGB, G2_GL_UNSIGNED_SHORT_5_6_5, data);
		break;
	case G2_PIXFMT_RGB888:
		g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGB, width, height, 0, G2_GL_RGB, G2_GL_UNSIGNED_BYTE, data);
		break;
	case G2_PIXFMT_RGBA4444:
		g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGBA, width, height, 0, G2_GL_RGBA, G2_GL_UNSIGNED_SHORT_4_4_4_4, data);
		break;
	case G2_PIXFMT_RGBA5551:
		g2_glTexImage2D(G2_GL_TEXTURE_2D, 0, G2_GL_RGBA, width, height, 0, G2_GL_RGBA, G2_GL_UNSIGNED_SHORT_5_5_5_1, data);
		break;
	default:
		tb_assert_and_check_return_val(0, tb_null);
		break;
	}

	// init matrix
	g2_shader_matrix_set(shader, tb_null);
	
	// ok
	return shader;
}
tb_void_t g2_shader_exit(tb_handle_t shader)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return(gshader);

	// refn--
	g2_shader_dec(shader);
}
g2_matrix_t const* g2_shader_matrix(tb_handle_t shader)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return_val(gshader, tb_null);

	return &gshader->matrix;
}
tb_void_t g2_shader_matrix_set(tb_handle_t shader, g2_matrix_t const* matrix)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return(gshader);

	// update matrix
	if (matrix) gshader->matrix = *matrix;
	else g2_matrix_clear(&gshader->matrix);

	// update matrix for gl
	switch (gshader->type)
	{
	case G2_GL_SHADER_TYPE_BITMAP:
		{
			// init
			g2_matrix_t mx = gshader->matrix;
			tb_float_t 	sw = gshader->width;
			tb_float_t 	sh = gshader->height;

			// matrix: global => camera for gl
			if (g2_matrix_invert(&mx))
			{
				mx.tx /= sw;
				mx.ty /= sh;

				// g2 matrix => gl matrix
				g2_gl_matrix_from(gshader->matrix_gl, &mx);
			}
		}
		break;
	case G2_GL_SHADER_TYPE_LINEAR:
		{
			// init
			g2_matrix_t mx = gshader->matrix;
			g2_point_t 	pb = gshader->u.linear.pb;
			g2_point_t 	pe = gshader->u.linear.pe;
			tb_float_t 	ux = g2_float_to_tb(pe.x - pb.x);
			tb_float_t 	uy = g2_float_to_tb(pe.y - pb.y);
			tb_float_t 	un = tb_sqrtf(ux * ux + uy * uy);
			tb_float_t 	sw = gshader->width;
			tb_float_t 	sh = gshader->height;

			// apply the linear matrix
			g2_matrix_translate(&mx, pb.x, pb.y);
			g2_matrix_scale(&mx, tb_float_to_g2(un / sw), tb_float_to_g2(un / sh));
			g2_matrix_sincos(&mx, tb_float_to_g2(uy / un), tb_float_to_g2(ux / un));
			
			// matrix: global => camera for gl
			if (g2_matrix_invert(&mx))
			{
				mx.tx /= sw;
				mx.ty /= sh;

				// g2 matrix => gl matrix
				g2_gl_matrix_from(gshader->matrix_gl, &mx);
			}
		}
		break;
	case G2_GL_SHADER_TYPE_RADIAL:
		{
			// init
			g2_matrix_t mx;
			g2_point_t 	pb = gshader->u.radial.cp.c;
			g2_point_t 	pe = g2_point_make(pb.x, pb.y - gshader->u.radial.cp.r);
			tb_float_t 	ux = g2_float_to_tb(pe.x - pb.x);
			tb_float_t 	uy = g2_float_to_tb(pe.y - pb.y);
			tb_float_t 	un = tb_sqrtf(ux * ux + uy * uy);
			tb_float_t 	sw = gshader->width;
			tb_float_t 	sh = gshader->height;

			// apply the linear matrix for the radial fragment
			g2_matrix_init_translate(&mx, pb.x, pb.y);
			g2_matrix_scale(&mx, tb_float_to_g2(un / sw), tb_float_to_g2(un / sh));
			g2_matrix_sincos(&mx, tb_float_to_g2(uy / un), tb_float_to_g2(ux / un));
			
			// matrix: global => camera for gl
			if (g2_matrix_invert(&mx))
			{
				mx.tx /= sw;
				mx.ty /= sh;

				// g2 matrix => gl matrix
				g2_gl_matrix_from(gshader->matrix_gl, &mx);
			}
		}
		break;
	default:
		break;
	}
}

tb_size_t g2_shader_ref(tb_handle_t shader)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return_val(gshader, 0);

	// refn
	return gshader->refn;
}
tb_void_t g2_shader_inc(tb_handle_t shader)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return(gshader && gshader->refn);

	// refn++
	gshader->refn++;
}
tb_void_t g2_shader_dec(tb_handle_t shader)
{
	// shader
	g2_gl_shader_t* gshader = (g2_gl_shader_t*)shader;
	tb_assert_and_check_return(gshader && gshader->refn);

	// refn--
	if (gshader->refn > 1) gshader->refn--;
	else g2_gl_shader_exit(gshader);
}
