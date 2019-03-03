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

#ifndef __STCAM_DD_CAM_H__
#define __STCAM_DD_CAM_H__

#include "../stcam_dd.h"
#include "stcam_dd_mem.h"

enum stcam_e_camera_category
{
	e_camera_category_type_0 = 0,	//USB2.0
	e_camera_category_type_1,		//USB3.0
	e_camera_category_type_2,		//U3V MxE132
	e_camera_category_type_3,		//U3V MxCM401
	e_camera_category_type_4,		//U3V MxS241
	e_camera_category_type_5,		//U3V MxS510
	e_camera_category_type_6,		//U3V MxS43
	e_camera_category_type_7,		//U3V MxS891/123
	e_camera_category_max
};

struct stcam_st_camera_model
{
	__u8								name[STC_IOC_PARAM_LEN_PRODUCT_NAME];
	__u16								product_id;
	__u16								camera_type;
	__u16								width;
	__u16								height;
	__u16								fps;
	enum stcam_e_camera_category		category;
};

enum stcam_e_fpga2_category
{
	e_fpga2_acquisition_control = 0,
	e_fpga2_analog_control,
	e_fpga2_digital_io_control,
	e_fpga2_vendor_unique_control,
	e_fpga2_usb_camera_org_function,
	e_fpga2_usb_camera_status,
	e_fpga2_usb_camera_feature,
	e_fpga2_hdr_control,
	e_fpga2_category_max
};

struct stcam_st_fpga2_offset
{
	enum stcam_e_fpga2_category	category;
	__u16						sub_offset;
};

// init function
int stcam_dd_cam_init_camera_data(struct stcam_st_dev_data *ddata);

// enable chack
bool stcam_dd_cam_is_enable_gain(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_hflip(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_vflip(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_temperature(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_camera_setting(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_decimation_binning(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_binningsum_mode(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_clock(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_v_blank(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_sensor_shutter_mode(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_alc(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_agc(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_ae(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_frame_burst_start_trigger(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_exposure_trigger(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_exposure_trigger_overlap(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_readout(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_readout_overlap(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_line_debounce_time(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_io0(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_io1(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_io2(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_io3(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_led_status(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_reset_switch(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_sw_status(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_white_balance(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_gamma(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_defect_pixel_correction(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_digital_clamp(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_analog_black_level(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_adjustment_mode(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_io_direction_change(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_digital_gain(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_wait_hd(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_wait_readout(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_v_binning_sum(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_90_fps(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_genicam_io(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_width(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_photocoupler(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_valid_out(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_transfer_end_out(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_exposure_end_trigger_source(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_overlap(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_overlap_off(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_overlap_read_out(struct stcam_st_dev_data *ddata);
bool stcam_dd_cam_is_enable_trigger_overlap_previous_frame(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_bitmask_scan_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_bitmask_v_decimation_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_bitmask_h_decimation_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_bitmask_sensor_shutter_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_bitmask_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata);

//get max value
__u32 stcam_dd_cam_max_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_max_exposure(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_max_v_blank(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_max_digital_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_max_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_max_analog_black_level(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_min_digital_gain(struct stcam_st_dev_data *ddata);

//get value
__u32 stcam_dd_cam_g_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_hflip(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_vflip(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_color_array(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_temperature(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_scan_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_max_roi_count(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_v_decimation_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_h_decimation_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_binningsum_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_output_fps(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_clock(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_v_blank(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_max_short_exposure(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_digital_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_sensor_shutter_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_alc_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_alc_target_level(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_agc_min_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_agc_max_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_ae_min_exposure_clock(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_ae_max_exposure_clock(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_start_trigger_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_start_trigger_source(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_start_trigger_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_burst_start_trigger_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_burst_start_trigger_source(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_burst_start_trigger_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_start_trigger_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_start_trigger_source(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_start_trigger_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_start_trigger_overlap(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_end_trigger_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_end_trigger_source(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_end_trigger_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_end_trigger_overlap(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_read_out_start_trigger_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_read_out_start_trigger_source(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_read_out_start_trigger_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_read_out_start_trigger_overlap(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_strobe_start_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_strobe_end_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_output_pulse_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_output_pulse_duration(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_readout_delay(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_line_debounce_time(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io0_direction(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io0_polarity(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io0_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io0_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io1_direction(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io1_polarity(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io1_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io1_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io2_direction(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io2_polarity(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io2_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io2_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io3_direction(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io3_polarity(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io3_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_io3_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_led_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_reset_switch(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_sw_status(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_white_balance_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_white_balance_gain_r(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_white_balance_gain_gr(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_white_balance_gain_gb(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_white_balance_gain_b(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_camera_gamma(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_defect_pixel_correction_count(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_defect_pixel_correction_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_digital_clamp(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_analog_black_level(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_adjustment_mode(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_digital_gain_type(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_gain_type(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_initial_gain(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_pid(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_image_width(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_image_height(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_total_h_clock(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_total_v_line(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_clock_hz(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_raw_image_size(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_h_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_v_binning(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_trigger_wait_hd(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_trigger_wait_readout(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_hdr_type(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_time_base(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_exposure_time_offset(struct stcam_st_dev_data *ddata);
int stcam_dd_cam_g_camera_name(struct stcam_st_dev_data *ddata, __u8 *param);
int stcam_dd_cam_g_base_size(struct stcam_st_dev_data *ddata, __u16 *paramw, __u16 *paraml);
int stcam_dd_cam_g_base_fps(struct stcam_st_dev_data *ddata, __u16 *param);
int stcam_dd_cam_g_total_scan_size(struct stcam_st_dev_data *ddata, __u16 *paramw, __u16 *paraml);
int stcam_dd_cam_g_cam_version(struct stcam_st_dev_data *ddata, struct stcam_st_cam_version *param);
int stcam_dd_cam_g_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *param);
int stcam_dd_cam_g_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *param);
int stcam_dd_cam_g_max_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_max_scan_size *param);
int stcam_dd_cam_g_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *param);
int stcam_dd_cam_g_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *param);
int stcam_dd_cam_g_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *param);

//set value
int stcam_dd_cam_s_gain(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_hflip(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_vflip(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_reset_counter(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_camera_setting(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_scan_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_v_decimation_binning(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_h_decimation_binning(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_binningsum_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_clock(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_v_blank(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_digital_gain(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_sensor_shutter_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_alc_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_alc_target_level(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_agc_min_gain(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_agc_max_gain(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_ae_min_exposure_clock(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_ae_max_exposure_clock(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_burst_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_burst_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_burst_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_end_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_end_trigger_source(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_end_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_end_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_read_out_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_read_out_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_read_out_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_read_out_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_trigger_software(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_exposure_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_strobe_start_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_strobe_end_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_output_pulse_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_output_pulse_duration(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_readout_delay(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_line_debounce_time(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io0_direction(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io0_polarity(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io0_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io0_status(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io1_direction(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io1_polarity(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io1_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io1_status(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io2_direction(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io2_polarity(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io2_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io2_status(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io3_direction(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io3_polarity(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io3_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_io3_status(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_led_status(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_reset_switch(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_white_balance_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_white_balance_gain_r(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_white_balance_gain_gr(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_white_balance_gain_gb(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_white_balance_gain_b(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_camera_gamma(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_defect_pixel_correction_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_digital_clamp(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_analog_black_level(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_adjustment_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *param);
int stcam_dd_cam_s_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *param);
int stcam_dd_cam_s_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *param);
int stcam_dd_cam_s_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *param);
int stcam_dd_cam_s_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *param);
int stcam_dd_cam_s_trigger_wait_hd(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_trigger_wait_readout(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_sentech_mode(struct stcam_st_dev_data *ddata, __u32 param);
int stcam_dd_cam_s_pixel_format(struct stcam_st_dev_data *ddata, __u32 param);

int stcam_dd_cam_update_crop_info(struct stcam_st_dev_data *ddata);
int stcam_dd_cam_update_format_info(struct stcam_st_dev_data *ddata);
int stcam_dd_cam_update_camera_info(struct stcam_st_dev_data *ddata);
__u32 stcam_dd_cam_g_pixel_format(__u16 color_array, __u16 bits_per_pixel);

#endif//__STCAM_DD_CAM_H__
