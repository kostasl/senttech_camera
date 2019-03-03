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
#include <linux/init.h>
#include <linux/module.h>

#include <media/v4l2-dev.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>

#include "../inc/stcam_dd_api.h"
#include "../inc/stcam_dd_mem.h"
#include "../inc/stcam_dd_func.h"
#include "../inc/stcam_dd_set.h"
#include "../inc/stcam_dd_cam.h"
#include "../inc/stcam_dd_usb.h"
#include "../inc/stcam_dd_util.h"
#include "../inc/stcam_dd_ext.h"
#include "../inc/stcam_dd_cam_def.h"

static const struct usb_device_id stcam_dd_cam_id_table[] =
{
	{ USB_DEVICE(VENDOR_ID, STC_MB33USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MC33USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MB83USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MC83USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MB133USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MC133USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MB152USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MC152USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MB202USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MC202USB) },
	{ USB_DEVICE(VENDOR_ID, STC_MBA5MUSB3) },
	{ USB_DEVICE(VENDOR_ID, STC_MCA5MUSB3) },
	{ USB_DEVICE(VENDOR_ID, STC_MBE132U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCE132U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBCM401U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCCM401U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBCM200U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCCM200U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS241U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS241U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS510U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS510U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS322U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS322U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS891U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS891U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS123BU3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS123BU3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS43U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS43U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MBS163U3V) },
	{ USB_DEVICE(VENDOR_ID, STC_MCS163U3V) },
	{}
};

static struct usb_driver stcam_driver =
{
	.name			= "stcam_dd",
	.probe			= stcam_dd_api_probe,
	.disconnect		= stcam_dd_api_disconnect,
	.id_table		= stcam_dd_cam_id_table,
};

static struct v4l2_file_operations stcam_file_operations =
{
	.owner				= THIS_MODULE,
	.open				= stcam_dd_api_open,
	.release			= stcam_dd_api_release,
	.mmap				= stcam_dd_api_mmap,
	.poll				= stcam_dd_api_poll, 
	.unlocked_ioctl		= video_ioctl2,
};

static const struct v4l2_ioctl_ops stcam_ioctl_ops =
{
	.vidioc_querycap			= stcam_dd_func_querycap,
	.vidioc_enum_input			= stcam_dd_func_enum_input,
	.vidioc_g_input				= stcam_dd_func_g_input,
	.vidioc_s_input				= stcam_dd_func_s_input,
	.vidioc_enum_fmt_vid_cap	= stcam_dd_func_enum_fmt,
	.vidioc_g_fmt_vid_cap		= stcam_dd_func_g_fmt,
	.vidioc_s_fmt_vid_cap		= stcam_dd_func_s_fmt,
	.vidioc_try_fmt_vid_cap		= stcam_dd_func_try_fmt,
	.vidioc_g_parm				= stcam_dd_func_g_parm,
	.vidioc_s_parm				= stcam_dd_func_s_parm,
	.vidioc_reqbufs				= stcam_dd_func_reqbufs,
	.vidioc_querybuf			= stcam_dd_func_querybuf,
	.vidioc_qbuf				= stcam_dd_func_qbuf,
	.vidioc_dqbuf				= stcam_dd_func_dqbuf,
	.vidioc_streamon			= stcam_dd_func_streamon,
	.vidioc_streamoff			= stcam_dd_func_streamoff,
	.vidioc_cropcap				= stcam_dd_func_cropcap,
	.vidioc_g_crop				= stcam_dd_func_g_crop,
	.vidioc_s_crop				= stcam_dd_func_s_crop,
	.vidioc_queryctrl			= stcam_dd_func_queryctrl,
	.vidioc_querymenu			= stcam_dd_func_querymenu,
	.vidioc_g_ctrl				= stcam_dd_func_g_ctrl,
	.vidioc_s_ctrl				= stcam_dd_func_s_ctrl,
	.vidioc_log_status			= stcam_dd_func_log_status,
	.vidioc_default				= stcam_dd_api_default_func,
};

int stcam_dd_api_probe(struct usb_interface *interface, const struct usb_device_id *device_id)
{
	int ret = 0;
	struct stcam_st_dev_data *ddata = NULL;
	struct usb_device *udev = NULL;
	struct video_device *vdev = NULL;
	if((interface->intf_assoc != NULL) && (interface->altsetting != NULL) && (interface->altsetting->endpoint != NULL)) {
		if((interface->intf_assoc->bInterfaceCount > 1) && (interface->altsetting->endpoint->desc.bEndpointAddress != 0x82)) {
			return -ENODEV;
		}
	}
	stcam_dd_util_function_log();
	
	ret = stcam_dd_mem_init_data_management(&ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	udev = usb_get_dev(interface_to_usbdev(interface));
	ddata->usb_dev = udev;
	usb_set_intfdata(interface, ddata);
	
	vdev = &ddata->video_dev;
	vdev->v4l2_dev = &ddata->v4l2_dev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	vdev->dev_parent = &udev->dev;
#endif
	strlcpy(vdev->name, "stcam_dd", sizeof(vdev->name));
	vdev->release = video_device_release_empty;
	vdev->fops = &stcam_file_operations;
	vdev->ioctl_ops = &stcam_ioctl_ops;
	
	video_set_drvdata(vdev, ddata);
	ret = v4l2_device_register(&interface->dev, &ddata->v4l2_dev);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	ret = video_register_device(vdev, VFL_TYPE_GRABBER, -1);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

void stcam_dd_api_disconnect(struct usb_interface *interface)
{
	struct stcam_st_dev_data *ddata = usb_get_intfdata(interface);
	stcam_dd_util_function_log();
	
	usb_set_intfdata(interface, NULL);
	video_unregister_device(&ddata->video_dev);
	usb_put_dev(ddata->usb_dev);
	v4l2_device_disconnect(&ddata->v4l2_dev);
	v4l2_device_unregister(&ddata->v4l2_dev);
	stcam_dd_mem_exit_data_management(ddata);
}

int stcam_dd_api_open(struct file *file)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	ret = stcam_dd_mem_alloc_ctrl_data(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	mb();
	ret = stcam_dd_cam_init_camera_data(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	mb();
	ret = stcam_dd_set_init_settings(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	mb();
	ret = stcam_dd_mem_alloc_usb_data(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	mb();
	stcam_dd_usb_fill_bulk(ddata);
	
	return v4l2_fh_open(file);
}

int stcam_dd_api_release(struct file *file)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	ret = stcam_dd_usb_cancel_bulk(ddata);
	if(ret < 0) stcam_dd_util_error_log_return(ret);

	ret = stcam_dd_cam_s_sentech_mode(ddata, 0x0001);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	stcam_dd_mem_free_data(ddata);
	
	return v4l2_fh_release(file);
}

int stcam_dd_api_mmap(struct file *file, struct vm_area_struct *vma)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	stcam_dd_util_function_log();
	
	ret = stcam_dd_mem_mmap_image_buffer(ddata, vma);
	if(ret < 0) stcam_dd_util_error_log_return(ret);
	
	return 0;
}

unsigned int stcam_dd_api_poll(struct file *file, struct poll_table_struct *wait)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	poll_wait(file, &ddata->usb_ctrl->wq, wait);
	
	if(0 == list_empty(&ddata->usb_ctrl->list_buffer_full_queue)) {
		ret |= POLLIN | POLLRDNORM;
	}
	
	return ret;
}

long stcam_dd_api_default_func(struct file *file, void *fh, bool valid_prio, unsigned int cmd, void *param)
{
	int ret = 0;
	struct video_device *dev = video_devdata(file);
	struct stcam_st_dev_data *ddata = video_get_drvdata(dev);
	
	if(ddata == NULL) stcam_dd_util_error_log_return(-ENODEV);
	
	switch(cmd) {
	case STCIOC_G_CAM_VERSION:
		ret = stcam_dd_func_g_cam_version(ddata, (struct stcam_st_cam_version*)param);
		break;
	case STCIOC_G_USER_DATA:
		ret = stcam_dd_func_g_user_data(ddata, (struct stcam_st_user_data*)param);
		break;
	case STCIOC_S_USER_DATA:
		ret = stcam_dd_func_s_user_data(ddata, (struct stcam_st_user_data*)param);
		break;
	case STCIOC_G_CAM_USER_ID:
		ret = stcam_dd_func_g_cam_user_id(ddata, (struct stcam_st_cam_user_id*)param);
		break;
	case STCIOC_S_CAM_USER_ID:
		ret = stcam_dd_func_s_cam_user_id(ddata, (struct stcam_st_cam_user_id*)param);
		break;
	case STCIOC_G_MAX_SCAN_SIZE:
		ret = stcam_dd_func_g_max_scan_size(ddata, (struct stcam_st_max_scan_size*)param);
		break;
	case STCIOC_G_SCAN_SIZE:
		ret = stcam_dd_func_g_scan_size(ddata, (struct stcam_st_scan_size*)param);
		break;
	case STCIOC_S_SCAN_SIZE:
		ret = stcam_dd_func_s_scan_size(ddata, (struct stcam_st_scan_size*)param);
		break;
	case STCIOC_G_HDR:
		ret = stcam_dd_func_g_hdr(ddata, (struct stcam_st_hdr*)param);
		break;
	case STCIOC_S_HDR:
		ret = stcam_dd_func_s_hdr(ddata, (struct stcam_st_hdr*)param);
		break;
	case STCIOC_G_DEF_PIX_CORRECT_POS:
		ret = stcam_dd_func_g_def_pix_correct_pos(ddata, (struct stcam_st_def_pix_correct_pos*)param);
		break;
	case STCIOC_S_DEF_PIX_CORRECT_POS:
		ret = stcam_dd_func_s_def_pix_correct_pos(ddata, (struct stcam_st_def_pix_correct_pos*)param);
		break;
	case STCIOC_G_CAM_EXT_INIT:
		ret = stcam_dd_func_g_ext_init(ddata, (struct stcam_st_ext_init*)param);
		break;
	default:
		ret = -1;
		break;
	}
	
	return ret;
}

static int __init stcam_dd_api_init(void)
{
	usb_register(&stcam_driver);
	return 0;
}

static void __exit stcam_dd_api_exit(void)
{
	usb_deregister(&stcam_driver);
}

module_init(stcam_dd_api_init);
module_exit(stcam_dd_api_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omron Sentech CO.,LTD");
MODULE_DESCRIPTION("Sentech USB Camera Driver");

