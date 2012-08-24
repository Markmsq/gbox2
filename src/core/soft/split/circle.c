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
 * includes
 */
#include "circle.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t g2_soft_split_circle_init(g2_soft_split_circle_t* split, g2_soft_split_circle_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(split);

	// clear
	tb_memset(split, 0, sizeof(g2_soft_split_circle_t));

	// init
	split->func = func;
	split->data = data;
}

/*!split circle
 *
 * <pre>
 *
 *                 /
 *               /
 *             /
 *           /
 *         /
 *       /
 *     /
 *   /  q
 * /----------------------|--|
 *            r            e
 *
 * q = 2 * pi / n;
 * (r + e) - (r + e) * cos(q/2) = 2 * e
 *
 * q <~ 1 => cos(q/2) ~= 1 - q * q / 8
 * n = 0.5 * pi * sqrt(r / e + 1) ~= 0.5 * pi * sqrt(r / e)
 *
 * if e < 0.4, n >= 2.5 * pi * sqrt(r)
 * ~=>: 
 * if (r <= 90) n ~= |r / 4| + 6
 * if (r > 90) n ~= |r / 16| + 23
 *
 * r = 1:
 * xx = x * cos(q) - y * sin(q)
 * yy = x * sin(q) + y * cos(q)
 *
 * if (q < 1)
 * cos(q) ~= 1 - q * q / 2
 * sin(q) ~= q - q * q * q / 6
 *
 * </pre>
 */
tb_void_t g2_soft_split_circle_done(g2_soft_split_circle_t* split, g2_circle_t const* circle)
{
	// check
	tb_assert(split->func);

	// init
	tb_size_t 	ri = (tb_size_t)g2_float_to_long(circle->r);
	tb_int64_t 	rf = (tb_int64_t)g2_float_to_fixed(circle->r);
	tb_int64_t 	pi = (tb_int64_t)TB_FIXED_PI;
	tb_size_t 	n = ri <= 90? (ri >> 2) + 6 : (ri >> 4) + 23;
	tb_int64_t 	a = TB_FIXED_ONE - (((pi * pi) / (n * n)) >> 15);
	tb_int64_t 	b = (pi << 1) / n - (((pi * pi * pi) / (3 * n * n * n)) >> 30);

	tb_int64_t 	x0 = (tb_int64_t)g2_float_to_fixed(circle->c.x);
	tb_int64_t 	y0 = (tb_int64_t)g2_float_to_fixed(circle->c.y);
	tb_int64_t 	x1 = TB_FIXED_ONE;
	tb_int64_t 	y1 = 0;
	tb_int64_t 	x2 = 0;
	tb_int64_t 	y2 = 0;

	// head
	g2_point_t pb, pt;
	pb.x = g2_fixed_to_float(x0 + rf);
	pb.y = g2_fixed_to_float(y0);

	// done
	split->func(split, &pb);

	// walk
	while (n--)
	{
		x2 = (a * x1 - b * y1) >> 16;
		y2 = (b * x1 + a * y1) >> 16;

		pt.x = g2_fixed_to_float(x0 + ((x2 * rf) >> 16));
		pt.y = g2_fixed_to_float(y0 - ((y2 * rf) >> 16));

		// done
		split->func(split, &pt);

		// next
		x1 = x2;
		y1 = y2;
	}
	
	// close 
	split->func(split, &pb);
}

