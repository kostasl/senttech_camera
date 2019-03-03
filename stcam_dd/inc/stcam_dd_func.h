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

#ifndef __STCAM_DD_FUNC_H__
#define __STCAM_DD_FUNC_H__

#include <linux/videodev2.h>

#include "../stcam_dd.h"
#include "stcam_dd_mem.h"
#include "stcam_dd_ext.h"

int stcam_dd_func_querycap(struct file *file, void *fh, struct v4l2_capability *arg);
int stcam_dd_func_enum_input(struct file *file, void *fh, struct v4l2_input *arg);
int stcam_dd_func_g_input(struct file *file, void *fh, unsigned int *arg);
int stcam_dd_func_s_input(struct file *file, void *fh, unsigned int arg);
int stcam_dd_func_enum_fmt(struct file *file, void *fh, struct v4l2_fmtdesc *arg);
int stcam_dd_func_g_fmt(struct file *file, void *fh, struct v4l2_format *arg);
int stcam_dd_func_s_fmt(struct file *file, void *fh, struct v4l2_format *arg);
int stcam_dd_func_try_fmt(struct file *file, void *fh, struct v4l2_format *arg);
int stcam_dd_func_g_parm(struct file *file, void *fh, struct v4l2_streamparm *arg);
int stcam_dd_func_s_parm(struct file *file, void *fh, struct v4l2_streamparm *arg);
int stcam_dd_func_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *arg);
int stcam_dd_func_querybuf(struct file *file, void *fh, struct v4l2_buffer *arg);
int stcam_dd_func_qbuf(struct file *file, void *fh, struct v4l2_buffer *arg);
int stcam_dd_func_dqbuf(struct file *file, void *fh, struct v4l2_buffer *arg);
int stcam_dd_func_streamon(struct file *file, void *fh, enum v4l2_buf_type arg);
int stcam_dd_func_streamoff(struct file *file, void *fh, enum v4l2_buf_type arg);
int stcam_dd_func_cropcap(struct file *file, void *fh, struct v4l2_cropcap *arg);
int stcam_dd_func_g_crop(struct file *file, void *fh, struct v4l2_crop *arg);
int stcam_dd_func_s_crop(struct file *file, void *fh, const struct v4l2_crop *arg);
int stcam_dd_func_queryctrl(struct file *file, void *fh, struct v4l2_queryctrl *arg);
int stcam_dd_func_querymenu(struct file *file, void *fh, struct v4l2_querymenu *arg);
int stcam_dd_func_g_ctrl(struct file *file, void *fh, struct v4l2_control *arg);
int stcam_dd_func_s_ctrl(struct file *file, void *fh, struct v4l2_control *arg);
int stcam_dd_func_log_status(struct file *file, void *fh);

int stcam_dd_func_g_cam_version(struct stcam_st_dev_data *ddata, struct stcam_st_cam_version *arg);
int stcam_dd_func_g_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *arg);
int stcam_dd_func_s_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *arg);
int stcam_dd_func_g_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *arg);
int stcam_dd_func_s_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *arg);
int stcam_dd_func_g_max_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_max_scan_size *arg);
int stcam_dd_func_g_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *arg);
int stcam_dd_func_s_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *arg);
int stcam_dd_func_g_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *arg);
int stcam_dd_func_s_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *arg);
int stcam_dd_func_g_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *arg);
int stcam_dd_func_s_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *arg);

int stcam_dd_func_g_ext_init(struct stcam_st_dev_data *ddata, struct stcam_st_ext_init *arg);

#endif//__STCAM_DD_FUNC_H__
