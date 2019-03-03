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

#include "../stcam_dd.h"
#include "../inc/stcam_dd_func.h"
#include "../inc/stcam_dd_set.h"
#include "../inc/stcam_dd_cam.h"
#include "../inc/stcam_dd_usb.h"
#include "../inc/stcam_dd_util.h"
#include "../inc/stcam_dd_ext.h"
#include "../inc/stcam_dd_cam_def.h"

int stcam_dd_func_querycap(struct file *file, void *fh, struct v4l2_capability *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	strncpy(arg->driver, "stcam_dd", sizeof(arg->driver));
	stcam_dd_cam_g_camera_name(ddata, arg->card);
	strncpy(arg->bus_info, ddata->usb_dev->bus->bus_name, sizeof(arg->bus_info));
	arg->version = SENTECH_VERSION;
	arg->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING | V4L2_CAP_DEVICE_CAPS;
	arg->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	
	return 0;
}

int stcam_dd_func_enum_input(struct file *file, void *fh, struct v4l2_input *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if(arg->index != 0) stcam_dd_util_error_log_return(-EINVAL);
	
	stcam_dd_cam_g_camera_name(ddata, arg->name);
	arg->type = V4L2_INPUT_TYPE_CAMERA;
	arg->status = 0;
	
	return 0;
}

int stcam_dd_func_g_input(struct file *file, void *fh, unsigned int *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	*arg = 0;
	
	return 0;
}

int stcam_dd_func_s_input(struct file *file, void *fh, unsigned int arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg != 0) stcam_dd_util_error_log_return(-EINVAL);
	
	return 0;
}

int stcam_dd_func_enum_fmt(struct file *file, void *fh, struct v4l2_fmtdesc *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	switch(ddata->dev_ctrl->bpp_list[arg->index]) {
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08);
		strlcpy((char*)arg->description, "RAW 8bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10);
		strlcpy((char*)arg->description, "RAW 10bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12);
		strlcpy((char*)arg->description, "RAW 12bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08);
		strlcpy((char*)arg->description, "MONO 8bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10);
		strlcpy((char*)arg->description, "MONO 10bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12);
		strlcpy((char*)arg->description, "MONO 12bit", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08);
		strlcpy((char*)arg->description, "BGR 24", sizeof(arg->description));
		arg->flags = 0;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10:
		arg->pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10);
		strlcpy((char*)arg->description, "BGR 32", sizeof(arg->description));
		arg->flags = 0;
		break;
	default:
		return -EINVAL;
		break;
	}
	return 0;
}

int stcam_dd_func_g_fmt(struct file *file, void *fh, struct v4l2_format *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	*arg = ddata->dev_ctrl->format;
	
	return 0;
}

int stcam_dd_func_s_fmt(struct file *file, void *fh, struct v4l2_format *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	ret = stcam_dd_func_try_fmt(file, fh, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	ddata->dev_ctrl->format = *arg;
	
	printk(KERN_DEBUG "stcam_dd_func_s_fmt %x\n", arg->fmt.pix.pixelformat);
	return stcam_dd_cam_s_pixel_format(ddata, ddata->dev_ctrl->format.fmt.pix.pixelformat);
}

int stcam_dd_func_try_fmt(struct file *file, void *fh, struct v4l2_format *arg)
{
	int i;
	__u32 pixelformat;
	struct v4l2_fmtdesc fmtdesc;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	pixelformat = arg->fmt.pix.pixelformat;
	*arg = ddata->dev_ctrl->format;
	for(i=0; i<12; i++) {
		fmtdesc.index = i;
		if(stcam_dd_func_enum_fmt(file, fh, &fmtdesc) == 0) {
			if(fmtdesc.pixelformat == pixelformat) {
				arg->fmt.pix.pixelformat = pixelformat;
				break;
			}
		}
	}
	return 0;
}

int stcam_dd_func_g_parm(struct file *file, void *fh, struct v4l2_streamparm *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	__u16 fps;
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if(arg->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) stcam_dd_util_error_log_return(-EINVAL);
	
	arg->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	arg->parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
	stcam_dd_cam_g_base_fps(ddata, &fps);
	arg->parm.capture.timeperframe.numerator = 1;
	arg->parm.capture.timeperframe.denominator = fps;
	arg->parm.capture.extendedmode = 0;
	arg->parm.capture.readbuffers = ddata->usb_ctrl->image_buf_num;
	
	return 0;
}

int stcam_dd_func_s_parm(struct file *file, void *fh, struct v4l2_streamparm *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if(arg->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) stcam_dd_util_error_log_return(-EINVAL);
	
	return 0;
}

int stcam_dd_func_reqbufs(struct file *file, void *fh, struct v4l2_requestbuffers *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if((arg->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) || (arg->memory != V4L2_MEMORY_MMAP) || (arg->count < 1)) stcam_dd_util_error_log_return(-EINVAL);
	
	ret = stcam_dd_mem_alloc_image_buffer(ddata, &arg->count, ddata->dev_ctrl->buffer_size);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	ddata->dev_ctrl->device_status = e_device_status_buffer_created;
	return 0;
}

int stcam_dd_func_querybuf(struct file *file, void *fh, struct v4l2_buffer *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if(arg->memory != V4L2_MEMORY_MMAP) stcam_dd_util_error_log_return(-EINVAL);
	if(arg->index >= ddata->usb_ctrl->image_buf_num) stcam_dd_util_error_log_return(-EINVAL);
	
	*arg = ddata->picture_image[arg->index].v4lbuf;
	return 0;
}

int stcam_dd_func_qbuf(struct file *file, void *fh, struct v4l2_buffer *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	if(ddata->dev_ctrl->device_status == e_device_status_init) stcam_dd_util_error_log_return(-EINVAL);
	
	ret = stcam_dd_mem_queue_image_buffer(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	if(ddata->dev_ctrl->device_status == e_device_status_buffer_created){
		ddata->dev_ctrl->device_status = e_device_status_stream_ready;
	}
	return 0;
}

int stcam_dd_func_dqbuf(struct file *file, void *fh, struct v4l2_buffer *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ret = stcam_dd_mem_dequeue_image_buffer(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	return 0;
}

int stcam_dd_func_streamon(struct file *file, void *fh, enum v4l2_buf_type arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(ddata->dev_ctrl->device_status < e_device_status_stream_ready) stcam_dd_util_error_log_return(-EINVAL);
	if(ddata->dev_ctrl->device_status == e_device_status_streaming) return 0;
	
	ret = stcam_dd_usb_send_bulk(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	ddata->dev_ctrl->device_status = e_device_status_streaming;
	return 0;
}

int stcam_dd_func_streamoff(struct file *file, void *fh, enum v4l2_buf_type arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	if(ddata->dev_ctrl->device_status != e_device_status_streaming) return 0;
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	
	ret = stcam_dd_usb_cancel_bulk(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	return 0;
}

int stcam_dd_func_cropcap(struct file *file, void *fh, struct v4l2_cropcap *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	arg->bounds.width = ddata->dev_ctrl->scan_info.width;
	arg->bounds.height = ddata->dev_ctrl->scan_info.height;
	arg->defrect.width = ddata->dev_ctrl->scan_info.width;
	arg->defrect.height = ddata->dev_ctrl->scan_info.height;
	arg->pixelaspect.numerator = 1;
	arg->pixelaspect.denominator = 1;
	return 0;
}

int stcam_dd_func_g_crop(struct file *file, void *fh, struct v4l2_crop *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	arg->c = ddata->dev_ctrl->crop_info;
	return 0;
}

int stcam_dd_func_s_crop(struct file *file, void *fh, const struct v4l2_crop *arg)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ddata->dev_ctrl->crop_info = arg->c;
	stcam_dd_cam_update_format_info(ddata);
	return 0;
}

int stcam_dd_func_queryctrl(struct file *file, void *fh, struct v4l2_queryctrl *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ret = stcam_dd_set_queryctrl(ddata, arg);
	return ret;
}

int stcam_dd_func_querymenu(struct file *file, void *fh, struct v4l2_querymenu *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ret = stcam_dd_set_querymenu(ddata, arg);
	return ret;
}

int stcam_dd_func_g_ctrl(struct file *file, void *fh, struct v4l2_control *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ret = stcam_dd_set_g_settings(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_ctrl(struct file *file, void *fh, struct v4l2_control *arg)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	if(arg == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ret = stcam_dd_set_s_settings(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_log_status(struct file *file, void *fh)
{
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	
	stcam_dd_util_param_log("device status", ddata->dev_ctrl->device_status);
	return 0;
}

int stcam_dd_func_g_cam_version(struct stcam_st_dev_data *ddata, struct stcam_st_cam_version *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_cam_version(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_user_data(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_s_user_data(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_cam_user_id(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_s_cam_user_id(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_max_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_max_scan_size *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_max_scan_size(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_scan_size(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_s_scan_size(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_hdr(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_s_hdr(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_g_def_pix_correct_pos(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_s_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *arg)
{
	int ret = 0;
	ret = stcam_dd_cam_s_def_pix_correct_pos(ddata, arg);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

int stcam_dd_func_g_ext_init(struct stcam_st_dev_data *ddata, struct stcam_st_ext_init *arg)
{
	arg->pid = stcam_dd_cam_g_pid(ddata);
	arg->initial_gain = stcam_dd_cam_g_initial_gain(ddata);
	arg->gain_type = stcam_dd_cam_g_gain_type(ddata);
	arg->max_gain = stcam_dd_cam_max_gain(ddata);
	arg->has_digital_gain_function = stcam_dd_cam_is_enable_digital_gain(ddata);
	arg->digital_gain_type = stcam_dd_cam_g_digital_gain_type(ddata);
	arg->max_digital_gain = stcam_dd_cam_max_digital_gain(ddata);
	arg->enable_defect_pixel_correction_count = stcam_dd_cam_g_defect_pixel_correction_count(ddata);
	arg->has_v_blank_function = stcam_dd_cam_is_enable_v_blank(ddata);
	arg->max_roi_count = stcam_dd_cam_g_max_roi_count(ddata);
	arg->max_long_exposure_clock = stcam_dd_cam_max_exposure(ddata);
	arg->exposure_time_base = stcam_dd_cam_g_exposure_time_base(ddata);
	arg->exposure_time_offset = stcam_dd_cam_g_exposure_time_offset(ddata);
	return 0;
}


