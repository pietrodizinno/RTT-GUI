/*
 * File      : filerw.h
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
#ifndef __RTGUI_FILERW_H__
#define __RTGUI_FILERW_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "include/rtgui.h"
#ifdef RTGUI_USING_DFS_FILERW
# ifndef RT_USING_DFS
#  error "Please enable RT_USING_DFS for RTGUI_USING_DFS_FILERW"
# endif
# include "components/dfs/include/dfs_posix.h"
#endif

/* Exported defines ----------------------------------------------------------*/
#define RTGUI_FILE_SEEK_SET 0
#define RTGUI_FILE_SEEK_CUR 1
#define RTGUI_FILE_SEEK_END 2

/* Exported types ------------------------------------------------------------*/
typedef struct rtgui_filerw rtgui_filerw_t;

struct rtgui_filerw {
    int (*seek)(rtgui_filerw_t *context, rt_off_t offset, int whence);
    int (*read)(rtgui_filerw_t *context, void *buffer, rt_size_t size,
        rt_size_t count);
    int (*write)(rtgui_filerw_t *context, const void *buffer, rt_size_t size,
        rt_size_t count);
    int (*tell)(rtgui_filerw_t *context);
    int (*eof)(rtgui_filerw_t *context);
    int (*close)(rtgui_filerw_t *context);
};

/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
rtgui_filerw_t *rtgui_filerw_create_file(const char *filename, const char *mode);
rtgui_filerw_t *rtgui_filerw_create_mem(const rt_uint8_t *mem, rt_size_t size);

int rtgui_filerw_seek(rtgui_filerw_t *context, rt_off_t offset, int whence);
int rtgui_filerw_read(rtgui_filerw_t *context, void *buffer, rt_size_t size,
    rt_size_t count);
int rtgui_filerw_write(rtgui_filerw_t *context, const void *buffer,
    rt_size_t size, rt_size_t count);
int rtgui_filerw_tell(rtgui_filerw_t *context);
int rtgui_filerw_eof(rtgui_filerw_t *context);
int rtgui_filerw_close(rtgui_filerw_t *context);
int rtgui_filerw_unlink(const char *filename);

/* get memory data from filerw memory object */
const rt_uint8_t *rtgui_filerw_mem_getdata(rtgui_filerw_t *context);

#ifdef __cplusplus
}
#endif

#endif
