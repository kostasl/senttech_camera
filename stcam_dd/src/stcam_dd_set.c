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
#include "../inc/stcam_dd_set.h"
#include "../inc/stcam_dd_cam.h"
#include "../inc/stcam_dd_cam_def.h"
#include "../inc/stcam_dd_util.h"
#include "../inc/stcam_dd_ext.h"

const struct stcam_st_ctrl_info stcam_dd_set_control_table[e_ctrl_id_max] =
{
	{ { V4L2_CID_GAIN,									V4L2_CTRL_TYPE_INTEGER,		"Gain",									0,		0,		1,		0,		0							},		stcam_dd_cam_g_gain,								stcam_dd_cam_s_gain },
	{ { V4L2_CID_EXPOSURE,								V4L2_CTRL_TYPE_INTEGER,		"Exposure",								0,		0,		1,		0,		0							},		stcam_dd_cam_g_exposure,							stcam_dd_cam_s_exposure },
	{ { V4L2_CID_HFLIP,									V4L2_CTRL_TYPE_BOOLEAN,		"H Flip",								0,		1,		1,		0,		0							},		stcam_dd_cam_g_hflip,								stcam_dd_cam_s_hflip },
	{ { V4L2_CID_VFLIP,									V4L2_CTRL_TYPE_BOOLEAN,		"V Flip",								0,		1,		1,		0,		0							},		stcam_dd_cam_g_vflip,								stcam_dd_cam_s_vflip },
	{ { STC_CID_COLOR_ARRAY,							V4L2_CTRL_TYPE_INTEGER,		"Color Array",							0x0001,	0x0005,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_color_array,							NULL },
	{ { STC_CID_TRANSFER_BITS_PER_PIXEL,				V4L2_CTRL_TYPE_MENU,		"Transfer Bits Per Pixel",				0,		11,		1,		0,		0							},		stcam_dd_cam_g_transfer_bits_per_pixel,				stcam_dd_cam_s_transfer_bits_per_pixel },
	{ { STC_CID_TEMPERATURE,							V4L2_CTRL_TYPE_INTEGER,		"Temperature",							0,		255,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_temperature,							NULL },
	{ { STC_CID_RESET_COUNTER,							V4L2_CTRL_TYPE_BUTTON,		"Reset Counter",						0,		0,		1,		0,		V4L2_CTRL_FLAG_WRITE_ONLY	},		NULL,												stcam_dd_cam_s_reset_counter },
	{ { STC_CID_CAMERA_SETTING,							V4L2_CTRL_TYPE_MENU,		"Camera Setting",						0,		4,		1,		0,		V4L2_CTRL_FLAG_WRITE_ONLY	},		NULL,												stcam_dd_cam_s_camera_setting },
	{ { STC_CID_SCAN_MODE,								V4L2_CTRL_TYPE_MENU,		"Scan Mode",							0,		10,		1,		0,		0							},		stcam_dd_cam_g_scan_mode,							stcam_dd_cam_s_scan_mode },
	{ { STC_CID_MAX_ROI_COUNT,							V4L2_CTRL_TYPE_INTEGER,		"Max ROI Count",						0,		255,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_max_roi_count,						NULL },
	{ { STC_CID_V_DECIMATION_BINNING,					V4L2_CTRL_TYPE_MENU,		"V Skipping Binning",					0,		19,		1,		0,		0							},		stcam_dd_cam_g_v_decimation_binning,				stcam_dd_cam_s_v_decimation_binning },
	{ { STC_CID_H_DECIMATION_BINNING,					V4L2_CTRL_TYPE_MENU,		"H Skipping Binning",					0,		19,		1,		0,		0							},		stcam_dd_cam_g_h_decimation_binning,				stcam_dd_cam_s_h_decimation_binning },
	{ { STC_CID_BINNINGSUM_MODE,						V4L2_CTRL_TYPE_MENU,		"Binning Sum Mode",						0,		2,		1,		0,		0							},		stcam_dd_cam_g_binningsum_mode,						stcam_dd_cam_s_binningsum_mode },
	{ { STC_CID_OUTPUT_FPS,								V4L2_CTRL_TYPE_INTEGER,		"Output FPS",							0,		65535,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_output_fps,							NULL },
	{ { STC_CID_CLOCK,									V4L2_CTRL_TYPE_MENU,		"Clock",								0,		3,		1,		0,		0							},		stcam_dd_cam_g_clock,								stcam_dd_cam_s_clock },
	{ { STC_CID_V_BLANK,								V4L2_CTRL_TYPE_INTEGER,		"V Blank",								0,		0,		1,		0,		0							},		stcam_dd_cam_g_v_blank,								stcam_dd_cam_s_v_blank },
	{ { STC_CID_MAX_SHORT_EXPOSURE,						V4L2_CTRL_TYPE_INTEGER,		"Max Short Exposure",					0,		0,		1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_max_short_exposure,					NULL },
	{ { STC_CID_DIGITAL_GAIN,							V4L2_CTRL_TYPE_INTEGER,		"Digital Gain",							0,		0,		1,		0,		0							},		stcam_dd_cam_g_digital_gain,						stcam_dd_cam_s_digital_gain },
	{ { STC_CID_EXPOSURE_MODE,							V4L2_CTRL_TYPE_MENU,		"Exposure Mode",						0,		3,		1,		0,		0							},		stcam_dd_cam_g_exposure_mode,						stcam_dd_cam_s_exposure_mode },
	{ { STC_CID_SENSOR_SHUTTER_MODE,					V4L2_CTRL_TYPE_MENU,		"Sensor Shutter Mode",					0,		2,		1,		0,		0							},		stcam_dd_cam_g_sensor_shutter_mode,					stcam_dd_cam_s_sensor_shutter_mode },
	{ { STC_CID_ALC_MODE,								V4L2_CTRL_TYPE_MENU,		"ALC Mode",								0,		3,		1,		0,		0							},		stcam_dd_cam_g_alc_mode,							stcam_dd_cam_s_alc_mode },
	{ { STC_CID_ALC_TARGET_LEVEL,						V4L2_CTRL_TYPE_INTEGER,		"ALC Target Level",						0,		255,	1,		0,		0							},		stcam_dd_cam_g_alc_target_level,					stcam_dd_cam_s_alc_target_level },
	{ { STC_CID_AGC_MIN_GAIN,							V4L2_CTRL_TYPE_INTEGER,		"AGC Min Gain",							0,		0,		1,		0,		0							},		stcam_dd_cam_g_agc_min_gain,						stcam_dd_cam_s_agc_min_gain },
	{ { STC_CID_AGC_MAX_GAIN,							V4L2_CTRL_TYPE_INTEGER,		"AGC Max Gain",							0,		0,		1,		0,		0							},		stcam_dd_cam_g_agc_max_gain,						stcam_dd_cam_s_agc_max_gain },
	{ { STC_CID_AE_MIN_EXPOSURE_CLOCK,					V4L2_CTRL_TYPE_INTEGER,		"AE Min Exposure Clock",				0,		0,		1,		0,		0							},		stcam_dd_cam_g_ae_min_exposure_clock,				stcam_dd_cam_s_ae_min_exposure_clock },
	{ { STC_CID_AE_MAX_EXPOSURE_CLOCK,					V4L2_CTRL_TYPE_INTEGER,		"AE Max Exposure Clock",				0,		0,		1,		0,		0							},		stcam_dd_cam_g_ae_max_exposure_clock,				stcam_dd_cam_s_ae_max_exposure_clock },
	{ { STC_CID_FRAME_START_TRIGGER_MODE,				V4L2_CTRL_TYPE_BOOLEAN,		"Frame Start Trigger Mode",				0,		1,		1,		0,		0							},		stcam_dd_cam_g_frame_start_trigger_mode,			stcam_dd_cam_s_frame_start_trigger_mode },
	{ { STC_CID_FRAME_START_TRIGGER_SOURCE,				V4L2_CTRL_TYPE_MENU,		"Frame Start Trigger Source",			0,		5,		1,		0,		0							},		stcam_dd_cam_g_frame_start_trigger_source,			stcam_dd_cam_s_frame_start_trigger_source },
	{ { STC_CID_FRAME_START_TRIGGER_DELAY,				V4L2_CTRL_TYPE_INTEGER,		"Frame Start Trigger Delay",			0,		65535,	1,		0,		0							},		stcam_dd_cam_g_frame_start_trigger_delay,			stcam_dd_cam_s_frame_start_trigger_delay },
	{ { STC_CID_FRAME_START_TRIGGER_OVERLAP,			V4L2_CTRL_TYPE_MENU,		"Frame Start Trigger Overlap",			0,		2,		1,		0,		0							},		stcam_dd_cam_g_frame_start_trigger_overlap,			stcam_dd_cam_s_frame_start_trigger_overlap },
	{ { STC_CID_FRAME_BURST_START_TRIGGER_MODE,			V4L2_CTRL_TYPE_BOOLEAN,		"Frame Burst Trigger Mode",				0,		1,		1,		0,		0							},		stcam_dd_cam_g_frame_burst_start_trigger_mode,		stcam_dd_cam_s_frame_burst_start_trigger_mode },
	{ { STC_CID_FRAME_BURST_START_TRIGGER_SOURCE,		V4L2_CTRL_TYPE_MENU,		"Frame Burst Trigger Source",			0,		5,		1,		0,		0							},		stcam_dd_cam_g_frame_burst_start_trigger_source,	stcam_dd_cam_s_frame_burst_start_trigger_source },
	{ { STC_CID_FRAME_BURST_START_TRIGGER_DELAY,		V4L2_CTRL_TYPE_INTEGER,		"Frame Burst Trigger Delay",			0,		65535,	1,		0,		0							},		stcam_dd_cam_g_frame_burst_start_trigger_delay,		stcam_dd_cam_s_frame_burst_start_trigger_delay },
	{ { STC_CID_FRAME_BURST_START_TRIGGER_OVERLAP,		V4L2_CTRL_TYPE_MENU,		"Frame Burst Trigger Overlap",			0,		2,		1,		0,		0							},		stcam_dd_cam_g_frame_burst_start_trigger_overlap,	stcam_dd_cam_s_frame_burst_start_trigger_overlap },
	{ { STC_CID_EXPOSURE_START_TRIGGER_MODE,			V4L2_CTRL_TYPE_BOOLEAN,		"Exposure Start Trigger Mode",			0,		1,		1,		0,		0							},		stcam_dd_cam_g_exposure_start_trigger_mode,			stcam_dd_cam_s_exposure_start_trigger_mode },
	{ { STC_CID_EXPOSURE_START_TRIGGER_SOURCE,			V4L2_CTRL_TYPE_MENU,		"Exposure Start Trigger Source",		0,		5,		1,		0,		0							},		stcam_dd_cam_g_exposure_start_trigger_source,		stcam_dd_cam_s_exposure_start_trigger_source },
	{ { STC_CID_EXPOSURE_START_TRIGGER_DELAY,			V4L2_CTRL_TYPE_INTEGER,		"Exposure Start Trigger Delay",			0,		65535,	1,		0,		0							},		stcam_dd_cam_g_exposure_start_trigger_delay,		stcam_dd_cam_s_exposure_start_trigger_delay },
	{ { STC_CID_EXPOSURE_START_TRIGGER_OVERLAP,			V4L2_CTRL_TYPE_MENU,		"Exposure Start Trigger Overlap",		0,		2,		1,		0,		0							},		stcam_dd_cam_g_exposure_start_trigger_overlap,		stcam_dd_cam_s_exposure_start_trigger_overlap },
	{ { STC_CID_EXPOSURE_END_TRIGGER_MODE,				V4L2_CTRL_TYPE_BOOLEAN,		"Exposure End Trigger Mode",			0,		1,		1,		0,		0							},		stcam_dd_cam_g_exposure_end_trigger_mode,			stcam_dd_cam_s_exposure_end_trigger_mode },
	{ { STC_CID_EXPOSURE_END_TRIGGER_SOURCE,			V4L2_CTRL_TYPE_MENU,		"Exposure End Trigger Source",			0,		5,		1,		0,		0							},		stcam_dd_cam_g_exposure_end_trigger_source,			stcam_dd_cam_s_exposure_end_trigger_source },
	{ { STC_CID_EXPOSURE_END_TRIGGER_DELAY,				V4L2_CTRL_TYPE_INTEGER,		"Exposure End Trigger Delay",			0,		65535,	1,		0,		0							},		stcam_dd_cam_g_exposure_end_trigger_delay,			stcam_dd_cam_s_exposure_end_trigger_delay },
	{ { STC_CID_EXPOSURE_END_TRIGGER_OVERLAP,			V4L2_CTRL_TYPE_MENU,		"Exposure End Trigger Overlap",			0,		2,		1,		0,		0							},		stcam_dd_cam_g_exposure_end_trigger_overlap,		stcam_dd_cam_s_exposure_end_trigger_overlap },
	{ { STC_CID_READ_OUT_START_TRIGGER_MODE,			V4L2_CTRL_TYPE_BOOLEAN,		"Read Out Start Trigger Mode",			0,		1,		1,		0,		0							},		stcam_dd_cam_g_read_out_start_trigger_mode,			stcam_dd_cam_s_read_out_start_trigger_mode },
	{ { STC_CID_READ_OUT_START_TRIGGER_SOURCE,			V4L2_CTRL_TYPE_MENU,		"Read Out Start Trigger Source",		0,		5,		1,		0,		0							},		stcam_dd_cam_g_read_out_start_trigger_source,		stcam_dd_cam_s_read_out_start_trigger_source },
	{ { STC_CID_READ_OUT_START_TRIGGER_DELAY,			V4L2_CTRL_TYPE_INTEGER,		"Read Out Start Trigger Delay",			0,		65535,	1,		0,		0							},		stcam_dd_cam_g_read_out_start_trigger_delay,		stcam_dd_cam_s_read_out_start_trigger_delay },
	{ { STC_CID_READ_OUT_START_TRIGGER_OVERLAP,			V4L2_CTRL_TYPE_MENU,		"Read Out Start Trigger Overlap",		0,		2,		1,		0,		0							},		stcam_dd_cam_g_read_out_start_trigger_overlap,		stcam_dd_cam_s_read_out_start_trigger_overlap },
	{ { STC_CID_TRIGGER_SOFTWARE,						V4L2_CTRL_TYPE_MENU,		"Trigger Software",						0,		4,		1,		0,		V4L2_CTRL_FLAG_WRITE_ONLY	},		NULL,												stcam_dd_cam_s_trigger_software },
	{ { STC_CID_TRIGGER_WAIT_HD,						V4L2_CTRL_TYPE_BOOLEAN,		"Trigger Wait Hd",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_trigger_wait_hd,						stcam_dd_cam_s_trigger_wait_hd },
	{ { STC_CID_TRIGGER_WAIT_READOUT,					V4L2_CTRL_TYPE_BOOLEAN,		"Trigger Wait Readout",					0,		1,		1,		0,		0							},		stcam_dd_cam_g_trigger_wait_readout,				stcam_dd_cam_s_trigger_wait_readout },
	{ { STC_CID_EXPOSURE_DELAY,							V4L2_CTRL_TYPE_INTEGER,		"Exposure Delay",						0,		0,		1,		0,		0							},		stcam_dd_cam_g_exposure_delay,						stcam_dd_cam_s_exposure_delay },
	{ { STC_CID_STROBE_START_DELAY,						V4L2_CTRL_TYPE_INTEGER,		"Strobe Start Delay",					0,		0,		1,		0,		0							},		stcam_dd_cam_g_strobe_start_delay,					stcam_dd_cam_s_strobe_start_delay },
	{ { STC_CID_STROBE_END_DELAY,						V4L2_CTRL_TYPE_INTEGER,		"Strobe End Delay",						0,		0,		1,		0,		0							},		stcam_dd_cam_g_strobe_end_delay,					stcam_dd_cam_s_strobe_end_delay },
	{ { STC_CID_OUTPUT_PULSE_DELAY,						V4L2_CTRL_TYPE_INTEGER,		"Output Pulse Delay",					0,		0,		1,		0,		0							},		stcam_dd_cam_g_output_pulse_delay,					stcam_dd_cam_s_output_pulse_delay },
	{ { STC_CID_OUTPUT_PULSE_DURATION,					V4L2_CTRL_TYPE_INTEGER,		"Output Pulse Duration",				0,		0,		1,		0,		0							},		stcam_dd_cam_g_output_pulse_duration,				stcam_dd_cam_s_output_pulse_duration },
	{ { STC_CID_READOUT_DELAY,							V4L2_CTRL_TYPE_INTEGER,		"Readout Delay",						0,		65535,	1,		0,		0							},		stcam_dd_cam_g_readout_delay,						stcam_dd_cam_s_readout_delay },
	{ { STC_CID_LINE_DEBOUNCE_TIME,						V4L2_CTRL_TYPE_INTEGER,		"Line Debounce Time",					0,		10000,	1,		0,		0							},		stcam_dd_cam_g_line_debounce_time,					stcam_dd_cam_s_line_debounce_time },
	{ { STC_CID_IO0_DIRECTION,							V4L2_CTRL_TYPE_MENU,		"IO0 Direction",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_io0_direction,						stcam_dd_cam_s_io0_direction },
	{ { STC_CID_IO0_POLARITY,							V4L2_CTRL_TYPE_MENU,		"IO0 Polarity",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io0_polarity,						stcam_dd_cam_s_io0_polarity },
	{ { STC_CID_IO0_MODE,								V4L2_CTRL_TYPE_MENU,		"IO0 Mode",								0,		14,		1,		0,		0							},		stcam_dd_cam_g_io0_mode,							stcam_dd_cam_s_io0_mode },
	{ { STC_CID_IO0_STATUS,								V4L2_CTRL_TYPE_MENU,		"IO0 Status",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io0_status,							stcam_dd_cam_s_io0_status },
	{ { STC_CID_IO1_DIRECTION,							V4L2_CTRL_TYPE_MENU,		"IO1 Direction",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_io1_direction,						stcam_dd_cam_s_io1_direction },
	{ { STC_CID_IO1_POLARITY,							V4L2_CTRL_TYPE_MENU,		"IO1 Polarity",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io1_polarity,						stcam_dd_cam_s_io1_polarity },
	{ { STC_CID_IO1_MODE,								V4L2_CTRL_TYPE_MENU,		"IO1 Mode",								0,		14,		1,		0,		0							},		stcam_dd_cam_g_io1_mode,							stcam_dd_cam_s_io1_mode },
	{ { STC_CID_IO1_STATUS,								V4L2_CTRL_TYPE_MENU,		"IO1 Status",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io1_status,							stcam_dd_cam_s_io1_status },
	{ { STC_CID_IO2_DIRECTION,							V4L2_CTRL_TYPE_MENU,		"IO2 Direction",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_io2_direction,						stcam_dd_cam_s_io2_direction },
	{ { STC_CID_IO2_POLARITY,							V4L2_CTRL_TYPE_MENU,		"IO2 Polarity",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io2_polarity,						stcam_dd_cam_s_io2_polarity },
	{ { STC_CID_IO2_MODE,								V4L2_CTRL_TYPE_MENU,		"IO2 Mode",								0,		14,		1,		0,		0							},		stcam_dd_cam_g_io2_mode,							stcam_dd_cam_s_io2_mode },
	{ { STC_CID_IO2_STATUS,								V4L2_CTRL_TYPE_MENU,		"IO2 Status",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io2_status,							stcam_dd_cam_s_io2_status },
	{ { STC_CID_IO3_DIRECTION,							V4L2_CTRL_TYPE_MENU,		"IO3 Direction",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_io3_direction,						stcam_dd_cam_s_io3_direction },
	{ { STC_CID_IO3_POLARITY,							V4L2_CTRL_TYPE_MENU,		"IO3 Polarity",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io3_polarity,						stcam_dd_cam_s_io3_polarity },
	{ { STC_CID_IO3_MODE,								V4L2_CTRL_TYPE_MENU,		"IO3 Mode",								0,		14,		1,		0,		0							},		stcam_dd_cam_g_io3_mode,							stcam_dd_cam_s_io3_mode },
	{ { STC_CID_IO3_STATUS,								V4L2_CTRL_TYPE_MENU,		"IO3 Status",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_io3_status,							stcam_dd_cam_s_io3_status },
	{ { STC_CID_LED_STATUS,								V4L2_CTRL_TYPE_MENU,		"LED Status",							0,		3,		1,		0,		0							},		stcam_dd_cam_g_led_status,							stcam_dd_cam_s_led_status },
	{ { STC_CID_RESET_SWITCH,							V4L2_CTRL_TYPE_BOOLEAN,		"Reset Switch",							0,		1,		1,		0,		0							},		stcam_dd_cam_g_reset_switch,						stcam_dd_cam_s_reset_switch },
	{ { STC_CID_SW_STATUS,								V4L2_CTRL_TYPE_INTEGER,		"SW Status",							0x0000,	0x000f,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_sw_status,							NULL },
	{ { STC_CID_TIMEOUT_ST2EE,							V4L2_CTRL_TYPE_INTEGER,		"Timeout ST2EE",						0,		65535,	1,		0,		0							},		stcam_dd_set_g_timeout_st2ee,						stcam_dd_set_s_timeout_st2ee },
	{ { STC_CID_TIMEOUT_TE2EE,							V4L2_CTRL_TYPE_INTEGER,		"Timeout TE2EE",						0,		65535,	1,		0,		0							},		stcam_dd_set_g_timeout_te2ee,						stcam_dd_set_s_timeout_te2ee },
	{ { STC_CID_TIMEOUT_EE2TE,							V4L2_CTRL_TYPE_INTEGER,		"Timeout EE2TE",						0,		65535,	1,		0,		0							},		stcam_dd_set_g_timeout_ee2te,						stcam_dd_set_s_timeout_ee2te },
	{ { STC_CID_TIMEOUT_RO2TE,							V4L2_CTRL_TYPE_INTEGER,		"Timeout RO2TE",						0,		65535,	1,		0,		0							},		stcam_dd_set_g_timeout_ro2te,						stcam_dd_set_s_timeout_ro2te },
	{ { STC_CID_EXPOSURE_END_CALLBACK,					V4L2_CTRL_TYPE_BOOLEAN,		"Exposure End Callback",				0,		1,		1,		0,		0							},		stcam_dd_set_g_exposure_end_callback,				stcam_dd_set_s_exposure_end_callback },
	{ { STC_CID_TRANSFER_END_CALLBACK,					V4L2_CTRL_TYPE_BOOLEAN,		"Transfer End Callback",				0,		1,		1,		0,		0							},		stcam_dd_set_g_transfer_end_callback,				stcam_dd_set_s_transfer_end_callback },
	{ { STC_CID_RCV_ERROR_CALLBACK,						V4L2_CTRL_TYPE_BOOLEAN,		"Rcv Error Callback",					0,		1,		1,		0,		0							},		stcam_dd_set_g_rcv_error_callback,					stcam_dd_set_s_rcv_error_callback },
	{ { STC_CID_HDR_TYPE,								V4L2_CTRL_TYPE_INTEGER,		"HDR Type",								0,		1,		1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_hdr_type,							NULL },
	{ { STC_CID_WHITE_BALANCE_MODE,						V4L2_CTRL_TYPE_MENU,		"White Balance Mode",					0,		3,		1,		0,		0							},		stcam_dd_cam_g_white_balance_mode,					stcam_dd_cam_s_white_balance_mode },
	{ { STC_CID_WHITE_BALANCE_GAIN_R,					V4L2_CTRL_TYPE_INTEGER,		"White Balance Gain R",					0,		511,	1,		0,		0							},		stcam_dd_cam_g_white_balance_gain_r,				stcam_dd_cam_s_white_balance_gain_r },
	{ { STC_CID_WHITE_BALANCE_GAIN_GR,					V4L2_CTRL_TYPE_INTEGER,		"White Balance Gain Gr",				0,		511,	1,		0,		0							},		stcam_dd_cam_g_white_balance_gain_gr,				stcam_dd_cam_s_white_balance_gain_gr },
	{ { STC_CID_WHITE_BALANCE_GAIN_GB,					V4L2_CTRL_TYPE_INTEGER,		"White Balance Gain Gb",				0,		511,	1,		0,		0							},		stcam_dd_cam_g_white_balance_gain_gb,				stcam_dd_cam_s_white_balance_gain_gb },
	{ { STC_CID_WHITE_BALANCE_GAIN_B,					V4L2_CTRL_TYPE_INTEGER,		"White Balance Gain B",					0,		511,	1,		0,		0							},		stcam_dd_cam_g_white_balance_gain_b,				stcam_dd_cam_s_white_balance_gain_b },
	{ { STC_CID_CAMERA_GAMMA,							V4L2_CTRL_TYPE_INTEGER,		"Camera Gamma",							0,		40,		1,		0,		0							},		stcam_dd_cam_g_camera_gamma,						stcam_dd_cam_s_camera_gamma },
	{ { STC_CID_DEFECT_PIXEL_CORRECTION_COUNT,			V4L2_CTRL_TYPE_INTEGER,		"Defect Pixel Correction Count",		0,		255,	1,		0,		V4L2_CTRL_FLAG_READ_ONLY	},		stcam_dd_cam_g_defect_pixel_correction_count,		NULL },
	{ { STC_CID_DEFECT_PIXEL_CORRECTION_MODE,			V4L2_CTRL_TYPE_BOOLEAN,		"Defect Pixel Correction Mode",			0,		1,		1,		0,		0							},		stcam_dd_cam_g_defect_pixel_correction_mode,		stcam_dd_cam_s_defect_pixel_correction_mode },
	{ { STC_CID_DIGITAL_CLAMP,							V4L2_CTRL_TYPE_INTEGER,		"Digital Clamp",						0,		255,	1,		0,		0							},		stcam_dd_cam_g_digital_clamp,						stcam_dd_cam_s_digital_clamp },
	{ { STC_CID_ANALOG_BLACK_LEVEL,						V4L2_CTRL_TYPE_INTEGER,		"Analog Black Level",					0,		1,		1,		0,		0							},		stcam_dd_cam_g_analog_black_level,					stcam_dd_cam_s_analog_black_level },
	{ { STC_CID_ADJUSTMENT_MODE,						V4L2_CTRL_TYPE_BOOLEAN,		"Adjustment Mode",						0,		1,		1,		0,		0							},		stcam_dd_cam_g_adjustment_mode,						stcam_dd_cam_s_adjustment_mode },
	{ { STC_CID_CLEAR_BUFFER,							V4L2_CTRL_TYPE_BUTTON,		"Clear Buffer",							0,		0,		1,		0,		V4L2_CTRL_FLAG_WRITE_ONLY	},		NULL,												stcam_dd_set_s_clear_buffer },
};

struct stcam_st_ctrl_init stcam_dd_set_control_init_table[e_ctrl_id_max] =
{
	{ NULL,								stcam_dd_cam_max_gain,						stcam_dd_cam_g_gain,									stcam_dd_cam_is_enable_gain,								NULL },
	{ NULL,								stcam_dd_cam_max_exposure,					stcam_dd_cam_g_exposure,								NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_hflip,									stcam_dd_cam_is_enable_hflip,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_vflip,									stcam_dd_cam_is_enable_vflip,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_color_array,								NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_transfer_bits_per_pixel,					NULL,														NULL },
	{ NULL,								NULL,										NULL,													stcam_dd_cam_is_enable_temperature,							NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													stcam_dd_cam_is_enable_camera_setting,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_scan_mode,								NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_max_roi_count,							NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_v_decimation_binning,					stcam_dd_cam_is_enable_decimation_binning,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_h_decimation_binning,					stcam_dd_cam_is_enable_decimation_binning,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_binningsum_mode,							stcam_dd_cam_is_enable_binningsum_mode,						NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_clock,									stcam_dd_cam_is_enable_clock,								NULL },
	{ NULL,								stcam_dd_cam_max_v_blank,					stcam_dd_cam_g_v_blank,									stcam_dd_cam_is_enable_v_blank,								NULL },
	{ NULL,								stcam_dd_cam_max_exposure,					stcam_dd_cam_g_max_short_exposure,						NULL,														NULL },
	{ stcam_dd_cam_min_digital_gain,	stcam_dd_cam_max_digital_gain,				stcam_dd_cam_g_digital_gain,							stcam_dd_cam_is_enable_digital_gain,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_mode,							NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_sensor_shutter_mode,						stcam_dd_cam_is_enable_sensor_shutter_mode,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_alc_mode,								stcam_dd_cam_is_enable_alc,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_alc_target_level,						stcam_dd_cam_is_enable_alc,									NULL },
	{ NULL,								stcam_dd_cam_max_gain,						stcam_dd_cam_g_agc_min_gain,							stcam_dd_cam_is_enable_agc,									NULL },
	{ NULL,								stcam_dd_cam_max_gain,						stcam_dd_cam_g_agc_max_gain,							stcam_dd_cam_is_enable_agc,									NULL },
	{ NULL,								stcam_dd_cam_max_exposure,					stcam_dd_cam_g_ae_min_exposure_clock,					stcam_dd_cam_is_enable_ae,									NULL },
	{ NULL,								stcam_dd_cam_max_exposure,					stcam_dd_cam_g_ae_max_exposure_clock,					stcam_dd_cam_is_enable_ae,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_start_trigger_mode,				NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_start_trigger_source,				NULL,														NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_frame_start_trigger_delay,				NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_start_trigger_overlap,				stcam_dd_cam_is_enable_frame_start_trigger_overlap,			NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_burst_start_trigger_mode,			stcam_dd_cam_is_enable_frame_burst_start_trigger,			NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_burst_start_trigger_source,		stcam_dd_cam_is_enable_frame_burst_start_trigger,			NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_frame_burst_start_trigger_delay,			stcam_dd_cam_is_enable_frame_burst_start_trigger,			NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_frame_burst_start_trigger_overlap,		stcam_dd_cam_is_enable_frame_burst_start_trigger_overlap,	NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_start_trigger_mode,				stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_start_trigger_source,			stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_exposure_start_trigger_delay,			stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_start_trigger_overlap,			stcam_dd_cam_is_enable_exposure_trigger_overlap,			NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_end_trigger_mode,				stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_end_trigger_source,				stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_exposure_end_trigger_delay,				stcam_dd_cam_is_enable_exposure_trigger,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_exposure_end_trigger_overlap,			stcam_dd_cam_is_enable_exposure_trigger_overlap,			NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_read_out_start_trigger_mode,				stcam_dd_cam_is_enable_readout,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_read_out_start_trigger_source,			stcam_dd_cam_is_enable_readout,								NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_read_out_start_trigger_delay,			stcam_dd_cam_is_enable_readout,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_read_out_start_trigger_overlap,			stcam_dd_cam_is_enable_readout_overlap,						NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_trigger_wait_hd,							stcam_dd_cam_is_enable_trigger_wait_hd,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_trigger_wait_readout,					stcam_dd_cam_is_enable_trigger_wait_readout,				NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_exposure_delay,							NULL,														NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_strobe_start_delay,						NULL,														NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_strobe_end_delay,						NULL,														NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_output_pulse_delay,						NULL,														NULL },
	{ NULL,								stcam_dd_cam_max_delay,						stcam_dd_cam_g_output_pulse_duration,					NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_readout_delay,							stcam_dd_cam_is_enable_readout,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_line_debounce_time,						stcam_dd_cam_is_enable_line_debounce_time,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io0_direction,							stcam_dd_cam_is_enable_io0,									stcam_dd_cam_is_enable_io_direction_change },
	{ NULL,								NULL,										stcam_dd_cam_g_io0_polarity,							stcam_dd_cam_is_enable_io0,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io0_mode,								stcam_dd_cam_is_enable_io0,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io0_status,								stcam_dd_cam_is_enable_io0,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io1_direction,							stcam_dd_cam_is_enable_io1,									stcam_dd_cam_is_enable_io_direction_change },
	{ NULL,								NULL,										stcam_dd_cam_g_io1_polarity,							stcam_dd_cam_is_enable_io1,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io1_mode,								stcam_dd_cam_is_enable_io1,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io1_status,								stcam_dd_cam_is_enable_io1,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io2_direction,							stcam_dd_cam_is_enable_io2,									stcam_dd_cam_is_enable_io_direction_change },
	{ NULL,								NULL,										stcam_dd_cam_g_io2_polarity,							stcam_dd_cam_is_enable_io2,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io2_mode,								stcam_dd_cam_is_enable_io2,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io2_status,								stcam_dd_cam_is_enable_io2,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io3_direction,							stcam_dd_cam_is_enable_io3,									stcam_dd_cam_is_enable_io_direction_change },
	{ NULL,								NULL,										stcam_dd_cam_g_io3_polarity,							stcam_dd_cam_is_enable_io3,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io3_mode,								stcam_dd_cam_is_enable_io3,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_io3_status,								stcam_dd_cam_is_enable_io3,									NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_led_status,								stcam_dd_cam_is_enable_led_status,							NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_reset_switch,							stcam_dd_cam_is_enable_reset_switch,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_sw_status,								stcam_dd_cam_is_enable_sw_status,							NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_hdr_type,								NULL,														NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_white_balance_mode,						stcam_dd_cam_is_enable_white_balance,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_white_balance_gain_r,					stcam_dd_cam_is_enable_white_balance,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_white_balance_gain_gr,					stcam_dd_cam_is_enable_white_balance,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_white_balance_gain_gb,					stcam_dd_cam_is_enable_white_balance,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_white_balance_gain_b,					stcam_dd_cam_is_enable_white_balance,						NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_camera_gamma,							stcam_dd_cam_is_enable_gamma,								NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_defect_pixel_correction_count,			stcam_dd_cam_is_enable_defect_pixel_correction,				NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_defect_pixel_correction_mode,			stcam_dd_cam_is_enable_defect_pixel_correction,				NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_digital_clamp,							stcam_dd_cam_is_enable_digital_clamp,						NULL },
	{ NULL,								stcam_dd_cam_max_analog_black_level,		stcam_dd_cam_g_analog_black_level,						stcam_dd_cam_is_enable_analog_black_level,					NULL },
	{ NULL,								NULL,										stcam_dd_cam_g_adjustment_mode,							stcam_dd_cam_is_enable_adjustment_mode,						NULL },
	{ NULL,								NULL,										NULL,													NULL,														NULL },
};

static int stcam_dd_set_ext_g_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg);

int stcam_dd_set_init_settings(struct stcam_st_dev_data *ddata)
{
	int i;
	memcpy(ddata->dev_ctrl->ctrl_info, stcam_dd_set_control_table, sizeof(struct stcam_st_ctrl_info) * e_ctrl_id_max);
	for(i=0; i<e_ctrl_id_max; i++){
		if(stcam_dd_set_control_init_table[i].get_min != NULL) {
			ddata->dev_ctrl->ctrl_info[i].query_ctrl.minimum = stcam_dd_set_control_init_table[i].get_min(ddata);
		}
		if(stcam_dd_set_control_init_table[i].get_max != NULL) {
			ddata->dev_ctrl->ctrl_info[i].query_ctrl.maximum = stcam_dd_set_control_init_table[i].get_max(ddata);
		}
		if(stcam_dd_set_control_init_table[i].get_default != NULL) {
			ddata->dev_ctrl->ctrl_info[i].query_ctrl.default_value = stcam_dd_set_control_init_table[i].get_default(ddata);
		}
		if(stcam_dd_set_control_init_table[i].get_readonly != NULL) {
			if(!stcam_dd_set_control_init_table[i].get_readonly(ddata)) {
				ddata->dev_ctrl->ctrl_info[i].query_ctrl.flags = V4L2_CTRL_FLAG_READ_ONLY;
			}
		}
		if(stcam_dd_set_control_init_table[i].get_enable != NULL) {
			if(!stcam_dd_set_control_init_table[i].get_enable(ddata)) {
				ddata->dev_ctrl->ctrl_info[i].query_ctrl.flags = V4L2_CTRL_FLAG_DISABLED;
			}
		}
	}
	return 0;
}

int stcam_dd_set_queryctrl(struct stcam_st_dev_data *ddata, struct v4l2_queryctrl *p_arg)
{
	enum stcam_e_ctrl_id ctrl_id = stcam_dd_set_conv_id(p_arg->id);
	
	if(ctrl_id == e_ctrl_id_invalid) {
		return -EINVAL;
	}
	if(ctrl_id == e_ctrl_id_other) {
		p_arg->flags = V4L2_CTRL_FLAG_DISABLED;
		return -EINVAL;
	}
	
	*p_arg = ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl;
	
	return 0;
}

int stcam_dd_set_querymenu(struct stcam_st_dev_data *ddata, struct v4l2_querymenu *p_arg)
{
	int ret = 0;
	enum stcam_e_ctrl_id ctrl_id = stcam_dd_set_conv_id(p_arg->id);
	
	if(ctrl_id == e_ctrl_id_invalid || ctrl_id == e_ctrl_id_other) {
		return -EINVAL;
	}
	
	if((ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.flags == V4L2_CTRL_FLAG_DISABLED) || (ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.type != V4L2_CTRL_TYPE_MENU)) {
		return -EINVAL;
	}
	
	switch(ctrl_id) {
	case e_ctrl_id_camera_setting:
		switch(p_arg->index) {
		case STC_MENU_CAMERA_SETTING_INITIALIZE:
			strncpy(p_arg->name, "Initialize" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CAMERA_SETTING_STANDARD_READ:
			strncpy(p_arg->name, "Standard_Read" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CAMERA_SETTING_STANDARD_WRITE:
			strncpy(p_arg->name, "Standard_Write" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CAMERA_SETTING_DEF_PIX_POS_READ:
			strncpy(p_arg->name, "Def_Pix_Pos_Read" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CAMERA_SETTING_DEF_PIX_POS_WRITE:
			strncpy(p_arg->name, "Def_Pix_Pos_Write" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_scan_mode:
		{
			__u32 mask = stcam_dd_cam_bitmask_scan_mode(ddata);
			switch(p_arg->index) {
			case STC_MENU_SCAN_MODE_NORMAL:
				strncpy(p_arg->name, "Normal" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_PARTIAL_1:
				if(STC_COM_VALUE_SCAN_MODE_PARTIAL_1 != (mask & STC_COM_VALUE_SCAN_MODE_PARTIAL_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Partial_1/1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_PARTIAL_2:
				if(STC_COM_VALUE_SCAN_MODE_PARTIAL_2 != (mask & STC_COM_VALUE_SCAN_MODE_PARTIAL_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Partial_1/2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_PARTIAL_4:
				if(STC_COM_VALUE_SCAN_MODE_PARTIAL_4 != (mask & STC_COM_VALUE_SCAN_MODE_PARTIAL_4)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Partial_1/4" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_VARIABLE_PARTIAL:
				if(STC_COM_VALUE_SCAN_MODE_VARIABLE_PARTIAL != (mask & STC_COM_VALUE_SCAN_MODE_VARIABLE_PARTIAL)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Variable_Partial" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_BINNING:
				if(STC_COM_VALUE_SCAN_MODE_BINNING != (mask & STC_COM_VALUE_SCAN_MODE_BINNING)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Binning" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_BINNING_PARTIAL_1:
				if(STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_1 != (mask & STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Binning_Partial_1/1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_BINNING_PARTIAL_2:
				if(STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_2 != (mask & STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Binning_Partial_1/2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_BINNING_PARTIAL_4:
				if(STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_4 != (mask & STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_4)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Binning_Partial_1/4" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_BINNING_VARIABLE_PARTIAL:
				if(STC_COM_VALUE_SCAN_MODE_BINNING_VARIABLE_PARTIAL != (mask & STC_COM_VALUE_SCAN_MODE_BINNING_VARIABLE_PARTIAL)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Binning_Variable_Partial" ,sizeof(p_arg->name));
				break;
			case STC_MENU_SCAN_MODE_ROI:
				if(STC_COM_VALUE_SCAN_MODE_ROI != (mask & STC_COM_VALUE_SCAN_MODE_ROI)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "ROI" ,sizeof(p_arg->name));
				break;
			default:
				ret = -EINVAL;
				break;
			}
		}
		break;
	case e_ctrl_id_v_decimation_binning:
		{
			__u32 mask = stcam_dd_cam_bitmask_v_decimation_binning(ddata);
			switch(p_arg->index) {
			case STC_MENU_DECIMATION_BINNING_0_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_0_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_0_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "0_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_4:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_4 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_4)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_4" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_3:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_3 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_3)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_3" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_5_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_5_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_5_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "5_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_5_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_5_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_5_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "5_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_6_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_6_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_6_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "6_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_3:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_3 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_3)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_3" ,sizeof(p_arg->name));
				break;
			default:
				ret = -EINVAL;
				break;
			}
		}
		break;
	case e_ctrl_id_h_decimation_binning:
		{
			__u32 mask = stcam_dd_cam_bitmask_h_decimation_binning(ddata);
			switch(p_arg->index) {
			case STC_MENU_DECIMATION_BINNING_0_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_0_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_0_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "0_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_1_4:
				if(STC_COM_BITMASK_DECIMATION_BINNING_1_4 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_1_4)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "1_4" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_2_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_2_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_2_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "2_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_3_3:
				if(STC_COM_BITMASK_DECIMATION_BINNING_3_3 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_3_3)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "3_3" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_4_2:
				if(STC_COM_BITMASK_DECIMATION_BINNING_4_2 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_4_2)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "4_2" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_5_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_5_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_5_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "5_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_5_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_5_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_5_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "5_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_6_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_6_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_6_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "6_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_0:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_0 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_0)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_0" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_1:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_1 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_1)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_1" ,sizeof(p_arg->name));
				break;
			case STC_MENU_DECIMATION_BINNING_7_3:
				if(STC_COM_BITMASK_DECIMATION_BINNING_7_3 != (mask & STC_COM_BITMASK_DECIMATION_BINNING_7_3)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "7_3" ,sizeof(p_arg->name));
				break;
			default:
				ret = -EINVAL;
				break;
			}
		}
		break;
	case e_ctrl_id_binningsum_mode:
		switch(p_arg->index) {
		case STC_MENU_BINNINGSUM_MODE_OFF:
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_BINNINGSUM_MODE_H_ON:
			strncpy(p_arg->name, "H_On" ,sizeof(p_arg->name));
			break;
		case STC_MENU_BINNINGSUM_MODE_V_ON:
			if(!stcam_dd_cam_is_enable_v_binning_sum(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "V_On" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_clock:
		switch(p_arg->index) {
		case STC_MENU_CLOCK_NORMAL:
			strncpy(p_arg->name, "Normal" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CLOCK_DIV_2:
			strncpy(p_arg->name, "1/2_Clock" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CLOCK_DIV_4:
			strncpy(p_arg->name, "1/4_Clock" ,sizeof(p_arg->name));
			break;
		case STC_MENU_CLOCK_VGA_90FPS:
			if(!stcam_dd_cam_is_enable_90_fps(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "VGA_90fps" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_exposure_mode:
		switch(p_arg->index) {
		case STC_MENU_EXPOSURE_MODE_OFF:
			if(!stcam_dd_cam_is_enable_genicam_io(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_EXPOSURE_MODE_TIMED:
			strncpy(p_arg->name, "Timed" ,sizeof(p_arg->name));
			break;
		case STC_MENU_EXPOSURE_MODE_TRIGGER_WIDTH:
			if(!stcam_dd_cam_is_enable_trigger_width(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Trigger_Width" ,sizeof(p_arg->name));
			break;
		case STC_MENU_EXPOSURE_MODE_TRIGGER_CONTROLLED:
			if(!stcam_dd_cam_is_enable_exposure_trigger(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Trigger_Controlled" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_sensor_shutter_mode:
		switch(p_arg->index) {
		case STC_MENU_SENSOR_SHUTTER_MODE_ROLLING:
			strncpy(p_arg->name, "Rolling" ,sizeof(p_arg->name));
			break;
		case STC_MENU_SENSOR_SHUTTER_MODE_GLOBAL_RESET:
			if(STC_COM_BITMASK_SENSOR_SHUTTER_GLOBAL_RESET != (stcam_dd_cam_bitmask_sensor_shutter_mode(ddata) & STC_COM_BITMASK_SENSOR_SHUTTER_GLOBAL_RESET)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Global_Reset" ,sizeof(p_arg->name));
			break;
		case STC_MENU_SENSOR_SHUTTER_MODE_GLOBAL:
			if(STC_COM_BITMASK_SENSOR_SHUTTER_GLOBAL != (stcam_dd_cam_bitmask_sensor_shutter_mode(ddata) & STC_COM_BITMASK_SENSOR_SHUTTER_GLOBAL)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Global" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_alc_mode:
		switch(p_arg->index) {
		case STC_MENU_ALC_MODE_OFF:
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_ALC_MODE_AE_ON:
			if(!stcam_dd_cam_is_enable_ae(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "AE_On" ,sizeof(p_arg->name));
			break;
		case STC_MENU_ALC_MODE_AGC_ON:
			if(!stcam_dd_cam_is_enable_agc(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "AGC_On" ,sizeof(p_arg->name));
			break;
		case STC_MENU_ALC_MODE_AE_AGC_ON:
			if(!stcam_dd_cam_is_enable_alc(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "AE_AGC_On" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_frame_start_trigger_source:
	case e_ctrl_id_frame_burst_start_trigger_source:
	case e_ctrl_id_exposure_start_trigger_source:
	case e_ctrl_id_exposure_end_trigger_source:
	case e_ctrl_id_read_out_start_trigger_source:
		switch(p_arg->index) {
		case STC_MENU_TRIGGER_SOURCE_DISABLED:
			strncpy(p_arg->name, "Disabled" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOURCE_SOFTWARE:
			strncpy(p_arg->name, "Software" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOURCE_LINE0:
			if(!stcam_dd_cam_is_enable_genicam_io(ddata)) {
				strncpy(p_arg->name, "Hardware" ,sizeof(p_arg->name));
				break;
			}
			if(!stcam_dd_cam_is_enable_io0(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Line0" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOURCE_LINE1:
			if(!stcam_dd_cam_is_enable_genicam_io(ddata)) {
				ret = -EINVAL;
				break;
			}
			if(!stcam_dd_cam_is_enable_io1(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Line1" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOURCE_LINE2:
			if(!stcam_dd_cam_is_enable_genicam_io(ddata)) {
				ret = -EINVAL;
				break;
			}
			if(!stcam_dd_cam_is_enable_io2(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Line2" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOURCE_LINE3:
			if(!stcam_dd_cam_is_enable_genicam_io(ddata)) {
				ret = -EINVAL;
				break;
			}
			if(!stcam_dd_cam_is_enable_io3(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Line3" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_frame_start_trigger_overlap:
	case e_ctrl_id_frame_burst_start_trigger_overlap:
	case e_ctrl_id_exposure_start_trigger_overlap:
	case e_ctrl_id_exposure_end_trigger_overlap:
	case e_ctrl_id_read_out_start_trigger_overlap:
		switch(p_arg->index) {
		case STC_MENU_TRIGGER_OVERLAP_OFF:
			if(!stcam_dd_cam_is_enable_trigger_overlap_off(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_OVERLAP_READ_OUT:
			if(!stcam_dd_cam_is_enable_trigger_overlap_read_out(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Read_out" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME:
			if(!stcam_dd_cam_is_enable_trigger_overlap_previous_frame(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Previous_Frame" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_trigger_software:
		switch(p_arg->index) {
		case STC_MENU_TRIGGER_SOFTWARE_FRAME_START:
			strncpy(p_arg->name, "Frame_Start" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOFTWARE_FRAME_BURST_START:
			if(!stcam_dd_cam_is_enable_frame_burst_start_trigger(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Frame_Burst_Start" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_START:
			if(!stcam_dd_cam_is_enable_exposure_trigger(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Exposure_Start" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_END:
			if(!stcam_dd_cam_is_enable_exposure_trigger(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Exposure_End" ,sizeof(p_arg->name));
			break;
		case STC_MENU_TRIGGER_SOFTWARE_READ_OUT_START:
			if(!stcam_dd_cam_is_enable_readout(ddata)) {
				ret = -EINVAL;
				break;
			}
			strncpy(p_arg->name, "Read_Out_Start" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_io0_direction:
	case e_ctrl_id_io1_direction:
	case e_ctrl_id_io2_direction:
	case e_ctrl_id_io3_direction:
		switch(p_arg->index) {
		case STC_MENU_IO_DIRECTION_INPUT:
			strncpy(p_arg->name, "Input" ,sizeof(p_arg->name));
			break;
		case STC_MENU_IO_DIRECTION_OUTPUT:
			strncpy(p_arg->name, "Output" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_io0_polarity:
	case e_ctrl_id_io1_polarity:
	case e_ctrl_id_io2_polarity:
	case e_ctrl_id_io3_polarity:
		switch(p_arg->index) {
		case STC_MENU_IO_POLARITY_POSI:
			strncpy(p_arg->name, "Posi" ,sizeof(p_arg->name));
			break;
		case STC_MENU_IO_POLARITY_NEGA:
			strncpy(p_arg->name, "Nega" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_io0_mode:
	case e_ctrl_id_io1_mode:
	case e_ctrl_id_io2_mode:
	case e_ctrl_id_io3_mode:
		{
			bool current_inout = false;
			switch(ctrl_id) {
			case e_ctrl_id_io0_mode:
				current_inout = stcam_dd_cam_g_io0_direction(ddata);
				break;
			case e_ctrl_id_io1_mode:
				current_inout = stcam_dd_cam_g_io1_direction(ddata);
				break;
			case e_ctrl_id_io2_mode:
				current_inout = stcam_dd_cam_g_io2_direction(ddata);
				break;
			case e_ctrl_id_io3_mode:
				current_inout = stcam_dd_cam_g_io3_direction(ddata);
				break;
			default:
				break;
			}
			switch(p_arg->index) {
			case STC_MENU_IO_MODE_OUT_DISABLE:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Disabled" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "General_Output" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Trigger_Output_Programmable" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_photocoupler(ddata)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Trigger_Output_Loop_Through" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_EXPOSURE_END:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Exposure_End_Output" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "CCD_Read_End_Output" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Strobe_Output_Programmable" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Strobe_Output_Exposure" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_valid_out(ddata)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Trigger_Output_Valid_Out" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_OUT_TRANSFER_END:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && !current_inout) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_transfer_end_out(ddata)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Transfer_Output_End" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_IN_DISABLE:
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Disabled" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_IN_GENERAL_INPUT:
				if(stcam_dd_cam_is_enable_genicam_io(ddata)) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "General_Input" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_IN_TRIGGER_INPUT:
				if(stcam_dd_cam_is_enable_genicam_io(ddata)) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && current_inout) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Trigger_Input" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_IN_READOUT_INPUT:
				if(stcam_dd_cam_is_enable_genicam_io(ddata)) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && current_inout) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_readout(ddata)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Readout_Input" ,sizeof(p_arg->name));
				break;
			case STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT:
				if(stcam_dd_cam_is_enable_genicam_io(ddata)) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_io_direction_change(ddata) && current_inout) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_exposure_trigger(ddata)) {
					ret = -EINVAL;
					break;
				}
				if(!stcam_dd_cam_is_enable_exposure_end_trigger_source(ddata)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "Sub_Trigger_Input" ,sizeof(p_arg->name));
				break;
			default:
				ret = -EINVAL;
				break;
			}
		}
		break;
	case e_ctrl_id_io0_status:
	case e_ctrl_id_io1_status:
	case e_ctrl_id_io2_status:
	case e_ctrl_id_io3_status:
		switch(p_arg->index) {
		case STC_MENU_IO_STATUS_LOW:
			strncpy(p_arg->name, "Low" ,sizeof(p_arg->name));
			break;
		case STC_MENU_IO_STATUS_HIGH:
			strncpy(p_arg->name, "High" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_led_status:
		switch(p_arg->index) {
		case STC_MENU_LED_STATUS_OFF:
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_LED_STATUS_GREEN_ON:
			strncpy(p_arg->name, "Green_On" ,sizeof(p_arg->name));
			break;
		case STC_MENU_LED_STATUS_RED_ON:
			strncpy(p_arg->name, "Red_On" ,sizeof(p_arg->name));
			break;
		case STC_MENU_LED_STATUS_GREEN_RED_ON:
			strncpy(p_arg->name, "Green_Red_On" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_white_balance_mode:
		switch(p_arg->index) {
		case STC_MENU_WHITE_BALANCE_MODE_OFF:
			strncpy(p_arg->name, "Off" ,sizeof(p_arg->name));
			break;
		case STC_MENU_WHITE_BALANCE_MODE_MANUAL:
			strncpy(p_arg->name, "Manual" ,sizeof(p_arg->name));
			break;
		case STC_MENU_WHITE_BALANCE_MODE_FULLAUTO:
			strncpy(p_arg->name, "Auto" ,sizeof(p_arg->name));
			break;
		case STC_MENU_WHITE_BALANCE_MODE_ONESHOT:
			strncpy(p_arg->name, "OneShot" ,sizeof(p_arg->name));
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	case e_ctrl_id_transfer_bits_per_pixel:
		{
			__u32 mask = stcam_dd_cam_bitmask_transfer_bits_per_pixel(ddata);
			switch(p_arg->index) {
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "RAW8" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "RAW10" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10P:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10P != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10P)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "RAW10P" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "RAW12" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12P:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12P != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12P)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "RAW12P" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "MONO8" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "MONO10" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10P:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10P != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10P)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "MONO10P" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "MONO12" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12P:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12P != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12P)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "MONO12P" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "BGR8" ,sizeof(p_arg->name));
				break;
			case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10:
				if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10 != (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10)) {
					ret = -EINVAL;
					break;
				}
				strncpy(p_arg->name, "BGR10" ,sizeof(p_arg->name));
				break;
			default:
				ret = -EINVAL;
				break;
			}
			break;
		}
	default:
		ret = -EINVAL;
		break;
	}
	
	return ret;
}

int stcam_dd_set_g_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg)
{
	enum stcam_e_ctrl_id ctrl_id;
	
	if(p_arg->id < STC_CID_EXT_BASE) {
		ctrl_id = stcam_dd_set_conv_id(p_arg->id);
		
		if(ctrl_id == e_ctrl_id_invalid || ctrl_id == e_ctrl_id_other) {
			return -EINVAL;
		}
		
		if((ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.flags == V4L2_CTRL_FLAG_DISABLED) || (ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.flags == V4L2_CTRL_FLAG_WRITE_ONLY)) {
			return -EACCES;
		}
		
		p_arg->value = ddata->dev_ctrl->ctrl_info[ctrl_id].get_ops(ddata);
	} else {
		return stcam_dd_set_ext_g_settings(ddata, p_arg);
	}
	
	return 0;
}

int stcam_dd_set_s_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg)
{
	int ret = 0;
	enum stcam_e_ctrl_id ctrl_id = stcam_dd_set_conv_id(p_arg->id);
	
	if(ctrl_id == e_ctrl_id_invalid || ctrl_id == e_ctrl_id_other) {
		return -EINVAL;
	}
	
	if((ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.flags == V4L2_CTRL_FLAG_DISABLED) || (ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.flags == V4L2_CTRL_FLAG_READ_ONLY)) {
		printk(KERN_DEBUG "stcam_dd_set_s_settings err id[%d] value[%d]", ctrl_id, p_arg->value);
		return -EACCES;
	}
	
	if((ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.minimum > p_arg->value) || (ddata->dev_ctrl->ctrl_info[ctrl_id].query_ctrl.maximum < p_arg->value)) {
		return -ERANGE;
	}
	
	ret = ddata->dev_ctrl->ctrl_info[ctrl_id].set_ops(ddata, p_arg->value);
	if(ret < 0) {
		printk(KERN_DEBUG "stcam_dd_set_s_settings err id[%d] value[%d]", ctrl_id, p_arg->value);
		stcam_dd_util_error_log_return(ret);
	}
	return 0;
}

enum stcam_e_ctrl_id stcam_dd_set_conv_id(__u32 id){
	enum stcam_e_ctrl_id ret_id = e_ctrl_id_invalid;
	
	switch(id) {
	case V4L2_CID_GAIN:
		ret_id = e_ctrl_id_gain;
		break;
	case V4L2_CID_EXPOSURE:
		ret_id = e_ctrl_id_exposure;
		break;
	case V4L2_CID_HFLIP:
		ret_id = e_ctrl_id_hflip;
		break;
	case V4L2_CID_VFLIP:
		ret_id = e_ctrl_id_vflip;
		break;
	default:
		if(id >= V4L2_CID_PRIVATE_BASE) {
			ret_id = (enum stcam_e_ctrl_id)(id - V4L2_CID_PRIVATE_BASE + 4);
			if((ret_id < e_ctrl_id_gain) || (e_ctrl_id_max < ret_id))
			{
				ret_id = e_ctrl_id_invalid;
			}
		} else {
			if(id < V4L2_CID_LASTP1) {
				ret_id = e_ctrl_id_other;
			}
		}
		break;
	}
	
	return ret_id;
}

__u32 stcam_dd_set_g_timeout_st2ee(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->timeout_soft_trigger;
}

__u32 stcam_dd_set_g_timeout_te2ee(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->timeout_hard_trigger;
}

__u32 stcam_dd_set_g_timeout_ee2te(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->timeout_exposure;
}

__u32 stcam_dd_set_g_timeout_ro2te(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->timeout_readout;
}

__u32 stcam_dd_set_g_exposure_end_callback(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->is_callback_exposure_end_enable;
}

__u32 stcam_dd_set_g_transfer_end_callback(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->is_callback_image_trancefer_enable;
}

__u32 stcam_dd_set_g_rcv_error_callback(struct stcam_st_dev_data *ddata)
{
	return ddata->usb_ctrl->is_callback_error_enable;
}

int stcam_dd_set_s_timeout_st2ee(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->timeout_soft_trigger = param * HZ / 1000;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_timeout_te2ee(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->timeout_hard_trigger = param * HZ / 1000;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_timeout_ee2te(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->timeout_exposure = param * HZ / 1000;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_timeout_ro2te(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->timeout_readout = param * HZ / 1000;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_exposure_end_callback(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->is_callback_exposure_end_enable = param;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_transfer_end_callback(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->is_callback_image_trancefer_enable = param;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_rcv_error_callback(struct stcam_st_dev_data *ddata, __u32 param)
{
	unsigned long flags;
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	ddata->usb_ctrl->is_callback_error_enable = param;
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

int stcam_dd_set_s_clear_buffer(struct stcam_st_dev_data *ddata, __u32 param)
{
	int i;
	unsigned long flags;
	struct list_head *pos, *tmp;
	struct stcam_st_picture_image *image = NULL;
	
	spin_lock_irqsave(&ddata->usb_ctrl->spinlock, flags);
	if(ddata->picture_image) {
		for(i=0; i<ddata->usb_ctrl->image_buf_num; i++) {
			memset(ddata->picture_image[i].image_buffer, 0, ddata->dev_ctrl->buffer_size);
		}
	}
	if(0 == list_empty(&ddata->usb_ctrl->list_buffer_full_queue)) {
		list_for_each_safe(pos, tmp, &ddata->usb_ctrl->list_buffer_full_queue) {
				image = list_entry(pos, struct stcam_st_picture_image, list_head_full_queue);
				list_del_init(&image->list_head_full_queue);
				list_add_tail(&image->list_head_empty_queue, &ddata->usb_ctrl->list_buffer_empty_queue);
		}
	}
	spin_unlock_irqrestore(&ddata->usb_ctrl->spinlock, flags);
	return 0;
}

static int stcam_dd_set_ext_g_settings(struct stcam_st_dev_data *ddata, struct v4l2_control *p_arg)
{
	switch(p_arg->id) {
	case STC_CID_EXT_IMAGE_WIDTH:
		p_arg->value = stcam_dd_cam_g_image_width(ddata);
		break;
	case STC_CID_EXT_IMAGE_HEIGHT:
		p_arg->value = stcam_dd_cam_g_image_height(ddata);
		break;
	case STC_CID_EXT_TOTAL_H_CLOCK:
		p_arg->value = stcam_dd_cam_g_total_h_clock(ddata);
		break;
	case STC_CID_EXT_TOTAL_V_LINE:
		p_arg->value = stcam_dd_cam_g_total_v_line(ddata);
		break;
	case STC_CID_EXT_CLOCK_HZ:
		p_arg->value = stcam_dd_cam_g_clock_hz(ddata);
		break;
	case STC_CID_EXT_TRANSFER_BITS_PER_PIXEL:
		p_arg->value = stcam_dd_cam_g_transfer_bits_per_pixel(ddata);
		break;
	case STC_CID_EXT_RAW_IMAGE_SIZE:
		p_arg->value = stcam_dd_cam_g_raw_image_size(ddata);
		break;
	case STC_CID_EXT_H_BINNING:
		p_arg->value = stcam_dd_cam_g_h_binning(ddata);
		break;
	case STC_CID_EXT_V_BINNING:
		p_arg->value = stcam_dd_cam_g_v_binning(ddata);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 15, 0))
//Deprecated /uses deprecate timer from older Kernel
void stcam_dd_set_time_out(unsigned long data)
{
    struct stcam_st_dev_data *ddata = (struct stcam_st_dev_data*)data;
    struct siginfo info;

    if(ddata->usb_ctrl->is_callback_error_enable) {
        info.si_int = ddata->usb_ctrl->current_timer;
        send_sig_info(STC_SIGNAL_CALLBACK_ERROR, &info, ddata->usb_ctrl->task);
    }
}
#else
//KL: New Time Call Back, Typed adhering to Kernel 4.15
void stcam_dd_set_time_out(struct timer_list * tm)
{
        //stcam_dd_set_time_out(tm->data);
        // Retrieve the custom struct that glued timer_list and data together
        struct stcam_st_dev_timer_data *tmd =  from_timer(tmd, tm, timer);
        struct stcam_st_dev_data *ddata = (struct stcam_st_dev_data*)tmd->ddata;
        struct siginfo info;

        if(ddata->usb_ctrl->is_callback_error_enable) {
            info.si_int = ddata->usb_ctrl->current_timer;
            send_sig_info(STC_SIGNAL_CALLBACK_ERROR, &info, ddata->usb_ctrl->task);
        }

}


#endif



