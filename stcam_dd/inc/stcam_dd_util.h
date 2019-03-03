/*
 * Sentech Camera Driver for Linux
 *
 * Copyright (C) 2017 Omron Sentech CO.,LTD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef __STCAM_DD_UTIL_H__
#define __STCAM_DD_UTIL_H__

#include <linux/printk.h>

#include "stcam_dd_mem.h"

#define MAJOR_VERSION	4
#define MINOR_VERSION	0
#define RELEASE_VERSION	3
#define SENTECH_VERSION KERNEL_VERSION(MAJOR_VERSION, MINOR_VERSION, RELEASE_VERSION)

#define stcam_dd_util_function_log() printk(KERN_DEBUG "stcam_dd: %s\n", __FUNCTION__)
#define stcam_dd_util_param_log(_STR, _PRM) printk(KERN_DEBUG "stcam_dd: %s %s[%d]\n", __FUNCTION__, _STR, _PRM)
#define stcam_dd_util_error_log(_ERR) printk(KERN_DEBUG "stcam_dd: %s %d error[%d]\n", __FUNCTION__, __LINE__, _ERR)
#define stcam_dd_util_error_log_return(_ERR) { stcam_dd_util_error_log(_ERR); return _ERR; }

#endif//__STCAM_DD_UTIL_H__
