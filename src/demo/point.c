/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "demo.h"

/* ////////////////////////////////////////////////////////////////////////
 * events
 */
static tb_void_t g2_demo_move(tb_int_t x, tb_int_t y)
{
	g_dx = (x - g_x0) << 1;
	g_dy = (y - g_y0) << 1;
}
static tb_void_t g2_demo_drag(tb_int_t x, tb_int_t y)
{
	g_x0 = x;
	g_y0 = y;
}
static tb_void_t g2_demo_wheeldown(tb_int_t x, tb_int_t y)
{	
	if (g_penw > 1) g_penw--;
	else g_penw = 1;
	g2_style_width_set(g_style, g2_int_to_scalar(g_penw));
}
static tb_void_t g2_demo_wheelup(tb_int_t x, tb_int_t y)
{
	if (g_penw > 1000) g_penw = 1000;
	else g_penw++;
	g2_style_width_set(g_style, g2_int_to_scalar(g_penw));
}
static tb_void_t g2_demo_lclickdown(tb_int_t x, tb_int_t y)
{
	g_x0 = x;
	g_y0 = y;
}
static tb_void_t g2_demo_lclickup(tb_int_t x, tb_int_t y)
{
}
static tb_void_t g2_demo_rclickdown(tb_int_t x, tb_int_t y)
{
	g_capi = (g_capi + 1) % 3;
	g2_style_cap_set(g_style, g_cap[g_capi]);
}
static tb_void_t g2_demo_rclickup(tb_int_t x, tb_int_t y)
{
}
static tb_void_t g2_demo_key(tb_int_t key)
{
}

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t g2_demo_init(tb_int_t argc, tb_char_t** argv)
{
	// init style
	g2_style_width_set(g_style, g2_int_to_scalar(g_penw));
	g2_style_cap_set(g_style, g_cap[g_capi]);
	g2_style_mode_set(g_style, G2_STYLE_MODE_STROKE);
	g2_style_color_set(g_style, G2_COLOR_RED);

	// ok
	return TB_TRUE;
}
static tb_void_t g2_demo_exit()
{

}
static tb_void_t g2_demo_render()
{
	g2_point_t pt = g2_point_imake(g_x, g_y);
	g2_draw_point(g_painter, &pt);
}