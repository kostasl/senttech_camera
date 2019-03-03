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

#ifndef __STCAM_DD_API_H__
#define __STCAM_DD_API_H__

#include <linux/usb.h>
#include <linux/videodev2.h>

int stcam_dd_api_probe(struct usb_interface *interface, const struct usb_device_id *device_id);
void stcam_dd_api_disconnect(struct usb_interface *interface);

int stcam_dd_api_open(struct file *file);
int stcam_dd_api_release(struct file *file);
int stcam_dd_api_mmap(struct file *file, struct vm_area_struct *vma);
unsigned int stcam_dd_api_poll(struct file *file, struct poll_table_struct *wait);
long stcam_dd_api_default_func(struct file *file, void *fh, bool valid_prio, unsigned int cmd, void *param);

#endif//__STCAM_DD_API_H__
