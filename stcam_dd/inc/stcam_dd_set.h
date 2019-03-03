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

#ifndef __STCAM_DD_SET_H__
#define __STCAM_DD_SET_H__

#include <linux/videodev2.h>

#include "stcam_dd_mem.h"

struct stcam_st_ctrl_init
{
	__u32 (*get_min)(struct stcam_st_dev_data *ddata);
	__u32 (*get_max)(struct stcam_st_dev_data *ddata);
	__u32 (*get_default)(struct stcam_st_dev_data *ddata);
	bool (*get_enable)(struct stcam_st_dev_data *ddata);
	bool (*get_readonly)(struct stcam_st_dev_data *ddata);
};

int stcam_dd_set_init_settings(struct stcam_st_dev_data *ddata);
int stcam_dd_set_queryctrl(struct stcam_st_dev_data *ddata, struct v4l2_queryctrl *p_arg);
int stcam_dd_set_querymenu(struct stcam_st_dev_data *ddata, struct v4l2_querymenu *p_arg);
int stcam_dd_set_g_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg);
int stcam_dd_set_s_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg);
enum stcam_e_ctrl_id stcam_dd_set_conv_id(__u32 id);

__u32 stcam_dd_set_g_timeout_st2ee(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_timeout_te2ee(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_timeout_ee2te(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_timeout_ro2te(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_exposure_end_callback(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_transfer_end_callback(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_set_g_rcv_error_callback(struct stcam_st_dev_data *ddata);
int stcam_dd_set_s_timeout_st2ee(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_timeout_te2ee(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_timeout_ee2te(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_timeout_ro2te(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_exposure_end_callback(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_transfer_end_callback(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_rcv_error_callback(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_set_s_clear_buffer(struct stcam_st_dev_data *ddata, __u32 param);

//void stcam_dd_set_time_out(unsigned long data);
void stcam_dd_set_time_out(struct timer_list * tm); //Patching For New Kernel , Timer Struct
#endif//__STCAM_DD_SET_H__
