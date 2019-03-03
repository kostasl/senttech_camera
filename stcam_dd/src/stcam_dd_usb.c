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
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
#include <linux/sched/signal.h>
#else
#include <linux/sched.h>
#endif
#include "../stcam_dd.h"
#include "../inc/stcam_dd_mem.h"
#include "../inc/stcam_dd_usb.h"
#include "../inc/stcam_dd_util.h"

#define STC_ENDPOINT_CONTROL		0
#define STC_ENDPOINT_BULK			2

#define STC_USB_CONTROL_TIMEOUT		10000
#define STC_USB_HEADER_SIZE			64

int stcam_dd_usb_send_ctrl(struct usb_device *p_udev, struct usb_ctrlrequest ctrl_request, void *ddata)
{
	unsigned int pipe;
	if(ctrl_request.bRequestType == 0xC0) {
		pipe = usb_rcvctrlpipe(p_udev, STC_ENDPOINT_CONTROL);
	} else {
		pipe = usb_sndctrlpipe(p_udev, STC_ENDPOINT_CONTROL);
	}
	
	return usb_control_msg(	p_udev,
							pipe,
							ctrl_request.bRequest,
							ctrl_request.bRequestType,
							ctrl_request.wValue,
							ctrl_request.wIndex,
							ddata,
							ctrl_request.wLength,
							STC_USB_CONTROL_TIMEOUT);
}

void stcam_dd_usb_fill_bulk(struct stcam_st_dev_data *ddata)
{
	int i;
	for(i=0; i<URB_DATA_MAX; i++) {
		usb_fill_bulk_urb(	ddata->usb_ctrl->urb_data[i].bulk_urb,
							ddata->usb_dev,
							usb_rcvbulkpipe(ddata->usb_dev, STC_ENDPOINT_BULK),
							ddata->usb_ctrl->urb_data[i].bulk_data,
							STC_USB_BULK_DATASIZE,
							stcam_dd_usb_callback_bulk,
							(void*)ddata->usb_ctrl);
	}
}

int stcam_dd_usb_send_bulk(struct stcam_st_dev_data *ddata)
{
	int i,ret=0;
	for(i=0; i<URB_DATA_MAX; i++) {
		ret = usb_submit_urb(ddata->usb_ctrl->urb_data[i].bulk_urb, GFP_ATOMIC);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
		}
	}
	return 0;
}

int stcam_dd_usb_cancel_bulk(struct stcam_st_dev_data *ddata)
{
	int i;
	if(ddata->dev_ctrl->device_status == e_device_status_streaming) {
		for(i=0; i<URB_DATA_MAX; i++) {
			usb_unlink_urb(ddata->usb_ctrl->urb_data[i].bulk_urb);
		}
		ddata->dev_ctrl->device_status = e_device_status_stream_ready;
	}
	if(ddata->usb_ctrl->is_error_detect) {
		usb_clear_halt(ddata->usb_dev, usb_rcvbulkpipe(ddata->usb_dev, STC_ENDPOINT_BULK));
	}
	return 0;
}

void stcam_dd_usb_callback_bulk(struct urb *p_urb)
{
	struct stcam_st_usb_ctrl *usb_ctrl = (struct stcam_st_usb_ctrl*)p_urb->context;
	struct stcam_st_picture_image *image = NULL;
	int i, index = 0;
	struct siginfo info;
	for(i=0; i<URB_DATA_MAX; i++) {
		if(p_urb->transfer_buffer == usb_ctrl->urb_data[i].bulk_data) {
			index=i;
			break;
		}
	}
	
	spin_lock(&usb_ctrl->spinlock);
	if(p_urb->status == 0) {
		if(p_urb->actual_length == 0) {
			//no action
		} else if(p_urb->actual_length == STC_USB_HEADER_SIZE) {
			del_timer(&usb_ctrl->timer_list);
			if(usb_ctrl->timeout_exposure > 0) {
				usb_ctrl->current_timer = STC_ERROR_TIMEOUT_EE2TE;
				usb_ctrl->timer_list.expires = usb_ctrl->timeout_exposure;
				add_timer(&usb_ctrl->timer_list);
			}
			if(usb_ctrl->is_callback_exposure_end_enable) {
				send_sig_info(STC_SIGNAL_CALLBACK_EXPOSURE_END, &info, usb_ctrl->task);
			}
			usb_ctrl->offset = 0;
		} else {
			if(0 == list_empty(&usb_ctrl->list_buffer_empty_queue)) {
				image = list_first_entry(&usb_ctrl->list_buffer_empty_queue, struct stcam_st_picture_image, list_head_empty_queue);
				memcpy((image->image_buffer + usb_ctrl->offset), (p_urb->transfer_buffer), p_urb->actual_length);
				usb_ctrl->offset += p_urb->actual_length;
				if(usb_ctrl->offset >= usb_ctrl->image_size) {
					del_timer(&usb_ctrl->timer_list);
					if(usb_ctrl->timeout_hard_trigger > 0) {
						usb_ctrl->current_timer = STC_ERROR_TIMEOUT_TE2EE;
						usb_ctrl->timer_list.expires = usb_ctrl->timeout_hard_trigger;
						add_timer(&usb_ctrl->timer_list);
					}
					list_del_init(&image->list_head_empty_queue);
					list_add_tail(&image->list_head_full_queue, &usb_ctrl->list_buffer_full_queue);
					usb_ctrl->offset = 0;
					if(usb_ctrl->is_callback_image_trancefer_enable) {
						send_sig_info(STC_SIGNAL_CALLBACK_TRANSFER_END, &info, usb_ctrl->task);
					}
					wake_up_interruptible(&usb_ctrl->wq);
				}
			}
		}
		usb_submit_urb(usb_ctrl->urb_data[index].bulk_urb, GFP_ATOMIC);
	} else {
		switch(p_urb->status) {
		case -ECONNRESET:
		case -ENOENT:
		case -ESHUTDOWN:
			break;
		default:
			stcam_dd_util_error_log(p_urb->status);
			usb_ctrl->is_error_detect = true;
			if(usb_ctrl->is_callback_error_enable) {
				info.si_int = p_urb->status;
				send_sig_info(STC_SIGNAL_CALLBACK_ERROR, &info, usb_ctrl->task);
			}
			break;
		}
	}
	spin_unlock(&usb_ctrl->spinlock);
}

