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

#ifndef __STCAM_DD_USB_H__
#define __STCAM_DD_USB_H__

#include <linux/usb.h>

#include "stcam_dd_mem.h"

#define STC_USB_BULK_DATASIZE		512*512

int stcam_dd_usb_send_ctrl(struct usb_device *p_udev, struct usb_ctrlrequest ctrl_request, void *ddata);
void stcam_dd_usb_fill_bulk(struct stcam_st_dev_data *ddata);
int stcam_dd_usb_send_bulk(struct stcam_st_dev_data *ddata);
int stcam_dd_usb_cancel_bulk(struct stcam_st_dev_data *ddata);
void stcam_dd_usb_callback_bulk(struct urb *urb);

#endif//__STCAM_DD_USB_H__
