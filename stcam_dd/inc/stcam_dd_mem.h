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

#ifndef __STCAM_DD_MEM_H__
#define __STCAM_DD_MEM_H__

#include <linux/timer.h>
#include <linux/completion.h>
#include <linux/mm_types.h>
#include <linux/videodev2.h>
#include <media/v4l2-device.h>

enum stcam_e_device_status
{
	e_device_status_init = 0,
	e_device_status_buffer_created,
	e_device_status_stream_ready,
	e_device_status_streaming
};

enum stcam_e_cam_param
{
	e_cam_param_camera_info = 0,
	e_cam_param_picture_size,
	e_cam_param_pixel_byte,
	e_cam_param_shutter_gain,
	e_cam_param_clock,
	e_cam_param_fpga,
	e_cam_param_fpga2,
	e_cam_param_fpga2_category_offset
};

enum stcam_e_cam_product
{
	e_cam_product_mb33usb = 0,
	e_cam_product_mc33usb,
	e_cam_product_sb33usb,
	e_cam_product_sc33usb,
	e_cam_product_mb83usb,
	e_cam_product_mc83usb,
	e_cam_product_sb83usb,
	e_cam_product_sc83usb,
	e_cam_product_mb133usb,
	e_cam_product_mc133usb,
	e_cam_product_sb133usb,
	e_cam_product_sc133usb,
	e_cam_product_mb152usb,
	e_cam_product_mc152usb,
	e_cam_product_mb202usb,
	e_cam_product_mc202usb,
	e_cam_product_mba5musb3,
	e_cam_product_mca5musb3,
	e_cam_product_mbe132u3v,
	e_cam_product_mce132u3v,
	e_cam_product_mbcm401u3v,
	e_cam_product_mccm401u3v,
	e_cam_product_mbcm200u3v,
	e_cam_product_mccm200u3v,
	e_cam_product_mbs231u3v,
	e_cam_product_mcs231u3v,
	e_cam_product_mbs241u3v,
	e_cam_product_mcs241u3v,
	e_cam_product_mbs500u3v,
	e_cam_product_mcs500u3v,
	e_cam_product_mbs510u3v,
	e_cam_product_mcs510u3v,
	e_cam_product_mbs312u3v,
	e_cam_product_mcs312u3v,
	e_cam_product_mbs322u3v,
	e_cam_product_mcs322u3v,
	e_cam_product_mbs163u3v,
	e_cam_product_mcs163u3v,
	e_cam_product_mbs43u3v,
	e_cam_product_mcs43u3v,
	e_cam_product_mbs891u3v,
	e_cam_product_mcs891u3v,
	e_cam_product_mbs881u3v,
	e_cam_product_mcs881u3v,
	e_cam_product_mbs891gupu3v,
	e_cam_product_mcs891gupu3v,
	e_cam_product_mbs123bu3v,
	e_cam_product_mcs123bu3v,
	e_cam_product_mbs122bu3v,
	e_cam_product_mcs122bu3v,
	e_cam_product_mbs123bgupu3v,
	e_cam_product_mcs123bgupu3v,
	e_cam_product_max,
	e_cam_product_invalid
};

enum stcam_e_ctrl_id
{
	e_ctrl_id_gain = 0,
	e_ctrl_id_exposure,
	e_ctrl_id_hflip,
	e_ctrl_id_vflip,
	e_ctrl_id_color_array,
	e_ctrl_id_transfer_bits_per_pixel,
	e_ctrl_id_temperature,
	e_ctrl_id_reset_counter,
	e_ctrl_id_camera_setting,
	e_ctrl_id_scan_mode,
	e_ctrl_id_max_roi_count,
	e_ctrl_id_v_decimation_binning,
	e_ctrl_id_h_decimation_binning,
	e_ctrl_id_binningsum_mode,
	e_ctrl_id_output_fps,
	e_ctrl_id_clock,
	e_ctrl_id_v_blank,
	e_ctrl_id_max_short_exposure,
	e_ctrl_id_digital_gain,
	e_ctrl_id_exposure_mode,
	e_ctrl_id_sensor_shutter_mode,
	e_ctrl_id_alc_mode,
	e_ctrl_id_alc_target_level,
	e_ctrl_id_agc_min_gain,
	e_ctrl_id_agc_max_gain,
	e_ctrl_id_ae_min_exposure_clock,
	e_ctrl_id_ae_max_exposure_clock,
	e_ctrl_id_frame_start_trigger_mode,
	e_ctrl_id_frame_start_trigger_source,
	e_ctrl_id_frame_start_trigger_delay,
	e_ctrl_id_frame_start_trigger_overlap,
	e_ctrl_id_frame_burst_start_trigger_mode,
	e_ctrl_id_frame_burst_start_trigger_source,
	e_ctrl_id_frame_burst_start_trigger_delay,
	e_ctrl_id_frame_burst_start_trigger_overlap,
	e_ctrl_id_exposure_start_trigger_mode,
	e_ctrl_id_exposure_start_trigger_source,
	e_ctrl_id_exposure_start_trigger_delay,
	e_ctrl_id_exposure_start_trigger_overlap,
	e_ctrl_id_exposure_end_trigger_mode,
	e_ctrl_id_exposure_end_trigger_source,
	e_ctrl_id_exposure_end_trigger_delay,
	e_ctrl_id_exposure_end_trigger_overlap,
	e_ctrl_id_read_out_start_trigger_mode,
	e_ctrl_id_read_out_start_trigger_source,
	e_ctrl_id_read_out_start_trigger_delay,
	e_ctrl_id_read_out_start_trigger_overlap,
	e_ctrl_id_trigger_software,
	e_ctrl_id_trigger_wait_hd,
	e_ctrl_id_trigger_wait_readout,
	e_ctrl_id_exposure_delay,
	e_ctrl_id_strobe_start_delay,
	e_ctrl_id_strobe_end_delay,
	e_ctrl_id_output_pulse_delay,
	e_ctrl_id_output_pulse_duration,
	e_ctrl_id_readout_delay,
	e_ctrl_id_line_debounce_time,
	e_ctrl_id_io0_direction,
	e_ctrl_id_io0_polarity,
	e_ctrl_id_io0_mode,
	e_ctrl_id_io0_status,
	e_ctrl_id_io1_direction,
	e_ctrl_id_io1_polarity,
	e_ctrl_id_io1_mode,
	e_ctrl_id_io1_status,
	e_ctrl_id_io2_direction,
	e_ctrl_id_io2_polarity,
	e_ctrl_id_io2_mode,
	e_ctrl_id_io2_status,
	e_ctrl_id_io3_direction,
	e_ctrl_id_io3_polarity,
	e_ctrl_id_io3_mode,
	e_ctrl_id_io3_status,
	e_ctrl_id_led_status,
	e_ctrl_id_reset_switch,
	e_ctrl_id_sw_status,
	e_ctrl_id_timeout_st2ee,
	e_ctrl_id_timeout_te2ee,
	e_ctrl_id_timeout_ee2te,
	e_ctrl_id_timeout_ro2te,
	e_ctrl_id_exposure_end_callback,
	e_ctrl_id_transfer_end_callback,
	e_ctrl_id_rcv_error_callback,
	e_ctrl_id_hdr_type,
	e_ctrl_id_white_balance_mode,
	e_ctrl_id_white_balance_gain_r,
	e_ctrl_id_white_balance_gain_gr,
	e_ctrl_id_white_balance_gain_gb,
	e_ctrl_id_white_balance_gain_b,
	e_ctrl_id_camera_gamma,
	e_ctrl_id_defect_pixel_correction_count,
	e_ctrl_id_defect_pixel_correction_mode,
	e_ctrl_id_digital_clamp,
	e_ctrl_id_analog_black_level,
	e_ctrl_id_adjustment_mode,
	e_ctrl_id_clear_buffer,
	e_ctrl_id_max,
	e_ctrl_id_invalid,
	e_ctrl_id_other
};

#define URB_DATA_MAX 2

struct stcam_st_picture_image
{
	struct list_head		list_head_empty_queue;
	struct list_head		list_head_full_queue;
	__u8					*image_buffer;
	__u32					offset;
	int						mapcount;
	struct v4l2_buffer		v4lbuf;
};

struct stcam_st_scan_info
{
	__u16								width;
	__u16								height;
	struct v4l2_fract					bytes_per_pixel;
	__u32								image_size;
};

struct stcam_st_dev_ctrl
{
	enum stcam_e_device_status			device_status;
	enum stcam_e_cam_product			product;
	__u32								scan_size;
	__u32								buffer_size;
	struct stcam_st_scan_info			scan_info;
	struct v4l2_rect					crop_info;
	struct v4l2_format					format;
	__u32								bpp_list[12];
	struct stcam_st_ctrl_info			*ctrl_info;
};

struct stcam_st_urb_data
{
	struct urb							*bulk_urb;
	__u8								*bulk_data;
};

struct stcam_st_usb_ctrl
{
	spinlock_t							spinlock;
	wait_queue_head_t					wq;
	struct list_head					list_buffer_empty_queue;
	struct list_head					list_buffer_full_queue;
	__u32								image_size;
	__u32								offset;
	__u16								image_buf_num;
	bool								is_callback_exposure_end_enable;
	bool								is_callback_image_trancefer_enable;
	bool								is_callback_error_enable;
	__u16								timeout_soft_trigger;
	__u16								timeout_hard_trigger;
	__u16								timeout_exposure;
	__u16								timeout_readout;
	struct timer_list					timer_list;
	__u8								current_timer;
	struct task_struct					*task;
	struct stcam_st_urb_data			urb_data[URB_DATA_MAX];
	bool								is_error_detect;
};

struct stcam_st_dev_data
{
	struct v4l2_device					v4l2_dev;
	struct video_device					video_dev;
	struct usb_device					*usb_dev;
	struct stcam_st_dev_ctrl			*dev_ctrl;
	struct stcam_st_usb_ctrl			*usb_ctrl;
	__u8								*cam_param_camera_info;
	__u8								*cam_param_picture_size;
	__u8								*cam_param_pixel_byte;
	__u8								*cam_param_shutter_gain;
	__u8								*cam_param_clock;
	__u16								*cam_param_fpga;
	__u32								*cam_param_fpga2;
	__u32								*cam_param_fpga2_category_offset;
	struct stcam_st_picture_image		*picture_image;
	__u16								picture_size_length;
};

// KL Patch to use timer as per Kernel 4.15
//Wrap Timer In struct that contains data
struct stcam_st_dev_timer_data {
    struct timer_list                   timer;
    unsigned long                       ddata;
};

struct stcam_st_ctrl_info
{
	struct v4l2_queryctrl query_ctrl;
	__u32 (*get_ops)(struct stcam_st_dev_data *ddata);
	int (*set_ops)(struct stcam_st_dev_data *ddata, __u32 param);
};

int stcam_dd_mem_init_data_management(struct stcam_st_dev_data **ddata);
void stcam_dd_mem_exit_data_management(struct stcam_st_dev_data *ddata);
int stcam_dd_mem_alloc_ctrl_data(struct stcam_st_dev_data *ddata);
int stcam_dd_mem_alloc_usb_data(struct stcam_st_dev_data *ddata);
int stcam_dd_mem_alloc_camera_param(struct stcam_st_dev_data *ddata, enum stcam_e_cam_param e_cam_param, unsigned int ui_size);
int stcam_dd_mem_alloc_image_buffer(struct stcam_st_dev_data *ddata, unsigned int *p_buffer_num, unsigned int ui_size);
void stcam_dd_mem_free_data(struct stcam_st_dev_data *ddata);
int stcam_dd_mem_mmap_image_buffer(struct stcam_st_dev_data *ddata, struct vm_area_struct *vma);
int stcam_dd_mem_queue_image_buffer(struct stcam_st_dev_data *ddata, struct v4l2_buffer *p_arg);
int stcam_dd_mem_dequeue_image_buffer(struct stcam_st_dev_data *ddata, struct v4l2_buffer *p_arg);

#endif//__STCAM_DD_MEM_H__
