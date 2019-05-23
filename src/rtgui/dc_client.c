/*
 * File      : dc_client.c
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
 * 2010-08-09     Bernard      rename hardware dc to client dc
 * 2010-09-13     Bernard      fix rtgui_dc_client_blit_line issue, which found
 *                             by appele
 * 2010-09-14     Bernard      fix vline and hline coordinate issue
 */

#include "../include/rtgui.h"
#include "../include/dc.h"
#include "../include/driver.h"
#include "../include/widgets/container.h"
#include "../include/widgets/window.h"

#ifdef RT_USING_ULOG
# define LOG_LVL                    RTGUI_LOG_LEVEL
# define LOG_TAG                    "CLT_DC "
# include "components/utilities/ulog/ulog.h"
#else /* RT_USING_ULOG */
# define LOG_E(format, args...)     rt_kprintf(format "\n", ##args)
# define LOG_D                      LOG_E
#endif /* RT_USING_ULOG */


static void rtgui_dc_client_draw_point(struct rtgui_dc *dc, int x, int y);
static void rtgui_dc_client_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color);
static void rtgui_dc_client_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y);
static void rtgui_dc_client_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2);
static void rtgui_dc_client_fill_rect(struct rtgui_dc *dc, rtgui_rect_t *rect);
static void rtgui_dc_client_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data);
static void rtgui_dc_client_blit(struct rtgui_dc *dc, struct rtgui_point *dc_point, struct rtgui_dc *dest, rtgui_rect_t *rect);
static rt_bool_t rtgui_dc_client_fini(struct rtgui_dc *dc);

#define hw_driver               (rtgui_graphic_driver_get_default())
#define dc_set_foreground(c)    dc->gc.foreground = c
#define dc_set_background(c)    dc->gc.background = c
#define _int_swap(x, y)         do {x ^= y; y ^= x; x ^= y;} while (0)

const struct rtgui_dc_engine dc_client_engine =
{
    rtgui_dc_client_draw_point,
    rtgui_dc_client_draw_color_point,
    rtgui_dc_client_draw_vline,
    rtgui_dc_client_draw_hline,
    rtgui_dc_client_fill_rect,
    rtgui_dc_client_blit_line,
    rtgui_dc_client_blit,

    rtgui_dc_client_fini,
};

void rtgui_dc_client_init(rtgui_widget_t *owner)
{
    struct rtgui_dc *dc;

    RT_ASSERT(owner != RT_NULL);

    dc = RTGUI_WIDGET_DC(owner);
    dc->type = RTGUI_DC_CLIENT;
    dc->engine = &dc_client_engine;
}

struct rtgui_dc *rtgui_dc_client_create(rtgui_widget_t *owner)
{
    /* adjudge owner */
    if (owner == RT_NULL || owner->toplevel == RT_NULL) return RT_NULL;

    return RTGUI_WIDGET_DC(owner);
}

static rt_bool_t rtgui_dc_client_fini(struct rtgui_dc *dc)
{
    if (dc == RT_NULL || dc->type != RTGUI_DC_CLIENT) return RT_FALSE;

    return RT_TRUE;
}

/*
 * draw a logic point on device
 */
static void rtgui_dc_client_draw_point(struct rtgui_dc *self, int x, int y)
{
    rtgui_rect_t rect;
    rtgui_widget_t *owner;

    if (self == RT_NULL) return;
    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, struct rtgui_widget, dc_type);

    x = x + owner->extent.x1;
    y = y + owner->extent.y1;

    if (rtgui_region_contains_point(&(owner->clip), x, y, &rect) == RT_EOK)
    {
        /* draw this point */
        hw_driver->ops->set_pixel(&(owner->gc.foreground), x, y);
    }
}

static void rtgui_dc_client_draw_color_point(struct rtgui_dc *self, int x, int y, rtgui_color_t color)
{
    rtgui_rect_t rect;
    rtgui_widget_t *owner;

    if (self == RT_NULL) return;
    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, struct rtgui_widget, dc_type);

    x = x + owner->extent.x1;
    y = y + owner->extent.y1;

    if (rtgui_region_contains_point(&(owner->clip), x, y, &rect) == RT_EOK)
    {
        /* draw this point */
        hw_driver->ops->set_pixel(&color, x, y);
    }
}

/*
 * draw a logic vertical line on device
 */
static void rtgui_dc_client_draw_vline(struct rtgui_dc *self, int x, int y1, int y2)
{
    register rt_uint32_t idx;
    rtgui_widget_t *owner;

    if (self == RT_NULL) return;
    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, struct rtgui_widget, dc_type);

    x  = x + owner->extent.x1;
    y1 = y1 + owner->extent.y1;
    y2 = y2 + owner->extent.y1;
    if (y1 > y2) _int_swap(y1, y2);

    if (owner->clip.data == RT_NULL)
    {
        rtgui_rect_t *rect;

        rect = &(owner->clip.extents);

        /* calculate vline intersect */
        if (rect->x1 > x   || rect->x2 <= x) return;
        if (rect->y2 <= y1 || rect->y1 > y2) return;

        if (rect->y1 > y1) y1 = rect->y1;
        if (rect->y2 < y2) y2 = rect->y2;

        /* draw vline */
        hw_driver->ops->draw_vline(&(owner->gc.foreground), x, y1, y2);
    }
    else
    {
        for (idx = 0; idx < rtgui_region_num_rects(&(owner->clip)); idx ++)
        {
            rtgui_rect_t *rect;
            register rt_base_t draw_y1, draw_y2;

            rect = ((rtgui_rect_t *)(owner->clip.data + idx + 1));
            draw_y1 = y1;
            draw_y2 = y2;

            /* calculate vline clip */
            if (rect->x1 > x   || rect->x2 <= x) continue;
            if (rect->y2 <= y1 || rect->y1 > y2) continue;

            if (rect->y1 > y1) draw_y1 = rect->y1;
            if (rect->y2 < y2) draw_y2 = rect->y2;

            /* draw vline */
            hw_driver->ops->draw_vline(&(owner->gc.foreground), x, draw_y1, draw_y2);
        }
    }
}

/*
 * draw a logic horizontal line on device
 */
static void rtgui_dc_client_draw_hline(struct rtgui_dc *self,
    int x1, int x2, int y) {
    rtgui_widget_t *owner;
    register rt_uint32_t idx;
    rtgui_rect_t *rect;

    if (!self) return;
    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, rtgui_widget_t, dc_type);

    /* convert logic to device */
    x1 = x1 + owner->extent.x1;
    x2 = x2 + owner->extent.x1;
    if (x1 > x2) _int_swap(x1, x2);
    y  = y + owner->extent.y1;

    if (!owner->clip.data) {
        rect = &(owner->clip.extents);

        /* calculate vline intersect */
        if ((rect->y1 > y) || (rect->y2 <= y)) return;
        if ((rect->x2 <= x1) || (rect->x1 > x2)) return;

        if (rect->x1 > x1) x1 = rect->x1;
        if (rect->x2 < x2) x2 = rect->x2;

        /* draw hline */
        LOG_E("hw hline1");
        hw_driver->ops->draw_hline(&(owner->gc.foreground), x1, x2, y);
    } else {
        rt_uint32_t num = rtgui_region_num_rects(&(owner->clip));
        for (idx = 0; idx < num; idx++) {
            register rt_base_t draw_x1, draw_x2;

            rect = ((rtgui_rect_t *)(owner->clip.data + idx + 1));
            draw_x1 = x1;
            draw_x2 = x2;

            /* calculate hline clip */
            if ((rect->y1 > y) || (rect->y2 <= y)) continue;
            if ((rect->x2 <= x1) || (rect->x1 > x2)) continue;

            if (rect->x1 > x1) draw_x1 = rect->x1;
            if (rect->x2 < x2) draw_x2 = rect->x2;

            /* draw hline */
            LOG_E("hw hline2");
            hw_driver->ops->draw_hline(&(owner->gc.foreground),
                draw_x1, draw_x2, y);
        }
    }
}

static void rtgui_dc_client_fill_rect(struct rtgui_dc *self, struct rtgui_rect *rect)
{
    rtgui_color_t foreground;
    register rt_base_t idx;
    rtgui_widget_t *owner;

    RT_ASSERT(self);
    RT_ASSERT(rect);

    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, struct rtgui_widget, dc_type);

    /* save foreground color */
    foreground = owner->gc.foreground;

    /* set background color as foreground color */
    owner->gc.foreground = owner->gc.background;

    /* fill rect */
    for (idx = rect->y1; idx < rect->y2; idx ++)
    {
        rtgui_dc_client_draw_hline(self, rect->x1, rect->x2, idx);
    }

    /* restore foreground color */
    owner->gc.foreground = foreground;
}

static void rtgui_dc_client_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data)
{
    register rt_base_t idx;
    rtgui_widget_t *owner;

    if (self == RT_NULL) return;
    if (!rtgui_dc_get_visible(self)) return;

    /* get owner */
    owner = rt_container_of(self, struct rtgui_widget, dc_type);

    /* convert logic to device */
    x1 = x1 + owner->extent.x1;
    x2 = x2 + owner->extent.x1;
    if (x1 > x2) _int_swap(x1, x2);
    y  = y + owner->extent.y1;

    if (rtgui_region_is_flat(&(owner->clip)) == RT_EOK)
    {
        rtgui_rect_t *rect;
        int offset = 0;
        rect = &(owner->clip.extents);

        /* calculate vline intersect */
        if (rect->y1 > y  || rect->y2 <= y) return;
        if (rect->x2 <= x1 || rect->x1 > x2) return;

        if (rect->x1 > x1) x1 = rect->x1;
        if (rect->x2 < x2) x2 = rect->x2;

        /* patch note:
         * We need to adjust the offset when update widget clip!
         * Of course at ordinary times for 0. General */
        offset = owner->clip.extents.x1 - owner->extent.x1;
        offset = offset * _UI_BITBYTES(hw_driver->bits_per_pixel);
        /* draw hline */
        hw_driver->ops->draw_raw_hline(line_data + offset, x1, x2, y);
    }
    else
    {
        for (idx = 0; idx < rtgui_region_num_rects(&(owner->clip)); idx ++)
        {
            rtgui_rect_t *rect;
            register rt_base_t draw_x1, draw_x2;

            rect = ((rtgui_rect_t *)(owner->clip.data + idx + 1));
            draw_x1 = x1;
            draw_x2 = x2;

            /* calculate hline clip */
            if (rect->y1 > y  || rect->y2 <= y) continue;
            if (rect->x2 <= x1 || rect->x1 > x2) continue;

            if (rect->x1 > x1) draw_x1 = rect->x1;
            if (rect->x2 < x2) draw_x2 = rect->x2;

            /* draw hline */
            hw_driver->ops->draw_raw_hline(line_data + (draw_x1 - x1) * _UI_BITBYTES(hw_driver->bits_per_pixel), draw_x1, draw_x2, y);
        }
    }
}

static void rtgui_dc_client_blit(struct rtgui_dc *dc, struct rtgui_point *dc_point, struct rtgui_dc *dest, rtgui_rect_t *rect)
{
    /* not blit in hardware dc */
    return ;
}

