/*
 * File      : dc.h
 * This file is part of RT-Thread GUI Engine
 * COPYRIGHT (C) 2006 - 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-16     Bernard      first version
 */
#ifndef __RTGUI_DC_H__
#define __RTGUI_DC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "include/rtgui.h"

#ifdef IMPORT_TYPES

/* Exported defines ----------------------------------------------------------*/
#ifndef M_PI
# define M_PI    (3.14159265358979323846)
#endif

/* Exported types ------------------------------------------------------------*/
typedef enum rtgui_dc_type {
    RTGUI_DC_HW,
    RTGUI_DC_CLIENT,
} rtgui_dc_type_t;

struct rtgui_dc_engine {
    /* interface */
    void (*draw_point)(rtgui_dc_t *dc, int x, int y);
    void (*draw_color_point)(rtgui_dc_t *dc, int x, int y, rtgui_color_t color);
    void (*draw_vline)(rtgui_dc_t *dc, int x, int y1, int y2);
    void (*draw_hline)(rtgui_dc_t *dc, int x1, int x2, int y);
    void (*fill_rect)(rtgui_dc_t *dc, rtgui_rect_t *rect);
    void (*blit_line)(rtgui_dc_t *dc, int x1, int x2, int y, rt_uint8_t *line_data);
    void (*blit)(rtgui_dc_t *dc, struct rtgui_point *dc_point, rtgui_dc_t *dest, rtgui_rect_t *rect);

    rt_bool_t (*fini)(rtgui_dc_t *dc);
};

/* device context/drawable canvas */
struct rtgui_dc {
    /* type of device context */
    rtgui_dc_type_t type;
    /* dc engine */
    const rtgui_dc_engine_t *engine;
};

/*
 * The hardware device context
 *
 * The hardware DC is a context based on hardware device, for examle the
 * LCD device. The operations on the hardware DC are reflected to the real
 * hardware.
 *
 */
struct rtgui_dc_hw {
    rtgui_dc_t _super;
    rtgui_widget_t *owner;
    const rtgui_gfx_driver_t *hw_driver;
};


#define RTGUI_DC(dc)            ((rtgui_dc_t*)(dc))
#define RTGUI_DC_FC(dc)         (rtgui_dc_get_gc(RTGUI_DC(dc))->foreground)
#define RTGUI_DC_BC(dc)         (rtgui_dc_get_gc(RTGUI_DC(dc))->background)
#define RTGUI_DC_FONT(dc)       (rtgui_dc_get_gc(RTGUI_DC(dc))->font)
#define RTGUI_DC_TEXTALIGN(dc)  (rtgui_dc_get_gc(RTGUI_DC(dc))->textalign)

/* Exported constants --------------------------------------------------------*/

#undef __RTGUI_DC_H__
#else /* IMPORT_TYPES */

/* Exported functions ------------------------------------------------------- */
/* destroy a dc */
void rtgui_dc_destory(rtgui_dc_t *dc);
/* create a hardware dc */
rtgui_dc_t *rtgui_dc_hw_create(rtgui_widget_t *owner);
/* create a client dc */
rtgui_dc_t *rtgui_dc_client_create(rtgui_widget_t *owner);
void rtgui_dc_client_init(rtgui_widget_t *owner);

/* begin and end a drawing */
rtgui_dc_t *rtgui_dc_begin_drawing(rtgui_widget_t *owner);
void rtgui_dc_end_drawing(rtgui_dc_t *dc, rt_bool_t update);

void rtgui_dc_draw_line(rtgui_dc_t *dc, int x1, int y1, int x2, int y2);
void rtgui_dc_draw_rect(rtgui_dc_t *dc, rtgui_rect_t *rect);
void rtgui_dc_fill_rect_forecolor(rtgui_dc_t *dc, rtgui_rect_t *rect);
void rtgui_dc_draw_round_rect(rtgui_dc_t *dc, rtgui_rect_t *rect, int r);
void rtgui_dc_fill_round_rect(rtgui_dc_t *dc, rtgui_rect_t *rect, int r);
/** Fill a vertical gradient rect from @c1 to @c2 */
void rtgui_dc_fill_gradient_rectv(rtgui_dc_t *dc, rtgui_rect_t *rect,
                                  rtgui_color_t c1, rtgui_color_t c2);
void rtgui_dc_draw_annulus(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t r1, rt_int16_t r2, rt_int16_t start, rt_int16_t end);
void rtgui_dc_draw_pie(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t r, rt_int16_t start, rt_int16_t end);
void rtgui_dc_fill_pie(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t r, rt_int16_t start, rt_int16_t end);

void rtgui_dc_draw_text(rtgui_dc_t *dc, const char *text, rtgui_rect_t *rect);
void rtgui_dc_draw_text_stroke(rtgui_dc_t *dc, const char *text, rtgui_rect_t *rect,
                               rtgui_color_t color_stroke, rtgui_color_t color_core);

void rtgui_dc_draw_mono_bmp(rtgui_dc_t *dc, int x, int y, int w, int h, const rt_uint8_t *data);
void rtgui_dc_draw_byte(rtgui_dc_t *dc, int x, int y, int h, const rt_uint8_t *data);
void rtgui_dc_draw_word(rtgui_dc_t *dc, int x, int y, int h, const rt_uint8_t *data);

void rtgui_dc_draw_border(rtgui_dc_t *dc, rtgui_rect_t *rect, int flag);
void rtgui_dc_draw_horizontal_line(rtgui_dc_t *dc, int x1, int x2, int y);
void rtgui_dc_draw_vertical_line(rtgui_dc_t *dc, int x, int y1, int y2);
void rtgui_dc_draw_focus_rect(rtgui_dc_t *dc, rtgui_rect_t *rect);

void rtgui_dc_draw_polygon(rtgui_dc_t *dc, const int *vx, const int *vy, int count);
void rtgui_dc_fill_polygon(rtgui_dc_t *dc, const int *vx, const int *vy, int count);

void rtgui_dc_draw_circle(rtgui_dc_t *dc, int x, int y, int r);
void rtgui_dc_fill_circle(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t r);
void rtgui_dc_draw_arc(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t r, rt_int16_t start, rt_int16_t end);

void rtgui_dc_draw_ellipse(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t rx, rt_int16_t ry);
void rtgui_dc_fill_ellipse(rtgui_dc_t *dc, rt_int16_t x, rt_int16_t y, rt_int16_t rx, rt_int16_t ry);

/*
 * dc inline function
 *
 * Note:
 * In order to improve drawing speed, put most of common function of dc to inline
 */

/*
 * draw a point on dc
 */
rt_inline void rtgui_dc_draw_point(rtgui_dc_t *dc, int x, int y) {
    return dc->engine->draw_point(dc, x, y);
}

/*
 * draw a color point on dc
 */
rt_inline void rtgui_dc_draw_color_point(rtgui_dc_t *dc, int x, int y, 
    rtgui_color_t color) {
    return dc->engine->draw_color_point(dc, x, y, color);
}

/*
 * draw a vertical line on dc
 */
rt_inline void rtgui_dc_draw_vline(rtgui_dc_t *dc, int x, int y1, int y2) {
    return dc->engine->draw_vline(dc, x, y1, y2);
}

/*
 * draw a horizontal line on dc
 */
rt_inline void rtgui_dc_draw_hline(rtgui_dc_t *dc, int x1, int x2, int y) {
    return dc->engine->draw_hline(dc, x1, x2, y);
}

/*
 * fill a rect with background color
 */
rt_inline void rtgui_dc_fill_rect(rtgui_dc_t *dc, rtgui_rect_t *rect) {
    return dc->engine->fill_rect(dc, rect);
}

/*
 * blit a dc (x, y) on another dc(rect)
 */
rt_inline void rtgui_dc_blit(rtgui_dc_t *dc, struct rtgui_point *point,
    rtgui_dc_t *dst, rtgui_rect_t *rect) {
    return dc->engine->blit(dc, point, dst, rect);
}

/* set gc of dc */
void rtgui_dc_set_gc(rtgui_dc_t *dc, rtgui_gc_t *gc);
/* get gc of dc */
rtgui_gc_t *rtgui_dc_get_gc(rtgui_dc_t *dc);
/* get visible status of dc */
rt_bool_t rtgui_dc_get_visible(rtgui_dc_t *dc);
/* get rect of dc */
void rtgui_dc_get_rect(rtgui_dc_t *dc, rtgui_rect_t *rect);
/* get pixel format */
rt_uint8_t rtgui_dc_get_pixel_format(rtgui_dc_t *dc);
/* coordinate conversion */
void rtgui_dc_logic_to_device(rtgui_dc_t* dc, struct rtgui_point *point);
void rtgui_dc_rect_to_device(rtgui_dc_t* dc, rtgui_rect_t* rect);

#endif /* IMPORT_TYPES */

#ifdef __cplusplus
}
#endif

#endif /* __RTGUI_DC_H__ */

