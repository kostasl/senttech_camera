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

//#include </usr/src/linux-headers-4.15.0-45/include/linux/slab.h>
//#include </usr/src/linux-headers-4.15.0-45/include/linux/mm.h>
//#include </usr/src/linux-headers-4.15.0-45/include/linux/vmalloc.h>
//#include </usr/src/linux-headers-4.15.0-45/include/linux/timer.h>

#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include "../inc/stcam_dd_mem.h"
#include "../inc/stcam_dd_set.h"
#include "../inc/stcam_dd_usb.h"
#include "../inc/stcam_dd_util.h"

int stcam_dd_mem_init_data_management(struct stcam_st_dev_data **pddata)
{
	*pddata = kzalloc(sizeof(struct stcam_st_dev_data), GFP_KERNEL);
	if(*pddata == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	
	return 0;
}

void stcam_dd_mem_exit_data_management(struct stcam_st_dev_data *ddata)
{
	if(ddata == NULL) {
		stcam_dd_util_error_log(-EFAULT);
		return;
	}
	
	kfree(ddata);
}

int stcam_dd_mem_alloc_ctrl_data(struct stcam_st_dev_data *ddata)
{
	if(ddata == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ddata->dev_ctrl = kzalloc(sizeof(struct stcam_st_dev_ctrl), GFP_KERNEL);
	if(ddata->dev_ctrl == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	
	ddata->dev_ctrl->ctrl_info = kzalloc(sizeof(struct stcam_st_ctrl_info) * e_ctrl_id_max, GFP_KERNEL);
	if(ddata->dev_ctrl->ctrl_info == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	
	return 0;
}

int stcam_dd_mem_alloc_usb_data(struct stcam_st_dev_data *ddata)
{
	int i = 0;
	if(ddata == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	ddata->usb_ctrl= kzalloc(sizeof(struct stcam_st_usb_ctrl), GFP_KERNEL);
	if(ddata->usb_ctrl == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	
	for(i=0; i<URB_DATA_MAX; i++){
		ddata->usb_ctrl->urb_data[i].bulk_urb = usb_alloc_urb(0, GFP_KERNEL);
		if(ddata->usb_ctrl->urb_data[i].bulk_urb == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		ddata->usb_ctrl->urb_data[i].bulk_data = kzalloc(STC_USB_BULK_DATASIZE, GFP_KERNEL);
		if(ddata->usb_ctrl->urb_data[i].bulk_data == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	}
	spin_lock_init(&ddata->usb_ctrl->spinlock);
	init_waitqueue_head(&ddata->usb_ctrl->wq);
	INIT_LIST_HEAD(&ddata->usb_ctrl->list_buffer_empty_queue);
	INIT_LIST_HEAD(&ddata->usb_ctrl->list_buffer_full_queue);
	ddata->usb_ctrl->task = current;
	ddata->usb_ctrl->image_size = ddata->dev_ctrl->scan_info.image_size;

    //KL2018: init_timer was removed as of 4.15 kernel (https://lwn.net/Articles/735887/) - found suggestion on https://patchwork.kernel.org/patch/9709467/

    #if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
        init_timers(&ddata->usb_ctrl->timer_list);
        ddata->usb_ctrl->timer_list.data = (unsigned long)ddata;
    #else
        // For (LINUX_VERSION_CODE > KERNEL_VERSION(4, 15, 0))
        //Suggested Alternative Is:
        // https://lwn.net/Articles/735887/, pointed to patch https://lwn.net/Articles/735892/.
        // Overall You need a new struct that contains data and timer_list. The data is then retrieved from the callback by using the from_timer function.
        //struct stcam_st_dev_timer_data *tmdat =  kzalloc(sizeof(struct stcam_st_dev_timer_data), GFP_KERNEL);
        //tmdat->timer = ddata->usb_ctrl->timer_list;
        //tmdat->ddata = (unsigned long)ddata;
        //Pass the Container Struct Directly - Then Data is recovered using from_timer in the callback
        timer_setup(&ddata->usb_ctrl->timer_list, stcam_dd_set_time_out, 0);
    #endif

    ddata->usb_ctrl->timer_list.function = stcam_dd_set_time_out;


    ddata->usb_ctrl->is_error_detect = false;


	return 0;
}

int stcam_dd_mem_alloc_camera_param(struct stcam_st_dev_data *ddata, enum stcam_e_cam_param e_cam_param, unsigned int ui_size)
{
	if(ddata == NULL) stcam_dd_util_error_log_return(-EFAULT);
	
	switch(e_cam_param) {
	case e_cam_param_camera_info:
		if(ddata->cam_param_camera_info) {
			kfree(ddata->cam_param_camera_info);
		}
		ddata->cam_param_camera_info = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_camera_info == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_picture_size:
		if(ddata->cam_param_picture_size) {
			kfree(ddata->cam_param_picture_size);
		}
		ddata->cam_param_picture_size = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_picture_size == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_pixel_byte:
		if(ddata->cam_param_pixel_byte) {
			kfree(ddata->cam_param_pixel_byte);
		}
		ddata->cam_param_pixel_byte = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_pixel_byte == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_shutter_gain:
		if(ddata->cam_param_shutter_gain) {
			kfree(ddata->cam_param_shutter_gain);
		}
		ddata->cam_param_shutter_gain = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_shutter_gain == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_clock:
		if(ddata->cam_param_clock) {
			kfree(ddata->cam_param_clock);
		}
		ddata->cam_param_clock = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_clock == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_fpga:
		if(ddata->cam_param_fpga) {
			kfree(ddata->cam_param_fpga);
		}
		ddata->cam_param_fpga = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_fpga == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_fpga2:
		if(ddata->cam_param_fpga2) {
			kfree(ddata->cam_param_fpga2);
		}
		ddata->cam_param_fpga2 = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_fpga2 == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	case e_cam_param_fpga2_category_offset:
		if(ddata->cam_param_fpga2_category_offset) {
			kfree(ddata->cam_param_fpga2_category_offset);
		}
		ddata->cam_param_fpga2_category_offset = kzalloc(ui_size, GFP_KERNEL);
		if(ddata->cam_param_fpga2_category_offset == NULL) stcam_dd_util_error_log_return(-ENOMEM);
		break;
	default:
		return -EFAULT;
	}
	
	return 0;
}

int stcam_dd_mem_alloc_image_buffer(struct stcam_st_dev_data *ddata, unsigned int *p_buffer_num, unsigned int ui_size)
{
	int i;
	__u32 align_size = PAGE_ALIGN(ui_size);
	stcam_dd_util_function_log();
	
	if((ddata == NULL) || (p_buffer_num == NULL)) stcam_dd_util_error_log_return(-EFAULT);
	
	if(ddata->picture_image) {
		for(i=0; i<ddata->usb_ctrl->image_buf_num; i++) {
			vfree(ddata->picture_image[i].image_buffer);
		}
		kfree(ddata->picture_image);
	}
	ddata->picture_image = kzalloc((*p_buffer_num) * sizeof(struct stcam_st_picture_image), GFP_KERNEL);
	if(ddata->picture_image == NULL) stcam_dd_util_error_log_return(-ENOMEM);
	INIT_LIST_HEAD(&ddata->usb_ctrl->list_buffer_empty_queue);
	INIT_LIST_HEAD(&ddata->usb_ctrl->list_buffer_full_queue);
	
	for(i=0; i<*p_buffer_num; i++) {
		ddata->picture_image[i].image_buffer = vmalloc_user(align_size);
		if(ddata->picture_image[i].image_buffer == NULL) {
			*p_buffer_num = ddata->usb_ctrl->image_buf_num = i;
			return 0;
		}
		INIT_LIST_HEAD(&ddata->picture_image[i].list_head_empty_queue);
		INIT_LIST_HEAD(&ddata->picture_image[i].list_head_full_queue);
		ddata->picture_image[i].offset = 0;
		ddata->picture_image[i].mapcount = 0;
		ddata->picture_image[i].v4lbuf.index = i;
		ddata->picture_image[i].v4lbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ddata->picture_image[i].v4lbuf.flags = 0;
		ddata->picture_image[i].v4lbuf.field = V4L2_FIELD_NONE;
		ddata->picture_image[i].v4lbuf.memory = V4L2_MEMORY_MMAP;
		ddata->picture_image[i].v4lbuf.m.offset = i * align_size * 2;
		ddata->picture_image[i].v4lbuf.length = align_size;
		ddata->picture_image[i].v4lbuf.bytesused = align_size;
	}
	ddata->usb_ctrl->image_buf_num = *p_buffer_num;
	return 0;
}

void stcam_dd_mem_free_data(struct stcam_st_dev_data *ddata)
{
	int i = 0;
	stcam_dd_util_function_log();
	
	if(ddata == NULL) {
		stcam_dd_util_error_log(-EFAULT);
		return;
	}
	
	if(ddata->cam_param_camera_info) {
		kfree(ddata->cam_param_camera_info);
		ddata->cam_param_camera_info = NULL;
	}
	
	if(ddata->cam_param_picture_size) {
		kfree(ddata->cam_param_picture_size);
		ddata->cam_param_picture_size = NULL;
		ddata->picture_size_length = 0;
	}
	
	if(ddata->cam_param_pixel_byte) {
		kfree(ddata->cam_param_pixel_byte);
		ddata->cam_param_pixel_byte = NULL;
	}
	
	if(ddata->cam_param_shutter_gain) {
		kfree(ddata->cam_param_shutter_gain);
		ddata->cam_param_shutter_gain = NULL;
	}
	
	if(ddata->cam_param_clock) {
		kfree(ddata->cam_param_clock);
		ddata->cam_param_clock = NULL;
	}
	
	if(ddata->cam_param_fpga) {
		kfree(ddata->cam_param_fpga);
		ddata->cam_param_fpga = NULL;
	}
	
	if(ddata->cam_param_fpga2) {
		kfree(ddata->cam_param_fpga2);
		ddata->cam_param_fpga2 = NULL;
	}
	
	if(ddata->cam_param_fpga2_category_offset) {
		kfree(ddata->cam_param_fpga2_category_offset);
		ddata->cam_param_fpga2_category_offset = NULL;
	}
	
	if(ddata->picture_image) {
		for(i=0; i<ddata->usb_ctrl->image_buf_num; i++) {
			vfree(ddata->picture_image[i].image_buffer);
		}
		kfree(ddata->picture_image);
		ddata->picture_image = NULL;
	}
	
	if(ddata->usb_ctrl) {
		for(i=0;i<URB_DATA_MAX;i++) {
			if(ddata->usb_ctrl->urb_data[i].bulk_urb) {
				usb_free_urb(ddata->usb_ctrl->urb_data[i].bulk_urb);
				ddata->usb_ctrl->urb_data[i].bulk_urb = NULL;
			}
			if(ddata->usb_ctrl->urb_data[i].bulk_data) {
				kfree(ddata->usb_ctrl->urb_data[i].bulk_data);
				ddata->usb_ctrl->urb_data[i].bulk_data = NULL;
			}
		}
		kfree(ddata->usb_ctrl);
		ddata->usb_ctrl = NULL;
	}
	
	if(ddata->dev_ctrl) {
		if(ddata->dev_ctrl->ctrl_info) {
			kfree(ddata->dev_ctrl->ctrl_info);
			ddata->dev_ctrl->ctrl_info = NULL;
		}
		kfree(ddata->dev_ctrl);
		ddata->dev_ctrl = NULL;
	}
}

static void stcam_dd_mem_vm_open(struct vm_area_struct *vma)
{
	struct stcam_st_picture_image *img = vma->vm_private_data;
	img->mapcount++;
}
static void stcam_dd_mem_vm_close(struct vm_area_struct *vma)
{
	struct stcam_st_picture_image *img = vma->vm_private_data;
	img->mapcount--;
	if(img->mapcount == 0) {
		img->v4lbuf.flags &= ~V4L2_BUF_FLAG_MAPPED;
	}
}
static const struct vm_operations_struct stcam_dd_mem_vm_ops = {
	.open	= stcam_dd_mem_vm_open,
	.close	= stcam_dd_mem_vm_close
};

int stcam_dd_mem_mmap_image_buffer(struct stcam_st_dev_data *ddata, struct vm_area_struct *vma)
{
	int ret, i;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	struct stcam_st_picture_image *img = NULL;
	stcam_dd_util_function_log();
	
	for(i=0; i<ddata->usb_ctrl->image_buf_num; i++) {
		if(ddata->picture_image[i].v4lbuf.m.offset == offset) {
			img = &ddata->picture_image[i];
			break;
		}
	}
	if(img == NULL) {
		return -EINVAL;
	}
	
	ret = remap_vmalloc_range(vma, img->image_buffer, 0);
	if(ret) {
		return ret;
	}
	vma->vm_flags |= VM_DONTEXPAND;
	vma->vm_private_data = img;
	vma->vm_ops = &stcam_dd_mem_vm_ops;
	img->v4lbuf.flags |= V4L2_BUF_FLAG_MAPPED;
	stcam_dd_mem_vm_open(vma);
	return 0;
}

int stcam_dd_mem_queue_image_buffer(struct stcam_st_dev_data *ddata, struct v4l2_buffer *p_arg)
{
	unsigned long flags;

	if(p_arg->memory != V4L2_MEMORY_MMAP) {
		return -EINVAL;
	}
	
	if(p_arg->index >= ddata->usb_ctrl->image_buf_num) {
		return -EINVAL;
	}
	
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	if(ddata->picture_image[p_arg->index].v4lbuf.flags & V4L2_BUF_FLAG_QUEUED) {
		spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
		return 0;
	}
	ddata->picture_image[p_arg->index].v4lbuf.flags |= V4L2_BUF_FLAG_QUEUED;
	ddata->picture_image[p_arg->index].v4lbuf.flags &= ~V4L2_BUF_FLAG_DONE;
	list_add_tail(&ddata->picture_image[p_arg->index].list_head_empty_queue, &ddata->usb_ctrl->list_buffer_empty_queue);
	*p_arg = ddata->picture_image[p_arg->index].v4lbuf;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_mem_dequeue_image_buffer(struct stcam_st_dev_data *ddata, struct v4l2_buffer *p_arg)
{
	struct stcam_st_picture_image *image;
	unsigned long flags;
	
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	image = list_first_entry(&ddata->usb_ctrl->list_buffer_full_queue, struct stcam_st_picture_image, list_head_full_queue);
	if(image == NULL) {
		spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
		return -EINVAL;
	}
	
	list_del_init(&image->list_head_full_queue);
	image->v4lbuf.flags &= ~V4L2_BUF_FLAG_QUEUED;
	image->v4lbuf.flags |= V4L2_BUF_FLAG_DONE;
	v4l2_get_timestamp(&image->v4lbuf.timestamp);
	image->offset = 0;
	if((ddata->dev_ctrl->scan_info.width != ddata->dev_ctrl->crop_info.width) || (ddata->dev_ctrl->scan_info.height != ddata->dev_ctrl->crop_info.height)) {
		unsigned char *src, *dst;
		int i;
		__u32 scan_line_pixels = (ddata->dev_ctrl->scan_info.width * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator);
		src = image->image_buffer + (scan_line_pixels * ddata->dev_ctrl->crop_info.top) + (ddata->dev_ctrl->crop_info.left * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator);
		dst = image->image_buffer;
		for(i=0; i<ddata->dev_ctrl->crop_info.height; i++) {
			memcpy(dst, src, ddata->dev_ctrl->format.fmt.pix.bytesperline);
			src += scan_line_pixels;
			dst += ddata->dev_ctrl->format.fmt.pix.bytesperline;
		}
	}
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	*p_arg = image->v4lbuf;
	return 0;
}

