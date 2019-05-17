/*
 * File      : rtgui_app.h
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
 * 2012-01-13     Grissiom     first version
 * 2019-05-15     onelife      refactor
 */

#ifndef __RTGUI_APP_H__
#define __RTGUI_APP_H__

#include "include/rtthread.h"
#include "./rtgui.h"
#include "./rtgui_system.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_CLASS_TYPE(application);

/** Gets the type of a application */
#define RTGUI_APP_TYPE       (RTGUI_TYPE(application))
/** Casts the object to an rtgui_workbench */
#define RTGUI_APP(obj)       (RTGUI_OBJECT_CAST((obj), RTGUI_APP_TYPE, rtgui_app_t))
/** Checks if the object is an rtgui_workbench */
#define RTGUI_IS_APP(obj)    (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_APP_TYPE))

/**
 * create an application named @myname on current thread.
 *
 * @param name the name of the application
 *
 * @return a pointer to struct rtgui_app on success. RT_NULL on failure.
 */
rtgui_app_t *rtgui_srv_create(const char *name);
rtgui_app_t *rtgui_app_create(const char *name);
void rtgui_app_destroy(struct rtgui_app *app);
rt_bool_t rtgui_app_event_handler(rtgui_obj_t *obj,
    rtgui_evt_generic_t *event);

rt_base_t rtgui_app_run(struct rtgui_app *app);
void rtgui_app_exit(struct rtgui_app *app, rt_uint16_t code);
void rtgui_app_activate(struct rtgui_app *app);
void rtgui_app_close(struct rtgui_app *app);
void rtgui_app_sleep(struct rtgui_app *app, rt_uint32_t ms);

void rtgui_app_set_onidle(struct rtgui_app *app, rtgui_idle_hdl_t onidle);
rtgui_idle_hdl_t rtgui_app_get_onidle(struct rtgui_app *app);

/**
 * return the rtgui_app struct on current thread
 */
struct rtgui_app *rtgui_app_self(void);

rt_err_t rtgui_app_set_as_wm(struct rtgui_app *app);

void rtgui_app_set_main_win(struct rtgui_app *app, rtgui_win_t *win);
rtgui_win_t* rtgui_app_get_main_win(struct rtgui_app *app);

/* get the topwin belong app window activate count */
unsigned int rtgui_app_get_win_acti_cnt(void);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: __RTGUI_APP_H__ */
