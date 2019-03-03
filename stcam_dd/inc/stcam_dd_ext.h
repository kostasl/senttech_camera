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

#ifndef __STCAM_DD_EXT_H__
#define __STCAM_DD_EXT_H__

#include <linux/videodev2.h>

#define STC_CID_EXT_BASE							V4L2_CID_PRIVATE_BASE + 1024
#define STC_CID_EXT_IMAGE_WIDTH						STC_CID_EXT_BASE + 0
#define STC_CID_EXT_IMAGE_HEIGHT					STC_CID_EXT_BASE + 1
#define STC_CID_EXT_TOTAL_H_CLOCK					STC_CID_EXT_BASE + 2
#define STC_CID_EXT_TOTAL_V_LINE					STC_CID_EXT_BASE + 3
#define STC_CID_EXT_CLOCK_HZ						STC_CID_EXT_BASE + 4
#define STC_CID_EXT_TRANSFER_BITS_PER_PIXEL			STC_CID_EXT_BASE + 5
#define STC_CID_EXT_RAW_IMAGE_SIZE					STC_CID_EXT_BASE + 6
#define STC_CID_EXT_H_BINNING						STC_CID_EXT_BASE + 7
#define STC_CID_EXT_V_BINNING						STC_CID_EXT_BASE + 8

#pragma pack(push,1)
struct stcam_st_ext_init {
	__u32	pid;
	__u16	initial_gain;
	__u32	gain_type;
	__u16	max_gain;
	__u8	has_digital_gain_function;
	__u8	digital_gain_type;
	__u16	max_digital_gain;
	__u16	enable_defect_pixel_correction_count;
	__u8	has_v_blank_function;
	__u8	max_roi_count;
	__u32	max_long_exposure_clock;
	__u32	exposure_time_base;
	__u32	exposure_time_offset;
};

#pragma pack(pop)

#define STCIOC_G_CAM_EXT_INIT_DEF					0xE0
#define STCIOC_G_CAM_EXT_INIT			_IOR('V',	STCIOC_G_CAM_EXT_INIT_DEF,		struct stcam_st_ext_init)

#endif//__STCAM_DD_EXT_H__
