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

#include <linux/usb.h>

#include "../inc/stcam_dd_cam.h"
#include "../inc/stcam_dd_cam_def.h"
#include "../inc/stcam_dd_usb.h"
#include "../inc/stcam_dd_util.h"

// data table
static struct stcam_st_camera_model cam_model_table[e_cam_product_max] =
{
	{ "STC-MB33USB",		STC_MB33USB,		0x0000,		640,	480,	60,		e_camera_category_type_0,	},
	{ "STC-MC33USB",		STC_MC33USB,		0x0000,		640,	480,	60,		e_camera_category_type_0,	},
	{ "STC-SB33USB",		STC_MB33USB,		0x0001,		640,	480,	60,		e_camera_category_type_0,	},
	{ "STC-SC33USB",		STC_MC33USB,		0x0001,		640,	480,	60,		e_camera_category_type_0,	},
	{ "STC-MB83USB",		STC_MB83USB,		0x0000,		1024,	768,	30,		e_camera_category_type_0,	},
	{ "STC-MC83USB",		STC_MC83USB,		0x0000,		1024,	768,	30,		e_camera_category_type_0,	},
	{ "STC-SB83USB",		STC_MB83USB,		0x0001,		1024,	768,	30,		e_camera_category_type_0,	},
	{ "STC-SC83USB",		STC_MC83USB,		0x0001,		1024,	768,	30,		e_camera_category_type_0,	},
	{ "STC-MB133USB",		STC_MB133USB,		0x0000,		1280,	960,	22,		e_camera_category_type_0,	},
	{ "STC-MC133USB",		STC_MC133USB,		0x0000,		1280,	960,	22,		e_camera_category_type_0,	},
	{ "STC-SB133USB",		STC_MB133USB,		0x0001,		1280,	960,	22,		e_camera_category_type_0,	},
	{ "STC-SC133USB",		STC_MC133USB,		0x0001,		1280,	960,	22,		e_camera_category_type_0,	},
	{ "STC-MB152USB",		STC_MB152USB,		0x0000,		1360,	1024,	19,		e_camera_category_type_0,	},
	{ "STC-MC152USB",		STC_MC152USB,		0x0000,		1360,	1024,	19,		e_camera_category_type_0,	},
	{ "STC-MB202USB",		STC_MB202USB,		0x0000,		1600,	1200,	15,		e_camera_category_type_0,	},
	{ "STC-MC202USB",		STC_MC202USB,		0x0000,		1600,	1200,	15,		e_camera_category_type_0,	},
	{ "STC-MBA5MUSB3",		STC_MBA5MUSB3,		0x0000,		2592,	1944,	14,		e_camera_category_type_1,	},
	{ "STC-MCA5MUSB3",		STC_MCA5MUSB3,		0x0000,		2592,	1944,	14,		e_camera_category_type_1,	},
	{ "STC-MBE132U3V",		STC_MBE132U3V,		0x0000,		1280,	1024,	61,		e_camera_category_type_2,	},
	{ "STC-MCE132U3V",		STC_MCE132U3V,		0x0000,		1280,	1024,	61,		e_camera_category_type_2,	},
	{ "STC-MBCM401U3V",		STC_MBCM401U3V,		0x0000,		2048,	2048,	89,		e_camera_category_type_3,	},
	{ "STC-MCCM401U3V",		STC_MCCM401U3V,		0x0000,		2048,	2048,	89,		e_camera_category_type_3,	},
	{ "STC-MBCM200U3V",		STC_MBCM200U3V,		0x0000,		2048,	1088,	167,	e_camera_category_type_3,	},
	{ "STC-MCCM200U3V",		STC_MCCM200U3V,		0x0000,		2048,	1088,	167,	e_camera_category_type_3,	},
	{ "STC-MBS231U3V",		STC_MBS241U3V,		0x0001,		1920,	1200,	42,		e_camera_category_type_4,	},
	{ "STC-MCS231U3V",		STC_MCS241U3V,		0x0001,		1920,	1200,	42,		e_camera_category_type_4,	},
	{ "STC-MBS241U3V",		STC_MBS241U3V,		0x0000,		1920,	1200,	163,	e_camera_category_type_4,	},
	{ "STC-MCS241U3V",		STC_MCS241U3V,		0x0000,		1920,	1200,	163,	e_camera_category_type_4,	},
	{ "STC-MBS500U3V",		STC_MBS510U3V,		0x0001,		2448,	2048,	36,		e_camera_category_type_5,	},
	{ "STC-MCS500U3V",		STC_MCS510U3V,		0x0001,		2448,	2048,	36,		e_camera_category_type_5,	},
	{ "STC-MBS510U3V",		STC_MBS510U3V,		0x0000,		2448,	2048,	76,		e_camera_category_type_5,	},
	{ "STC-MCS510U3V",		STC_MCS510U3V,		0x0000,		2448,	2048,	76,		e_camera_category_type_5,	},
	{ "STC-MBS312U3V",		STC_MBS322U3V,		0x0001,		2048,	1536,	56,		e_camera_category_type_5,	},
	{ "STC-MCS312U3V",		STC_MCS322U3V,		0x0001,		2048,	1536,	56,		e_camera_category_type_5,	},
	{ "STC-MBS322U3V",		STC_MBS322U3V,		0x0000,		2048,	1536,	121,	e_camera_category_type_5,	},
	{ "STC-MCS322U3V",		STC_MCS322U3V,		0x0000,		2048,	1536,	121,	e_camera_category_type_5,	},
	{ "STC-MBS163U3V",		STC_MBS163U3V,		0x0000,		1440,	1080,	238,	e_camera_category_type_5,	},
	{ "STC-MCS163U3V",		STC_MCS163U3V,		0x0000,		1440,	1080,	238,	e_camera_category_type_5,	},
	{ "STC-MBS43U3V",		STC_MBS43U3V,		0x0000,		720,	540,	527,	e_camera_category_type_6,	},
	{ "STC-MCS43U3V",		STC_MCS43U3V,		0x0000,		720,	540,	527,	e_camera_category_type_6,	},
	{ "STC-MBS891U3V",		STC_MBS891U3V,		0x0000,		4096,	2160,	42,		e_camera_category_type_7,	},
	{ "STC-MCS891U3V",		STC_MCS891U3V,		0x0000,		4096,	2160,	42,		e_camera_category_type_7,	},
	{ "STC-MBS881U3V",		STC_MBS891U3V,		0x0001,		4096,	2160,	32,		e_camera_category_type_7,	},
	{ "STC-MCS881U3V",		STC_MCS891U3V,		0x0001,		4096,	2160,	32,		e_camera_category_type_7,	},
	{ "STC-MBS891U3V",		STC_MBS891U3V,		0x0002,		4096,	2160,	42,		e_camera_category_type_7,	},
	{ "STC-MCS891U3V",		STC_MCS891U3V,		0x0002,		4096,	2160,	42,		e_camera_category_type_7,	},
	{ "STC-MBS123BU3V",		STC_MBS123BU3V,		0x0000,		4096,	3000,	30,		e_camera_category_type_7,	},
	{ "STC-MCS123BU3V",		STC_MCS123BU3V,		0x0000,		4096,	3000,	30,		e_camera_category_type_7,	},
	{ "STC-MBS122BU3V",		STC_MBS123BU3V,		0x0001,		4096,	3000,	23,		e_camera_category_type_7,	},
	{ "STC-MCS122BU3V",		STC_MCS123BU3V,		0x0001,		4096,	3000,	23,		e_camera_category_type_7,	},
	{ "STC-MBS123BU3V",		STC_MBS123BU3V,		0x0002,		4096,	3000,	30,		e_camera_category_type_7,	},
	{ "STC-MCS123BU3V",		STC_MCS123BU3V,		0x0002,		4096,	3000,	30,		e_camera_category_type_7,	}
};

const __u16 cam_fpga2_category_table[e_fpga2_category_max] =
{
	STC_COM_FPGA2_CATEGORY_OFFSET_ACQUISITION_CONTROL,
	STC_COM_FPGA2_CATEGORY_OFFSET_ANALOG_CONTROL,
	STC_COM_FPGA2_CATEGORY_OFFSET_DIGITAL_IO_CONTROL,
	STC_COM_FPGA2_CATEGORY_OFFSET_VENDOR_UNIQUE_CONTROL,
	STC_COM_FPGA2_CATEGORY_OFFSET_USB_CAMERA_ORG_FUNCTION,
	STC_COM_FPGA2_CATEGORY_OFFSET_USB_CAMERA_STATUS,
	STC_COM_FPGA2_CATEGORY_OFFSET_USB_CAMERA_FEATURE,
	STC_COM_FPGA2_CATEGORY_OFFSET_HDR_CONTROL,
};

static struct stcam_st_fpga2_offset cam_fpga2_table[e_fpga2_sub_category_max] =
{
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FPS_BY_V_BLANKING				},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_MODE					},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_TRIGGER_SOFTWARE				},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_START_TRIGGER_MODE		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_START_TRIGGER_SOURCE		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_START_TRIGGER_DELAY		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_START_TRIGGER_OVERLAP	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_BURST_TRIGGER_MODE		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_BURST_TRIGGER_SOURCE		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_BURST_TRIGGER_DELAY		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_FRAME_BURST_TRIGGER_OVERLAP	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_START_TRIGGER_MODE	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_START_TRIGGER_SOURCE	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_START_TRIGGER_DELAY	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_START_TRIGGER_OVERLAP	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_END_TRIGGER_MODE		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_END_TRIGGER_SOURCE	},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_END_TRIGGER_DELAY		},
	{ e_fpga2_acquisition_control,				STC_COM_FPGA2_SUB_OFFSET_ACQUISITION_CONTROL_EXPOSURE_END_TRIGGER_OVERLAP	},
	{ e_fpga2_analog_control,					STC_COM_FPGA2_SUB_OFFSET_ANALOG_CONTROL_ANALOG_OFFSET						},
	{ e_fpga2_digital_io_control,				STC_COM_FPGA2_SUB_OFFSET_DIGITAL_IO_CONTROL_LINE_DEBOUNCE_TIME				},
	{ e_fpga2_vendor_unique_control,			STC_COM_FPGA2_SUB_OFFSET_VENDOR_UNIQUE_CONTROL_SENSOR_SHUTTER_MODE			},
	{ e_fpga2_vendor_unique_control,			STC_COM_FPGA2_SUB_OFFSET_VENDOR_UNIQUE_CONTROL_DIGITAL_GAIN_ENABLED			},
	{ e_fpga2_usb_camera_org_function,			STC_COM_FPGA2_SUB_OFFSET_USB_ORIGINAL_FUNCTION_SAVE_IO_RESET_SWITCH_ENABLED	},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_FPS_DECIMAL						},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_FRAME_RATE_BASE					},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_MAX_V_BLANKING_FOR_FPS			},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_CAMERA_TEMPERATURE				},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_EXPOSURE_TIME_BASE				},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_MAX_ANALOG_BLACK_LEVEL			},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_MAX_DIGITAL_GAIN					},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_MIN_DIGITAL_GAIN					},
	{ e_fpga2_usb_camera_status,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_STATUS_EXPOSURE_TIME_OFFSET				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_1				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_3				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_5				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_9				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_A				},
	{ e_fpga2_usb_camera_feature,				STC_COM_FPGA2_SUB_OFFSET_USB_CAMERA_FEATURE_CAMERA_FEATURE_C				},
	{ e_fpga2_hdr_control,						STC_COM_FPGA2_SUB_OFFSET_HDR_CONTROL_SIZE									},
	{ e_fpga2_hdr_control,						STC_COM_FPGA2_SUB_OFFSET_HDR_CONTROL_PARAMETOR_0							},
	{ e_fpga2_hdr_control,						STC_COM_FPGA2_SUB_OFFSET_HDR_CONTROL_PARAMETOR_1							},
};

const __u32 stcam_dd_cam_bpp_table[] =
{
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10P,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12P,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10P,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12P,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08,
	STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10,
};

const __u32 stcam_dd_cam_pixelformat_table[12][5] =
{
	// MONO						RGGB						GRBG						GBRG						BGGR
	{ V4L2_PIX_FMT_GREY,		V4L2_PIX_FMT_SRGGB8,		V4L2_PIX_FMT_SGRBG8,		V4L2_PIX_FMT_SGBRG8,		V4L2_PIX_FMT_SBGGR8		}, // RAW_08
	{ V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_SRGGB10,		V4L2_PIX_FMT_SGRBG10,		V4L2_PIX_FMT_SGBRG10,		V4L2_PIX_FMT_SBGGR10	}, // RAW_10
	{ V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_SRGGB10,		V4L2_PIX_FMT_SGRBG10,		V4L2_PIX_FMT_SGBRG10,		V4L2_PIX_FMT_SBGGR10	}, // RAW_10P
	{ V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_SRGGB12,		V4L2_PIX_FMT_SGRBG12,		V4L2_PIX_FMT_SGBRG12,		V4L2_PIX_FMT_SBGGR12	}, // RAW_12
	{ V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_SRGGB12,		V4L2_PIX_FMT_SGRBG12,		V4L2_PIX_FMT_SGBRG12,		V4L2_PIX_FMT_SBGGR12	}, // RAW_12P
	{ V4L2_PIX_FMT_GREY,		V4L2_PIX_FMT_GREY,			V4L2_PIX_FMT_GREY,			V4L2_PIX_FMT_GREY,			V4L2_PIX_FMT_GREY		}, // MONO_08
	{ V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10		}, // MONO_10
	{ V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10,			V4L2_PIX_FMT_Y10		}, // MONO_10P
	{ V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12		}, // MONO_12
	{ V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12,			V4L2_PIX_FMT_Y12		}, // MONO_12P
	{ V4L2_PIX_FMT_BGR24,		V4L2_PIX_FMT_BGR24,			V4L2_PIX_FMT_BGR24,			V4L2_PIX_FMT_BGR24,			V4L2_PIX_FMT_BGR24		}, // BGR_08
	{ V4L2_PIX_FMT_BGR32,		V4L2_PIX_FMT_BGR32,			V4L2_PIX_FMT_BGR32,			V4L2_PIX_FMT_BGR32,			V4L2_PIX_FMT_BGR32		}, // BGR_10
};


//static function define
static int read_camera_info(struct stcam_st_dev_data *ddata);
static int read_picture_size(struct stcam_st_dev_data *ddata, __u16 length);
static int read_pixel_byte(struct stcam_st_dev_data *ddata);
static int read_shutter_gain(struct stcam_st_dev_data *ddata);
static int read_clock(struct stcam_st_dev_data *ddata);
static int read_fpga(struct stcam_st_dev_data *ddata);
static int read_fpga_pix_correct(struct stcam_st_dev_data *ddata, __u16 index, __u32 *param);
static int read_fpga2(struct stcam_st_dev_data *ddata, __u16 index);
static int read_fpga2_category_offset(struct stcam_st_dev_data *ddata, __u16 index);
static int read_eeprom_w(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param);
static int read_eeprom_info(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length,  __u8 *param);
static int write_picture_size(struct stcam_st_dev_data *ddata);
static int write_pixel_byte(struct stcam_st_dev_data *ddata);
static int write_shutter_gain(struct stcam_st_dev_data *ddata);
static int write_clock(struct stcam_st_dev_data *ddata);
static int write_software_triger(struct stcam_st_dev_data *ddata);
static int write_fpga_s(struct stcam_st_dev_data *ddata, __u16 addr, __u16 option);
static int write_fpga_l(struct stcam_st_dev_data *ddata, __u16 addr, __u16 option);
static int refresh_fpga(struct stcam_st_dev_data *ddata);
static int write_fpga_pix_correct(struct stcam_st_dev_data *ddata, __u16 index, __u32 *param);
static int write_fx3(struct stcam_st_dev_data *ddata, __u16 *param);
static int write_fpga2(struct stcam_st_dev_data *ddata, __u16 index);
static int write_eeprom_w(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param);
static int write_eeprom_info(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param);
static int stcam_dd_cam_update_scan_info(struct stcam_st_dev_data *ddata);
static struct v4l2_fract stcam_dd_cam_g_bytes_per_pixel(__u16 bits_per_pixel);
static struct v4l2_fract stcam_dd_cam_g_max_bytes_per_pixel(__u32 bitmask_transfer_bits_per_pixel);

//functions
int stcam_dd_cam_init_camera_data(struct stcam_st_dev_data *ddata){
	int ret,i,j;
	__u16 product_id, camera_type;
	__u32 enable_transfer_bpp;
	enum stcam_e_cam_product product = e_cam_product_invalid;
	struct v4l2_fract bytes_per_pixel;
	stcam_dd_util_function_log();
	
	// Camera Info
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_camera_info, STC_COM_LEN_CAMERA_INFO);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	ret = read_camera_info(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Picture Size
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_picture_size, STC_COM_LEN_PICTURE_SIZE);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	ret = read_picture_size(ddata, STC_COM_LEN_PICTURE_SIZE);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	if(ddata->cam_param_picture_size[STC_COM_ADR_PICTURE_SIZE_LENGTH] != STC_COM_LEN_PICTURE_SIZE) {
		if(ddata->cam_param_picture_size[STC_COM_ADR_PICTURE_SIZE_LENGTH] <= 255) {
			ddata->picture_size_length = ddata->cam_param_picture_size[STC_COM_ADR_PICTURE_SIZE_LENGTH];
		} else {
			ddata->picture_size_length = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_LEN);
		}
		ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_picture_size, ddata->picture_size_length);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
		ret = read_picture_size(ddata, ddata->picture_size_length);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
	} else {
		ddata->picture_size_length = STC_COM_LEN_PICTURE_SIZE;
	}
	
	// Pixel Byte
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_pixel_byte, STC_COM_LEN_PIXEL_BYTE);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	ret = read_pixel_byte(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Shutter Gain
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_shutter_gain, STC_COM_LEN_SHUTTER_GAIN);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	ret = read_shutter_gain(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Clock
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_clock, STC_COM_LEN_CLOCK);
	if(ret < 0) {
		return ret;
	}
	ret = read_clock(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// FPGA
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_fpga, STC_COM_LEN_FPGA);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	ret = read_fpga(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Model Detect
	product_id = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_PID);
	camera_type = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION7];
	
	for(i=0; i<e_cam_product_max; i++) {
		if((cam_model_table[i].product_id == product_id) && (cam_model_table[i].camera_type == camera_type)){
			ddata->dev_ctrl->product = product = i;
			break;
		}
	}
	if(product == e_cam_product_invalid) {
		ret = -EFAULT;
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// FPGA2
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_fpga2_category_offset, e_fpga2_category_max * sizeof(__u32));
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	for(i=0; i<e_fpga2_category_max; i++) {
		ret = read_fpga2_category_offset(ddata, i);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
	}
	ret = stcam_dd_mem_alloc_camera_param(ddata, e_cam_param_fpga2, STC_COM_LEN_FPGA2);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	for(i=0; i<e_fpga2_sub_category_max; i++) {
		ret = read_fpga2(ddata, i);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
	}
	
	enable_transfer_bpp = stcam_dd_cam_bitmask_transfer_bits_per_pixel(ddata);
	bytes_per_pixel = stcam_dd_cam_g_max_bytes_per_pixel(enable_transfer_bpp);
	ddata->dev_ctrl->buffer_size = cam_model_table[ddata->dev_ctrl->product].width * cam_model_table[ddata->dev_ctrl->product].height * bytes_per_pixel.numerator / bytes_per_pixel.denominator;
	ddata->dev_ctrl->buffer_size = ddata->dev_ctrl->buffer_size * 2;
	j = 0;
	for(i=0; i<(sizeof(stcam_dd_cam_bpp_table)/sizeof(__u32)); i++) {
		if((stcam_dd_cam_bpp_table[i] & enable_transfer_bpp)) {
			ddata->dev_ctrl->bpp_list[j] = stcam_dd_cam_bpp_table[i];
			j++;
		}
	}
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	
	// split mode
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_PACKET_SPLIT_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (1 << STC_COM_BITSHIFT_FPGA_PACKET_SPLIT_MODE) & STC_COM_BITMASK_FPGA_PACKET_SPLIT_MODE;
	ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// sentech mode
	ret = stcam_dd_cam_s_sentech_mode(ddata, 0x0000);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	return 0;
}

bool stcam_dd_cam_is_enable_gain(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_DISABLED_ANALOG_GAIN ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_hflip(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_MIRROR_HORIZONTAL ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_vflip(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_MIRROR_VERTICAL ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_temperature(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_DEVICE_TEMPERATURE_MAINBOARD ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_camera_setting(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_STORE_CAMERA_SETTING ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_decimation_binning(struct stcam_st_dev_data *ddata)
{
	if((cam_model_table[ddata->dev_ctrl->product].category != e_camera_category_type_0)
		&& (cam_model_table[ddata->dev_ctrl->product].category != e_camera_category_type_6)) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_binningsum_mode(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION8];
	if((( data1 >> STC_COM_BIT_FPGA_FUNCTION3_H_BINNING_SUM ) & 1) || (( data2 >> STC_COM_BIT_FPGA_FUNCTION8_V_BINNING_SUM ) & 1)) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_clock(struct stcam_st_dev_data *ddata)
{
	if(cam_model_table[ddata->dev_ctrl->product].category == e_camera_category_type_0) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_v_blank(struct stcam_st_dev_data *ddata)
{
	if(cam_model_table[ddata->dev_ctrl->product].category > e_camera_category_type_0) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_sensor_shutter_mode(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	if((( data1 >> STC_COM_BIT_FPGA_FUNCTION1_CMOS_RESET_TYPE_0 ) & 1) || (( data2 >> STC_COM_BIT_FPGA_FUNCTION4_CMOS_RESET_TYPE_1 ) & 5)) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_alc(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if((( data >> STC_COM_BIT_FPGA_FUNCTION1_AGC ) & 1) || (( data >> STC_COM_BIT_FPGA_FUNCTION1_AE ) & 1)) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_agc(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_AGC ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_ae(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_AE ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	return stcam_dd_cam_is_enable_trigger_overlap(ddata);
}

bool stcam_dd_cam_is_enable_frame_burst_start_trigger(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_FRAME_BURST_START ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	if(stcam_dd_cam_is_enable_frame_burst_start_trigger(ddata)) {
		return stcam_dd_cam_is_enable_trigger_overlap(ddata);
	}
	return false;
}

bool stcam_dd_cam_is_enable_exposure_trigger(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION0];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION0_STARTSTOP ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_exposure_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	if(stcam_dd_cam_is_enable_exposure_trigger(ddata)) {
		return stcam_dd_cam_is_enable_trigger_overlap(ddata);
	}
	return false;
}

bool stcam_dd_cam_is_enable_readout(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_DISABLED_READOUT ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_readout_overlap(struct stcam_st_dev_data *ddata)
{
	if(stcam_dd_cam_is_enable_readout(ddata)) {
		return stcam_dd_cam_is_enable_trigger_overlap(ddata);
	}
	return false;
}

bool stcam_dd_cam_is_enable_line_debounce_time(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_LINE_DEBOUNCE_TIME ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_io0(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION6];
	if(( data1 >> STC_COM_BIT_FPGA_FUNCTION4_IO_EXISTENCE_FLAG ) & 1) {
		if(( data2 >> STC_COM_BIT_FPGA_FUNCTION6_IO0_EXISTENCE ) & 1) {
			return true;
		}
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_io1(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION6];
	if(( data1 >> STC_COM_BIT_FPGA_FUNCTION4_IO_EXISTENCE_FLAG ) & 1) {
		if(( data2 >> STC_COM_BIT_FPGA_FUNCTION6_IO1_EXISTENCE ) & 1) {
			return true;
		}
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_io2(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION6];
	if(( data1 >> STC_COM_BIT_FPGA_FUNCTION4_IO_EXISTENCE_FLAG ) & 1) {
		if(( data2 >> STC_COM_BIT_FPGA_FUNCTION6_IO2_EXISTENCE ) & 1) {
			return true;
		}
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_io3(struct stcam_st_dev_data *ddata)
{
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION6];
	if(( data1 >> STC_COM_BIT_FPGA_FUNCTION4_IO_EXISTENCE_FLAG ) & 1) {
		if(( data2 >> STC_COM_BIT_FPGA_FUNCTION6_IO3_EXISTENCE ) & 1) {
			return true;
		}
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_led_status(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION0];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION0_LED ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_reset_switch(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION4_IO_RESET_SW_DISABLED ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_sw_status(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION0];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION0_DISABLE_DIP_SW ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_white_balance(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_AWB ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_gamma(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_CAMERA_GAMMA ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_defect_pixel_correction(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_DEFECT_PIXEL_CORRECTION ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_digital_clamp(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_DIGITAL_CLAMP ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_analog_black_level(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_5];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_5_ANALOG_BLACK_LEVEL ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_adjustment_mode(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_ADJUSTMENT_MODE_DIGITAL_GAIN ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_io_direction_change(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION0];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION0_IO_CHANGE_DIRECTION ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_digital_gain(struct stcam_st_dev_data *ddata) {
	return true;
}

bool stcam_dd_cam_is_enable_trigger_wait_hd(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_DISABLE_EXPOSURE_START_WAIT_HD ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_trigger_wait_readout(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_DISABLE_EXPOSURE_START_WAIT_READ_OUT ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_v_binning_sum(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION8];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION8_V_BINNING_SUM ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_90_fps(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION0];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION0_VGA90FPS ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_genicam_io(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_trigger_width(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION4_DISABLE_PULSE_WIDTH_EXPOSURE ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_photocoupler(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION1_PHOTOCOUPLER ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_valid_out(struct stcam_st_dev_data *ddata) {
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION3_TRIGGER_VALID_OUT ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_transfer_end_out(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_TRANSFER_END_OUT ) & 1) {
		return true;
	}
	return false;
}

bool stcam_dd_cam_is_enable_exposure_end_trigger_source(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_EXPOSURE_END_TRIGGER_SOURCE ) & 1) {
		return false;
	}
	return true;
}

bool stcam_dd_cam_is_enable_trigger_overlap(struct stcam_st_dev_data *ddata) {
	bool ret = false;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_0:
		{
			__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
			if(( data >> STC_COM_BIT_FPGA_FUNCTION1_TRIGGER_MASK ) & 1) {
				ret =  true;
			}
		}
		break;
	case e_camera_category_type_1:
	case e_camera_category_type_2:
		ret = false;
		break;
	case e_camera_category_type_3:
	case e_camera_category_type_4:
	case e_camera_category_type_5:
	case e_camera_category_type_6:
	case e_camera_category_type_7:
		{
			__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_c];
			if(( data >> STC_COM_BIT_FPGA2_FEATURE_C_TRIGGER_OVERLAP ) & 1) {
				return true;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

bool stcam_dd_cam_is_enable_trigger_overlap_off(struct stcam_st_dev_data *ddata) {
	bool ret = false;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_0:
		ret =  true;
		break;
	case e_camera_category_type_1:
	case e_camera_category_type_2:
	case e_camera_category_type_3:
	case e_camera_category_type_4:
	case e_camera_category_type_5:
	case e_camera_category_type_6:
	case e_camera_category_type_7:
		{
			__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_c];
			if(( data >> STC_COM_BIT_FPGA2_FEATURE_C_TRIGGER_OVERLAP_OFF ) & 1) {
				return true;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

bool stcam_dd_cam_is_enable_trigger_overlap_read_out(struct stcam_st_dev_data *ddata) {
	bool ret = false;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_0:
		ret =  false;
		break;
	case e_camera_category_type_1:
	case e_camera_category_type_2:
	case e_camera_category_type_3:
	case e_camera_category_type_4:
	case e_camera_category_type_5:
	case e_camera_category_type_6:
	case e_camera_category_type_7:
		{
			__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_c];
			if(( data >> STC_COM_BIT_FPGA2_FEATURE_C_TRIGGER_OVERLAP_READ_OUT ) & 1) {
				return true;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

bool stcam_dd_cam_is_enable_trigger_overlap_previous_frame(struct stcam_st_dev_data *ddata) {
	bool ret = false;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_0:
		ret =  true;
		break;
	case e_camera_category_type_1:
	case e_camera_category_type_2:
	case e_camera_category_type_3:
	case e_camera_category_type_4:
	case e_camera_category_type_5:
	case e_camera_category_type_6:
	case e_camera_category_type_7:
		{
			__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_c];
			if(( data >> STC_COM_BIT_FPGA2_FEATURE_C_TRIGGER_OVERLAP_PREVIOUS_FRAME ) & 1) {
				return true;
			}
		}
		break;
	default:
		break;
	}
	return ret;
}

__u32 stcam_dd_cam_bitmask_scan_mode(struct stcam_st_dev_data *ddata) {
	__u16 val;
	val = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_ENABLE_SCAN_MODE);
	return val;
}

__u32 stcam_dd_cam_bitmask_v_decimation_binning(struct stcam_st_dev_data *ddata) {
	__u32 val = STC_COM_BITMASK_DECIMATION_BINNING_NONE;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_1:
		val = STC_COM_BITMASK_DECIMATION_BINNING_0_0
			| STC_COM_BITMASK_DECIMATION_BINNING_1_0
			| STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_0
			| STC_COM_BITMASK_DECIMATION_BINNING_3_0
			| STC_COM_BITMASK_DECIMATION_BINNING_3_1
			| STC_COM_BITMASK_DECIMATION_BINNING_3_3
			| STC_COM_BITMASK_DECIMATION_BINNING_4_0
			| STC_COM_BITMASK_DECIMATION_BINNING_5_0
			| STC_COM_BITMASK_DECIMATION_BINNING_5_1
			| STC_COM_BITMASK_DECIMATION_BINNING_6_0
			| STC_COM_BITMASK_DECIMATION_BINNING_7_0
			| STC_COM_BITMASK_DECIMATION_BINNING_7_1
			| STC_COM_BITMASK_DECIMATION_BINNING_7_3;
		break;
	case e_camera_category_type_2:
		val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_1_2
			| STC_COM_BITMASK_DECIMATION_BINNING_2_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_2
			| STC_COM_BITMASK_DECIMATION_BINNING_4_1
			| STC_COM_BITMASK_DECIMATION_BINNING_4_2;
		break;
	case e_camera_category_type_3:
		if(stcam_dd_cam_g_color_array(ddata) == STC_MENU_COLOR_ARRAY_MONO) {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_1_2
				| STC_COM_BITMASK_DECIMATION_BINNING_1_4
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1
				| STC_COM_BITMASK_DECIMATION_BINNING_4_1;
		} else {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1
				| STC_COM_BITMASK_DECIMATION_BINNING_4_1;
		}
		break;
	case e_camera_category_type_4:
	case e_camera_category_type_5:
		if(stcam_dd_cam_g_color_array(ddata) == STC_MENU_COLOR_ARRAY_MONO) {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_1_2
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		} else {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		}
		break;
	case e_camera_category_type_7:
		val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		break;
	case e_camera_category_type_0:
	case e_camera_category_type_6:
	default:
		break;
	}
	return val;
}

__u32 stcam_dd_cam_bitmask_h_decimation_binning(struct stcam_st_dev_data *ddata) {
	__u32 val = STC_COM_BITMASK_DECIMATION_BINNING_NONE;
	switch(cam_model_table[ddata->dev_ctrl->product].category) {
	case e_camera_category_type_1:
		val = STC_COM_BITMASK_DECIMATION_BINNING_0_0
			| STC_COM_BITMASK_DECIMATION_BINNING_1_0
			| STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_0
			| STC_COM_BITMASK_DECIMATION_BINNING_3_0
			| STC_COM_BITMASK_DECIMATION_BINNING_3_1
			| STC_COM_BITMASK_DECIMATION_BINNING_3_3
			| STC_COM_BITMASK_DECIMATION_BINNING_4_0
			| STC_COM_BITMASK_DECIMATION_BINNING_5_0
			| STC_COM_BITMASK_DECIMATION_BINNING_5_1
			| STC_COM_BITMASK_DECIMATION_BINNING_6_0;
		break;
	case e_camera_category_type_2:
		val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_1_2
			| STC_COM_BITMASK_DECIMATION_BINNING_2_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_2
			| STC_COM_BITMASK_DECIMATION_BINNING_4_1
			| STC_COM_BITMASK_DECIMATION_BINNING_4_2;
		break;
	case e_camera_category_type_3:
		if(stcam_dd_cam_g_color_array(ddata) == STC_MENU_COLOR_ARRAY_MONO) {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_1_2
				| STC_COM_BITMASK_DECIMATION_BINNING_1_4
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1
				| STC_COM_BITMASK_DECIMATION_BINNING_4_1;
		} else {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1;
		}
		break;
	case e_camera_category_type_4:
	case e_camera_category_type_5:
		if(stcam_dd_cam_g_color_array(ddata) == STC_MENU_COLOR_ARRAY_MONO) {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_1_2
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		} else {
			val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
				| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		}
		break;
	case e_camera_category_type_7:
		val = STC_COM_BITMASK_DECIMATION_BINNING_1_1
			| STC_COM_BITMASK_DECIMATION_BINNING_2_1;
		break;
	case e_camera_category_type_0:
	case e_camera_category_type_6:
	default:
		break;
	}
	return val;
}

__u32 stcam_dd_cam_bitmask_sensor_shutter_mode(struct stcam_st_dev_data *ddata) {
	__u16 val;
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	val = (( data1 >> STC_COM_BIT_FPGA_FUNCTION1_CMOS_RESET_TYPE_0 ) & 1) | ((( data2 >> STC_COM_BIT_FPGA_FUNCTION4_CMOS_RESET_TYPE_1 ) & 5) << 1);
	return val;
}

__u32 stcam_dd_cam_bitmask_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = MACRO_GET_CAM_VALUE_32(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_ENABLE_TRANSFER_BPP) & 0x0000ffff;
	return val;
}

__u32 stcam_dd_cam_max_gain(struct stcam_st_dev_data *ddata)
{
	__u16 val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_GAIN);
	return val;
}

__u32 stcam_dd_cam_max_exposure(struct stcam_st_dev_data *ddata)
{
	__u16 val;
	__u32 val1, val2, val3;
	
	if(cam_model_table[ddata->dev_ctrl->product].category == e_camera_category_type_0) {
		val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_SCAN_MODE);
		switch(val) {
		case STC_COM_VALUE_SCAN_MODE_PARTIAL_1:
		case STC_COM_VALUE_SCAN_MODE_PARTIAL_2:
		case STC_COM_VALUE_SCAN_MODE_PARTIAL_4:
		case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_1:
		case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_2:
		case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_4:
			val1 = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK) - 1;
			val2 = 4090;
			val3 = 0;
			break;
		default:
			val1 = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK) - 1;
			val2 = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_V_LINE);
			val3 = 0;
			break;
		}
	} else {
		val1 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_EXP_FINE_ADJUSTMENT_MAX_VALUE);
		val2 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_LONG_EXP_COARSE_ADJUSTMENT);
		val3 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_LONG_EXP_FINE_ADJUSTMENT);
	}
	return (((val1 + 1) * val2) + val3);
}

__u32 stcam_dd_cam_max_v_blank(struct stcam_st_dev_data *ddata)
{
	__u16 data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION8];
	if(( data >> STC_COM_BIT_FPGA_FUNCTION8_MAX_V_BINNING_FOR_FPS ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_max_v_blanking_for_fps];
	}
	return 2035;
}

__u32 stcam_dd_cam_max_digital_gain(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_GET_DIGITAL_GAIN_RANGE ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_max_digital_gain];
	}
	return 511;
}

__u32 stcam_dd_cam_max_delay(struct stcam_st_dev_data *ddata)
{
	if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_1) {
		return 262143;
	}
	return 4094;
}
__u32 stcam_dd_cam_max_analog_black_level(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga2[e_fpga2_sub_max_analog_black_level];
}

__u32 stcam_dd_cam_min_digital_gain(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_GET_DIGITAL_GAIN_RANGE ) & 1) {
		return 0;
	}
	return 64;
}

//get parameta
__u32 stcam_dd_cam_g_gain(struct stcam_st_dev_data *ddata)
{
	__u16 val;
	read_shutter_gain(ddata);
	val = MACRO_GET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_GAIN);
	return val;
}

__u32 stcam_dd_cam_g_exposure(struct stcam_st_dev_data *ddata)
{
	__u16 val_l, val_h;
	__u32 line_count, clock_count, div;
	read_shutter_gain(ddata);
	if(cam_model_table[ddata->dev_ctrl->product].category == e_camera_category_type_0) {
		div = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK);
	} else {
		div = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_EXP_FINE_ADJUSTMENT_MAX_VALUE);
		div += 1;
	}
	clock_count = MACRO_GET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_CLOCK_L);
	val_l = MACRO_GET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_LINE_L);
	val_h = MACRO_GET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_LINE_H);
	line_count = (val_l | ((val_h << 16) & 0xffff0000));
	
	return (line_count * div) + clock_count;
}

__u32 stcam_dd_cam_g_hflip(struct stcam_st_dev_data *ddata)
{
	__u16 val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE];
	if(val & STC_COM_BITMASK_FPGA_H_FLIP) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_vflip(struct stcam_st_dev_data *ddata)
{
	__u16 val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE];
	if(val & STC_COM_BITMASK_FPGA_V_FLIP) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_color_array(struct stcam_st_dev_data *ddata)
{
	__u16 val, data;
	val = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_COLOR_ARRAY);
	data = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION4];
	if((val == STC_MENU_COLOR_ARRAY_MONO) || (( data >> STC_COM_BIT_FPGA_FUNCTION4_COLOR_ARRAY_DOESNOT_DEPEND_ON_FLIPING ) & 1)) {
		return val;
	}
	if(1 == stcam_dd_cam_g_hflip(ddata)) {
		if(1 == (val % 2)) {
			val -= 1;
		} else {
			val += 1;
		}
	}
	if(1 == stcam_dd_cam_g_vflip(ddata)) {
		if(1 == (val / 4)) {
			val -= 2;
		} else {
			val += 2;
		}
	}

	return val;
}

__u32 stcam_dd_cam_g_temperature(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga2[e_fpga2_sub_camera_temperature];
}

__u32 stcam_dd_cam_g_scan_mode(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	__u16 val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_SCAN_MODE);
	switch(val) {
	case STC_COM_VALUE_SCAN_MODE_NORMAL:
		ret = STC_MENU_SCAN_MODE_NORMAL;
		break;
	case STC_COM_VALUE_SCAN_MODE_PARTIAL_1:
		ret = STC_MENU_SCAN_MODE_PARTIAL_1;
		break;
	case STC_COM_VALUE_SCAN_MODE_PARTIAL_2:
		ret = STC_MENU_SCAN_MODE_PARTIAL_2;
		break;
	case STC_COM_VALUE_SCAN_MODE_PARTIAL_4:
		ret = STC_MENU_SCAN_MODE_PARTIAL_4;
		break;
	case STC_COM_VALUE_SCAN_MODE_VARIABLE_PARTIAL:
		ret = STC_MENU_SCAN_MODE_VARIABLE_PARTIAL;
		break;
	case STC_COM_VALUE_SCAN_MODE_BINNING:
		ret = STC_MENU_SCAN_MODE_BINNING;
		break;
	case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_1:
		ret = STC_MENU_SCAN_MODE_BINNING_PARTIAL_1;
		break;
	case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_2:
		ret = STC_MENU_SCAN_MODE_BINNING_PARTIAL_2;
		break;
	case STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_4:
		ret = STC_MENU_SCAN_MODE_BINNING_PARTIAL_4;
		break;
	case STC_COM_VALUE_SCAN_MODE_BINNING_VARIABLE_PARTIAL:
		ret = STC_MENU_SCAN_MODE_BINNING_VARIABLE_PARTIAL;
		break;
	case STC_COM_VALUE_SCAN_MODE_ROI:
		ret = STC_MENU_SCAN_MODE_ROI;
		break;
	default:
		break;
	}
	
	return ret;
}

__u32 stcam_dd_cam_g_max_roi_count(struct stcam_st_dev_data *ddata)
{
	__u32 val = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_ROI_COUNT);
	if(val == 1) {
		val = 0;
	}
	return val;
}

__u32 stcam_dd_cam_g_v_decimation_binning(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	__u16 val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] & 0x00ff;
	switch(val) {
	case STC_COM_VALUE_DECIMATION_BINNING_0_0:
		ret = STC_MENU_DECIMATION_BINNING_0_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_0:
		ret = STC_MENU_DECIMATION_BINNING_1_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_1:
		ret = STC_MENU_DECIMATION_BINNING_1_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_2:
		ret = STC_MENU_DECIMATION_BINNING_1_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_4:
		ret = STC_MENU_DECIMATION_BINNING_1_4;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_0:
		ret = STC_MENU_DECIMATION_BINNING_2_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_1:
		ret = STC_MENU_DECIMATION_BINNING_2_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_2:
		ret = STC_MENU_DECIMATION_BINNING_2_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_0:
		ret = STC_MENU_DECIMATION_BINNING_3_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_1:
		ret = STC_MENU_DECIMATION_BINNING_3_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_3:
		ret = STC_MENU_DECIMATION_BINNING_3_3;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_0:
		ret = STC_MENU_DECIMATION_BINNING_4_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_1:
		ret = STC_MENU_DECIMATION_BINNING_4_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_2:
		ret = STC_MENU_DECIMATION_BINNING_4_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_5_0:
		ret = STC_MENU_DECIMATION_BINNING_5_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_5_1:
		ret = STC_MENU_DECIMATION_BINNING_5_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_6_0:
		ret = STC_MENU_DECIMATION_BINNING_6_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_0:
		ret = STC_MENU_DECIMATION_BINNING_7_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_1:
		ret = STC_MENU_DECIMATION_BINNING_7_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_3:
		ret = STC_MENU_DECIMATION_BINNING_7_3;
		break;
	default:
		break;
	}
	
	return ret;
}

__u32 stcam_dd_cam_g_h_decimation_binning(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	__u16 val = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] >> 8) & 0x00ff;
	switch(val) {
	case STC_COM_VALUE_DECIMATION_BINNING_0_0:
		ret = STC_MENU_DECIMATION_BINNING_0_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_0:
		ret = STC_MENU_DECIMATION_BINNING_1_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_1:
		ret = STC_MENU_DECIMATION_BINNING_1_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_2:
		ret = STC_MENU_DECIMATION_BINNING_1_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_1_4:
		ret = STC_MENU_DECIMATION_BINNING_1_4;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_0:
		ret = STC_MENU_DECIMATION_BINNING_2_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_1:
		ret = STC_MENU_DECIMATION_BINNING_2_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_2_2:
		ret = STC_MENU_DECIMATION_BINNING_2_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_0:
		ret = STC_MENU_DECIMATION_BINNING_3_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_1:
		ret = STC_MENU_DECIMATION_BINNING_3_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_3_3:
		ret = STC_MENU_DECIMATION_BINNING_3_3;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_0:
		ret = STC_MENU_DECIMATION_BINNING_4_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_1:
		ret = STC_MENU_DECIMATION_BINNING_4_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_4_2:
		ret = STC_MENU_DECIMATION_BINNING_4_2;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_5_0:
		ret = STC_MENU_DECIMATION_BINNING_5_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_5_1:
		ret = STC_MENU_DECIMATION_BINNING_5_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_6_0:
		ret = STC_MENU_DECIMATION_BINNING_6_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_0:
		ret = STC_MENU_DECIMATION_BINNING_7_0;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_1:
		ret = STC_MENU_DECIMATION_BINNING_7_1;
		break;
	case STC_COM_VALUE_DECIMATION_BINNING_7_3:
		ret = STC_MENU_DECIMATION_BINNING_7_3;
		break;
	default:
		break;
	}
	
	return ret;
}

__u32 stcam_dd_cam_g_binningsum_mode(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	__u16 val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_SUM];
	switch(val) {
	case STC_COM_VALUE_BINNINGSUM_MODE_OFF:
		ret = STC_MENU_BINNINGSUM_MODE_OFF;
		break;
	case STC_COM_VALUE_BINNINGSUM_MODE_H_ON:
		ret = STC_MENU_BINNINGSUM_MODE_H_ON;
		break;
	case STC_COM_VALUE_BINNINGSUM_MODE_V_ON:
		ret = STC_MENU_BINNINGSUM_MODE_V_ON;
		break;
	default:
		break;
	}
	
	return ret;
}

__u32 stcam_dd_cam_g_output_fps(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_2) {
		ret = read_fpga2(ddata, e_fpga2_sub_frame_rate_base);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
		ret = read_fpga2(ddata, e_fpga2_sub_fps_decimal);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
		if(ddata->cam_param_fpga2[e_fpga2_sub_frame_rate_base] != 0) {
			ret = ddata->cam_param_fpga2[e_fpga2_sub_fps_decimal] / ddata->cam_param_fpga2[e_fpga2_sub_frame_rate_base];
		}
	} else if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_1) {
		ret = read_fpga(ddata);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
		ret = ddata->cam_param_fpga[STC_COM_ADR_FPGA_FPS_INTEGER];
 	} else {
		__u32 clock_hz = MACRO_GET_CAM_VALUE_32(cam_param_clock, STC_COM_ADR_CLOCK_SPEED);
		__u16 total_v_line = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_V_LINE);
		__u16 total_h_clock = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK);
 		
 		if((total_v_line != 0) && (total_h_clock != 0)) {
 			ret = clock_hz / (total_v_line * total_h_clock);
 		}
	}
	return ret;
}

__u32 stcam_dd_cam_g_clock(struct stcam_st_dev_data *ddata)
{
	__u32 ret = 0;
	__u32 val = MACRO_GET_CAM_VALUE_32(cam_param_clock, STC_COM_ADR_CLOCK_MODE);
	switch(val) {
	case STC_COM_VALUE_CLOCK_NORMAL:
		ret = STC_MENU_CLOCK_NORMAL;
		break;
	case STC_COM_VALUE_CLOCK_DIV_2:
		ret = STC_MENU_CLOCK_DIV_2;
		break;
	case STC_COM_VALUE_CLOCK_DIV_4:
		ret = STC_MENU_CLOCK_DIV_4;
		break;
	case STC_COM_VALUE_CLOCK_VGA_90FPS:
		ret = STC_MENU_CLOCK_VGA_90FPS;
		break;
	default:
		break;
	}
	
	return ret;
}

__u32 stcam_dd_cam_g_v_blank(struct stcam_st_dev_data *ddata)
{
	if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_4) {
		return ddata->cam_param_fpga2[e_fpga2_sub_fps_by_v_blanking];
	}
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_V_BLNAK_FOR_FPS];
}

__u32 stcam_dd_cam_g_max_short_exposure(struct stcam_st_dev_data *ddata)
{
	__u32 val1, val2, val3;
	
	if(cam_model_table[ddata->dev_ctrl->product].category == e_camera_category_type_0) {
		val1 = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK) - 1;
		val2 = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_V_LINE);
		val3 = 0;
	} else {
		val1 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_EXP_FINE_ADJUSTMENT_MAX_VALUE);
		val2 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_SHORT_EXP_COARSE_ADJUSTMENT);
		val3 = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_MAX_SHORT_EXP_FINE_ADJUSTMENT);
	}
	return (((val1 + 1) * val2) + val3);
}

__u32 stcam_dd_cam_g_digital_gain(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_DIGITAL_GAIN];
}

__u32 stcam_dd_cam_g_exposure_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_mode];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_EXPOSURE_MODE) & 3;
	return (value + 1);
}

__u32 stcam_dd_cam_g_sensor_shutter_mode(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_sensor_shutter_mode];
	}
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_CMOS_RESET) & 3;
}

__u32 stcam_dd_cam_g_alc_mode(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_ALC_MODE];
}

__u32 stcam_dd_cam_g_alc_target_level(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_ALC_TARGET_LEVEL];
}

__u32 stcam_dd_cam_g_agc_min_gain(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_AGC_MIN_GAIN];
}

__u32 stcam_dd_cam_g_agc_max_gain(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_AGC_MAX_GAIN];
}

__u32 stcam_dd_cam_g_ae_min_exposure_clock(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MIN_EXP_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MIN_EXP_H] << 16) & 0xffff0000);
}

__u32 stcam_dd_cam_g_ae_max_exposure_clock(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MAX_EXP_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MAX_EXP_H] << 16) & 0xffff0000);
}

__u32 stcam_dd_cam_g_frame_start_trigger_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_mode];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & 3;
	if(value == 1) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_frame_start_trigger_source(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_source];
	}
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & 3;
}

__u32 stcam_dd_cam_g_frame_start_trigger_delay(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_delay];
	}
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_overlap];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME;
	}
	return STC_MENU_TRIGGER_OVERLAP_OFF;
}

__u32 stcam_dd_cam_g_frame_burst_start_trigger_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_mode];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & 3;
	if(value == 1) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_frame_burst_start_trigger_source(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_source];
	}
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & 3;
}

__u32 stcam_dd_cam_g_frame_burst_start_trigger_delay(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_delay];
	}
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_overlap];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME;
	}
	return STC_MENU_TRIGGER_OVERLAP_OFF;
}

__u32 stcam_dd_cam_g_exposure_start_trigger_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_mode];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & 3;
	if(value == 1) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_exposure_start_trigger_source(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_source];
	}
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & 3;
}

__u32 stcam_dd_cam_g_exposure_start_trigger_delay(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_delay];
	}
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_exposure_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_overlap];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME;
	}
	return STC_MENU_TRIGGER_OVERLAP_OFF;
}

__u32 stcam_dd_cam_g_exposure_end_trigger_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_mode];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & 3;
	if(value == 1) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_exposure_end_trigger_source(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_source];
	}
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & 3;
}

__u32 stcam_dd_cam_g_exposure_end_trigger_delay(struct stcam_st_dev_data *ddata)
{
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_delay];
	}
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_exposure_end_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_overlap];
	}
	value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME;
	}
	return STC_MENU_TRIGGER_OVERLAP_OFF;
}

__u32 stcam_dd_cam_g_read_out_start_trigger_mode(struct stcam_st_dev_data *ddata)
{
	__u16 value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & 3;
	if(value == 2) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_read_out_start_trigger_source(struct stcam_st_dev_data *ddata)
{
	__u16 value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_READOUT_SELECT) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_SOURCE_SOFTWARE;
	}
	return STC_MENU_TRIGGER_SOURCE_HARDWARE;
}

__u32 stcam_dd_cam_g_read_out_start_trigger_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_read_out_start_trigger_overlap(struct stcam_st_dev_data *ddata)
{
	__u16 value = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] >> STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & 1;
	if(value == 0) {
		return STC_MENU_TRIGGER_OVERLAP_PREVIOUS_FRAME;
	}
	return STC_MENU_TRIGGER_OVERLAP_OFF;
}

__u32 stcam_dd_cam_g_exposure_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_strobe_start_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_strobe_end_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_END_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_END_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_output_pulse_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_DELAY_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_DELAY_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_output_pulse_duration(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_PULSE_WIDTH_L] | ((ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_PULSE_WIDTH_H] << 16) & 0x00030000);
}

__u32 stcam_dd_cam_g_readout_delay(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_READOUT_DELAY];
}

__u32 stcam_dd_cam_g_line_debounce_time(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga2[e_fpga2_sub_line_debounce_time];
}

__u32 stcam_dd_cam_g_io0_direction(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO0) & 1;
}

__u32 stcam_dd_cam_g_io0_polarity(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] >> STC_COM_BITSHIFT_FPGA_IO0) & 1;
}

__u32 stcam_dd_cam_g_io0_mode(struct stcam_st_dev_data *ddata)
{
	__u32 val, ret;
	read_fpga(ddata);
	val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_0];
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO0) & 1) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(val) {
		case STC_COM_VALUE_OUT_GENERAL_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_COM_VALUE_OUT_EXPOSURE_END:
			ret = STC_MENU_IO_MODE_OUT_EXPOSURE_END;
			break;
		case STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_VALID_OUT:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_COM_VALUE_OUT_TRANSFER_END:
			ret = STC_MENU_IO_MODE_OUT_TRANSFER_END;
			break;
		case STC_COM_VALUE_OUT_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_OUT_DISABLE;
			break;
		}
	} else {
		switch(val) {
		case STC_COM_VALUE_IN_GENERAL_INPUT:
			ret = STC_MENU_IO_MODE_IN_GENERAL_INPUT;
			break;
		case STC_COM_VALUE_IN_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_READOUT_INPUT:
			ret = STC_MENU_IO_MODE_IN_READOUT_INPUT;
			break;
		case STC_COM_VALUE_IN_SUB_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_IN_DISABLE;
			break;
		}
	}
	return ret;
}

__u32 stcam_dd_cam_g_io0_status(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] >> STC_COM_BITSHIFT_FPGA_IO0) & 1;
}

__u32 stcam_dd_cam_g_io1_direction(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO1) & 1;
}

__u32 stcam_dd_cam_g_io1_polarity(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] >> STC_COM_BITSHIFT_FPGA_IO1) & 1;
}

__u32 stcam_dd_cam_g_io1_mode(struct stcam_st_dev_data *ddata)
{
	__u32 val, ret;
	read_fpga(ddata);
	val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_1];
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO1) & 1) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(val) {
		case STC_COM_VALUE_OUT_GENERAL_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_COM_VALUE_OUT_EXPOSURE_END:
			ret = STC_MENU_IO_MODE_OUT_EXPOSURE_END;
			break;
		case STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_VALID_OUT:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_COM_VALUE_OUT_TRANSFER_END:
			ret = STC_MENU_IO_MODE_OUT_TRANSFER_END;
			break;
		case STC_COM_VALUE_OUT_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_OUT_DISABLE;
			break;
		}
	} else {
		switch(val) {
		case STC_COM_VALUE_IN_GENERAL_INPUT:
			ret = STC_MENU_IO_MODE_IN_GENERAL_INPUT;
			break;
		case STC_COM_VALUE_IN_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_READOUT_INPUT:
			ret = STC_MENU_IO_MODE_IN_READOUT_INPUT;
			break;
		case STC_COM_VALUE_IN_SUB_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_IN_DISABLE;
			break;
		}
	}
	return ret;
}

__u32 stcam_dd_cam_g_io1_status(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] >> STC_COM_BITSHIFT_FPGA_IO1) & 1;
}

__u32 stcam_dd_cam_g_io2_direction(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO2) & 1;
}

__u32 stcam_dd_cam_g_io2_polarity(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] >> STC_COM_BITSHIFT_FPGA_IO2) & 1;
}

__u32 stcam_dd_cam_g_io2_mode(struct stcam_st_dev_data *ddata)
{
	__u32 val, ret;
	read_fpga(ddata);
	val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_2];
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO2) & 1) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(val) {
		case STC_COM_VALUE_OUT_GENERAL_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_COM_VALUE_OUT_EXPOSURE_END:
			ret = STC_MENU_IO_MODE_OUT_EXPOSURE_END;
			break;
		case STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_VALID_OUT:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_COM_VALUE_OUT_TRANSFER_END:
			ret = STC_MENU_IO_MODE_OUT_TRANSFER_END;
			break;
		case STC_COM_VALUE_OUT_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_OUT_DISABLE;
			break;
		}
	} else {
		switch(val) {
		case STC_COM_VALUE_IN_GENERAL_INPUT:
			ret = STC_MENU_IO_MODE_IN_GENERAL_INPUT;
			break;
		case STC_COM_VALUE_IN_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_READOUT_INPUT:
			ret = STC_MENU_IO_MODE_IN_READOUT_INPUT;
			break;
		case STC_COM_VALUE_IN_SUB_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_IN_DISABLE;
			break;
		}
	}
	return ret;
}

__u32 stcam_dd_cam_g_io2_status(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] >> STC_COM_BITSHIFT_FPGA_IO2) & 1;
}

__u32 stcam_dd_cam_g_io3_direction(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO3) & 1;
}

__u32 stcam_dd_cam_g_io3_polarity(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] >> STC_COM_BITSHIFT_FPGA_IO3) & 1;
}

__u32 stcam_dd_cam_g_io3_mode(struct stcam_st_dev_data *ddata)
{
	__u32 val, ret;
	read_fpga(ddata);
	val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_3];
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] >> STC_COM_BITSHIFT_FPGA_IO3) & 1) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(val) {
		case STC_COM_VALUE_OUT_GENERAL_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_COM_VALUE_OUT_EXPOSURE_END:
			ret = STC_MENU_IO_MODE_OUT_EXPOSURE_END;
			break;
		case STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT:
			ret = STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE:
			ret = STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_COM_VALUE_OUT_TRIGGER_VALID_OUT:
			ret = STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_COM_VALUE_OUT_TRANSFER_END:
			ret = STC_MENU_IO_MODE_OUT_TRANSFER_END;
			break;
		case STC_COM_VALUE_OUT_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_OUT_DISABLE;
			break;
		}
	} else {
		switch(val) {
		case STC_COM_VALUE_IN_GENERAL_INPUT:
			ret = STC_MENU_IO_MODE_IN_GENERAL_INPUT;
			break;
		case STC_COM_VALUE_IN_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_READOUT_INPUT:
			ret = STC_MENU_IO_MODE_IN_READOUT_INPUT;
			break;
		case STC_COM_VALUE_IN_SUB_TRIGGER_INPUT:
			ret = STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_COM_VALUE_IN_DISABLE:
		default:
			ret = STC_MENU_IO_MODE_IN_DISABLE;
			break;
		}
	}
	return ret;
}

__u32 stcam_dd_cam_g_io3_status(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] >> STC_COM_BITSHIFT_FPGA_IO3) & 1;
}

__u32 stcam_dd_cam_g_led_status(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_LED];
}

__u32 stcam_dd_cam_g_reset_switch(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_RESET_SW_ENABLED];
}

__u32 stcam_dd_cam_g_sw_status(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_SW_STATUS];
}

__u32 stcam_dd_cam_g_white_balance_mode(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_MODE];
}

__u32 stcam_dd_cam_g_white_balance_gain_r(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_R];
}

__u32 stcam_dd_cam_g_white_balance_gain_gr(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_GR];
}

__u32 stcam_dd_cam_g_white_balance_gain_gb(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_GB];
}

__u32 stcam_dd_cam_g_white_balance_gain_b(struct stcam_st_dev_data *ddata)
{
	read_fpga(ddata);
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_B];
}

__u32 stcam_dd_cam_g_camera_gamma(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_GAMMA];
}

__u32 stcam_dd_cam_g_defect_pixel_correction_count(struct stcam_st_dev_data *ddata)
{
	__u16 val = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION5];
	if((0 == val) && (1 == stcam_dd_cam_is_enable_defect_pixel_correction(ddata))) {
		return 64;
	}
	return val;
}

__u32 stcam_dd_cam_g_defect_pixel_correction_mode(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_DEFFECT_PIXEL_CORRECTION_MODE];
}

__u32 stcam_dd_cam_g_digital_clamp(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga[STC_COM_ADR_FPGA_DIGITAL_CLAMP];
}

__u32 stcam_dd_cam_g_analog_black_level(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga2[e_fpga2_sub_analog_offset];
}

__u32 stcam_dd_cam_g_adjustment_mode(struct stcam_st_dev_data *ddata)
{
	return ddata->cam_param_fpga2[e_fpga2_sub_digital_gain_enabled];
}

int stcam_dd_cam_g_camera_name(struct stcam_st_dev_data *ddata, __u8 *param) {
	strncpy(param, cam_model_table[ddata->dev_ctrl->product].name, STC_IOC_PARAM_LEN_PRODUCT_NAME);
	return 0;
}

int stcam_dd_cam_g_base_size(struct stcam_st_dev_data *ddata, __u16 *paramw, __u16 *paraml) {
	*paramw = cam_model_table[ddata->dev_ctrl->product].width;
	*paraml = cam_model_table[ddata->dev_ctrl->product].height;
	return 0;
}

int stcam_dd_cam_g_base_fps(struct stcam_st_dev_data *ddata, __u16 *param) {
	*param = cam_model_table[ddata->dev_ctrl->product].fps;
	return 0;
}

int stcam_dd_cam_g_total_scan_size(struct stcam_st_dev_data *ddata, __u16 *paramw, __u16 *paraml) {
	*paramw = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_WIDTH);
	*paraml = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_HEIGHT);
	return 0;
}

int stcam_dd_cam_g_cam_version(struct stcam_st_dev_data *ddata, struct stcam_st_cam_version *param) {
	param->vendor_id = VENDOR_ID;
	param->product_id = cam_model_table[ddata->dev_ctrl->product].product_id;
	param->camera_type = cam_model_table[ddata->dev_ctrl->product].camera_type;
	strncpy(param->product_name, cam_model_table[ddata->dev_ctrl->product].name, STC_IOC_PARAM_LEN_PRODUCT_NAME);
	param->fpga_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FPGA_VERSION);
	param->firm_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FIRM_VERSION);
	return 0;
}

int stcam_dd_cam_g_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *param) {
	int ret = 0;
	__u8 *data = kzalloc(param->length, GFP_KERNEL);
	__u16 firm_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FIRM_VERSION);
	if(firm_version >= 0x0100) {
		ret = read_eeprom_info(ddata, (STC_COM_ADR_EEPROM_INFO_USER_DATA_START + param->offset), param->length, data);
	} else {
		ret = read_eeprom_w(ddata, (STC_COM_ADR_EEPROM_W_USER_DATA_START + param->offset), param->length, data);
	}
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
	}
	memcpy(param->data, data, param->length);
	kfree(data);
	return ret;
}

int stcam_dd_cam_g_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *param) {
	int ret = 0;
	__u8 *data = kzalloc(256, GFP_KERNEL);
	__u16 firm_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FIRM_VERSION);
	if(firm_version >= 0x0100) {
		ret = read_eeprom_info(ddata, STC_COM_ADR_EEPROM_INFO_CAMERA_ID_START, 256, data);
		if(ret < 0) {
			kfree(data);
			stcam_dd_util_error_log(ret);
			return ret;
		}
		memcpy((__u8*)&param->camera_id, data, 4);
		param->length = data[5];
		memcpy(param->camera_name, &data[6], 250);
		kfree(data);
		return ret;
	}
	ret = read_eeprom_w(ddata, STC_COM_ADR_EEPROM_W_CAMERA_ID_START, 256, data);
	if(ret < 0) {
		kfree(data);
		stcam_dd_util_error_log(ret);
		return ret;
	}
	memcpy((__u8*)&param->camera_id, data, 4);
	param->length = data[5];
	memcpy(param->camera_name, &data[6], 250);
	kfree(data);
	return ret;
}

int stcam_dd_cam_g_max_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_max_scan_size *param) {
	param->width = cam_model_table[ddata->dev_ctrl->product].width;
	param->height = cam_model_table[ddata->dev_ctrl->product].height;
	return 0;
}

int stcam_dd_cam_g_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *param) {
	__u32 roi_count = stcam_dd_cam_g_max_roi_count(ddata);
	if((roi_count == 0) || (param->roi.index == 0)) {
		param->roi.offset_x = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_OFFSET_X);
		param->roi.offset_y = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_OFFSET_Y);
		param->roi.width = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_WIDTH);
		param->roi.height = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_HEIGHT);
		param->roi.index = 0;
		param->roi.region_mode = 1;
	} else {
		if(param->roi.index < roi_count) {
			__u16 roi_pos = param->roi.index - 1;
			__u32 region_mode = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_REGION_MODE);
			param->roi.offset_x = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_OFFSET_X + (roi_pos *16)));
			param->roi.offset_y = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_OFFSET_Y + (roi_pos *16)));
			param->roi.width = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_WIDTH + (roi_pos *16)));
			param->roi.height = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_HEIGHT + (roi_pos *16)));
			param->roi.region_mode = (region_mode >> param->roi.index) & 0x0001;
		} else {
			return -EINVAL;
		}
	}
	return 0;
}

int stcam_dd_cam_g_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *param) {
	if(ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_size] != 0) {
		param->cmosis_4m.mode = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_0] >> 0) & 0x0001;
		param->cmosis_4m.slope = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_0] >> 8) & 0x0003;
		param->cmosis_4m.vlow3 = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_1] >> 0) & 0x003F;
		param->cmosis_4m.vlow2 = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_1] >> 8) & 0x003F;
		param->cmosis_4m.knee2 = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_1] >> 16) & 0x00FF;
		param->cmosis_4m.knee1 = (ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_1] >> 24) & 0x00FF;
	} else {
		return -EINVAL;
	}
	return 0;
}

int stcam_dd_cam_g_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *param) {
	int ret = 0;
	__u32 value;
	if(stcam_dd_cam_is_enable_defect_pixel_correction(ddata)) {
		if(param->index < stcam_dd_cam_g_defect_pixel_correction_count(ddata)) {
			ret = read_fpga_pix_correct(ddata, param->index, &value);
			param->pos_x = (value & 0x0000ffff);
			param->pos_y = (value >> 16 & 0x0000ffff);
		} else {
			return -EINVAL;
		}
	} else {
		return -EINVAL;
	}
	return ret;
}

__u32 stcam_dd_cam_g_pid(struct stcam_st_dev_data *ddata) {
	return cam_model_table[ddata->dev_ctrl->product].product_id;
}

__u32 stcam_dd_cam_g_digital_gain_type(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_9];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_9_DIGITAL_GAIN_TYPE ) & 1) {
		return 1;
	}
	return 0;
}

__u32 stcam_dd_cam_g_gain_type(struct stcam_st_dev_data *ddata) {
	__u16 val;
	__u16 data1 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION1];
	__u16 data2 = ddata->cam_param_fpga[STC_COM_ADR_FPGA_CAMERA_FUNCTION3];
	val = (( data1 >> STC_COM_BIT_FPGA_FUNCTION1_CDS_GAIN_TYPE ) & 0x0001) | (( data2 >> STC_COM_BIT_FPGA_FUNCTION3_CDS_GAIN_TYPE2 ) & 0x0003) << 1;
	return val;
}

__u32 stcam_dd_cam_g_initial_gain(struct stcam_st_dev_data *ddata) {
	__u16 val;
	val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_DEFAULT_GAIN);
	return val;
}

__u32 stcam_dd_cam_g_image_width(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_WIDTH);
	return val;
}

__u32 stcam_dd_cam_g_image_height(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_HEIGHT);
	return val;
}

__u32 stcam_dd_cam_g_total_h_clock(struct stcam_st_dev_data *ddata) {
	__u16 val;
	val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK);
	return val;
}

__u32 stcam_dd_cam_g_total_v_line(struct stcam_st_dev_data *ddata) {
	__u16 val;
	val = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_V_LINE);
	return val;
}

__u32 stcam_dd_cam_g_clock_hz(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = MACRO_GET_CAM_VALUE_32(cam_param_clock, STC_COM_ADR_CLOCK_SPEED);
	return val;
}

__u32 stcam_dd_cam_g_raw_image_size(struct stcam_st_dev_data *ddata) {
	return ddata->dev_ctrl->scan_info.image_size;
}

__u32 stcam_dd_cam_g_h_binning(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] &= 0x0f00) >> 16;
	return val;
}

__u32 stcam_dd_cam_g_v_binning(struct stcam_st_dev_data *ddata) {
	__u32 val;
	val = (ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] &= 0x000f);
	return val;
}

__u32 stcam_dd_cam_g_trigger_wait_hd(struct stcam_st_dev_data *ddata) {
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_NEXT_H) & 1;
}

__u32 stcam_dd_cam_g_trigger_wait_readout(struct stcam_st_dev_data *ddata) {
	return (ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] >> STC_COM_BITSHIFT_FPGA_COMPLETE_READOUT) & 1;
}

__u32 stcam_dd_cam_g_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata) {
	__u32 ret = 0;
	__u32 val = MACRO_GET_CAM_VALUE_32(cam_param_pixel_byte, STC_COM_ADR_PIXEL_BYTE_TRANSFER_BITS_PER_PIXEL);
	switch(val) {
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10P:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10P;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12P:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12P;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10P:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10P;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12P:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12P;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08;
		break;
	case STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10:
		ret = STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10;
		break;
	default:
		break;
	}
	return ret;
}

__u32 stcam_dd_cam_g_hdr_type(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_size];
	if(data != 0) {
		return STC_MENU_HDR_TYPE_CMOSIS_4M;
	}
	return STC_MENU_HDR_TYPE_NONE;
}

__u32 stcam_dd_cam_g_exposure_time_base(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_1];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_1_EXPOSURE_TIME_BASE ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_time_base];
	}
	return 0;
}

__u32 stcam_dd_cam_g_exposure_time_offset(struct stcam_st_dev_data *ddata) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_EXPOSURE_TIME_OFFSET ) & 1) {
		return ddata->cam_param_fpga2[e_fpga2_sub_exposure_time_offset];
	}
	return 0;
}

//set parameta
int stcam_dd_cam_s_gain(struct stcam_st_dev_data *ddata, __u32 param) {
	MACRO_SET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_GAIN, param)
	return write_shutter_gain(ddata);
}

int stcam_dd_cam_s_exposure(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val_l, val_h;
	__u32 line_count, clock_count, div;
	read_shutter_gain(ddata);
	if(cam_model_table[ddata->dev_ctrl->product].category == e_camera_category_type_0) {
		div = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_TOTAL_H_CLOCK);
	} else {
		div = MACRO_GET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_EXP_FINE_ADJUSTMENT_MAX_VALUE);
		div += 1;
	}
	line_count =  param / div;
	clock_count =  param % div;
	
	MACRO_SET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_CLOCK_L, clock_count);
	val_l = line_count & 0xffff;
	val_h = line_count >> 16 & 0xffff;
	MACRO_SET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_LINE_L, val_l);
	MACRO_SET_CAM_VALUE_16(cam_param_shutter_gain, STC_COM_ADR_SHUTTER_GAIN_EXP_LINE_H, val_h);
	
	return write_shutter_gain(ddata);
}

int stcam_dd_cam_s_hflip(struct stcam_st_dev_data *ddata, __u32 param) {
	if(param == 0) {
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE] &= ~STC_COM_BITMASK_FPGA_H_FLIP;
	} else {
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE] |= STC_COM_BITMASK_FPGA_H_FLIP;
	}
	write_fpga_s(ddata, STC_COM_ADR_FPGA_MIRROR_MODE, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	return 0;
}

int stcam_dd_cam_s_vflip(struct stcam_st_dev_data *ddata, __u32 param) {
	if(param == 0) {
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE] &= ~STC_COM_BITMASK_FPGA_V_FLIP;
	} else {
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_MIRROR_MODE] |= STC_COM_BITMASK_FPGA_V_FLIP;
	}
	write_fpga_s(ddata, STC_COM_ADR_FPGA_MIRROR_MODE, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	return 0;
}

int stcam_dd_cam_s_reset_counter(struct stcam_st_dev_data *ddata, __u32 param) {
	int ret;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] |= STC_COM_BITMASK_FPGA_COUNTER_RESET;
	ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_REFRESH_RC_RO, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] = 0x0000;
	return ret;
}

int stcam_dd_cam_s_camera_setting(struct stcam_st_dev_data *ddata, __u32 param) {
	int ret;
	__u16 *data = kzalloc(sizeof(__u16), GFP_KERNEL);
	*data = 0;
	switch(param) {
	case STC_MENU_CAMERA_SETTING_INITIALIZE:
		*data = STC_COM_VALUE_CAMERA_SETTING_INITIALIZE;
		break;
	case STC_MENU_CAMERA_SETTING_STANDARD_READ:
		*data = STC_COM_VALUE_CAMERA_SETTING_STANDARD_READ;
		break;
	case STC_MENU_CAMERA_SETTING_STANDARD_WRITE:
		*data = STC_COM_VALUE_CAMERA_SETTING_STANDARD_WRITE;
		break;
	case STC_MENU_CAMERA_SETTING_DEF_PIX_POS_READ:
		*data = STC_COM_VALUE_CAMERA_SETTING_DEF_PIX_POS_READ;
		break;
	case STC_MENU_CAMERA_SETTING_DEF_PIX_POS_WRITE:
		*data = STC_COM_VALUE_CAMERA_SETTING_DEF_PIX_POS_WRITE;
		break;
	default:
		break;
	}
	ret = write_fx3(ddata, data);
	kfree(data);
	if(ret<0){
		return ret;
	}
	stcam_dd_cam_update_camera_info(ddata);
	return ret;
}

int stcam_dd_cam_s_scan_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val =0;
	enum stcam_e_device_status device_status = ddata->dev_ctrl->device_status;
	switch(param) {
	case STC_MENU_SCAN_MODE_NORMAL:
		val = STC_COM_VALUE_SCAN_MODE_NORMAL;
		break;
	case STC_MENU_SCAN_MODE_PARTIAL_1:
		val = STC_COM_VALUE_SCAN_MODE_PARTIAL_1;
		break;
	case STC_MENU_SCAN_MODE_PARTIAL_2:
		val = STC_COM_VALUE_SCAN_MODE_PARTIAL_2;
		break;
	case STC_MENU_SCAN_MODE_PARTIAL_4:
		val = STC_COM_VALUE_SCAN_MODE_PARTIAL_4;
		break;
	case STC_MENU_SCAN_MODE_VARIABLE_PARTIAL:
		val = STC_COM_VALUE_SCAN_MODE_VARIABLE_PARTIAL;
		break;
	case STC_MENU_SCAN_MODE_BINNING:
		val = STC_COM_VALUE_SCAN_MODE_BINNING;
		break;
	case STC_MENU_SCAN_MODE_BINNING_PARTIAL_1:
		val = STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_1;
		break;
	case STC_MENU_SCAN_MODE_BINNING_PARTIAL_2:
		val = STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_2;
		break;
	case STC_MENU_SCAN_MODE_BINNING_PARTIAL_4:
		val = STC_COM_VALUE_SCAN_MODE_BINNING_PARTIAL_4;
		break;
	case STC_MENU_SCAN_MODE_BINNING_VARIABLE_PARTIAL:
		val = STC_COM_VALUE_SCAN_MODE_BINNING_VARIABLE_PARTIAL;
		break;
	case STC_MENU_SCAN_MODE_ROI:
		val = STC_COM_VALUE_SCAN_MODE_ROI;
		break;
	default:
		break;
	}
	MACRO_SET_CAM_VALUE_16(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_SCAN_MODE, val)
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_cancel_bulk(ddata);
	}
	write_picture_size(ddata);
	read_picture_size(ddata, ddata->picture_size_length);
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_send_bulk(ddata);
	}
	ddata->dev_ctrl->device_status = device_status;
	return 0;
}

int stcam_dd_cam_s_v_decimation_binning(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val =0;
	enum stcam_e_device_status device_status = ddata->dev_ctrl->device_status;
	switch(param) {
	case STC_MENU_DECIMATION_BINNING_0_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_0_0;
		break;
	case STC_MENU_DECIMATION_BINNING_1_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_0;
		break;
	case STC_MENU_DECIMATION_BINNING_1_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_1;
		break;
	case STC_MENU_DECIMATION_BINNING_1_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_2;
		break;
	case STC_MENU_DECIMATION_BINNING_1_4:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_4;
		break;
	case STC_MENU_DECIMATION_BINNING_2_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_0;
		break;
	case STC_MENU_DECIMATION_BINNING_2_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_1;
		break;
	case STC_MENU_DECIMATION_BINNING_2_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_2;
		break;
	case STC_MENU_DECIMATION_BINNING_3_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_0;
		break;
	case STC_MENU_DECIMATION_BINNING_3_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_1;
		break;
	case STC_MENU_DECIMATION_BINNING_3_3:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_3;
		break;
	case STC_MENU_DECIMATION_BINNING_4_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_0;
		break;
	case STC_MENU_DECIMATION_BINNING_4_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_1;
		break;
	case STC_MENU_DECIMATION_BINNING_4_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_2;
		break;
	case STC_MENU_DECIMATION_BINNING_5_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_5_0;
		break;
	case STC_MENU_DECIMATION_BINNING_5_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_5_1;
		break;
	case STC_MENU_DECIMATION_BINNING_6_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_6_0;
		break;
	case STC_MENU_DECIMATION_BINNING_7_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_0;
		break;
	case STC_MENU_DECIMATION_BINNING_7_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_1;
		break;
	case STC_MENU_DECIMATION_BINNING_7_3:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_3;
		break;
	default:
		break;
	}
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_cancel_bulk(ddata);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] &= 0xff00;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] |= (val & 0x00ff);
	
	write_fpga_s(ddata, STC_COM_ADR_FPGA_BINNING_DECIMATION, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	read_picture_size(ddata, ddata->picture_size_length);
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_send_bulk(ddata);
	}
	ddata->dev_ctrl->device_status = device_status;
	return 0;
}

int stcam_dd_cam_s_h_decimation_binning(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val =0;
	enum stcam_e_device_status device_status = ddata->dev_ctrl->device_status;
	switch(param) {
	case STC_MENU_DECIMATION_BINNING_0_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_0_0;
		break;
	case STC_MENU_DECIMATION_BINNING_1_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_0;
		break;
	case STC_MENU_DECIMATION_BINNING_1_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_1;
		break;
	case STC_MENU_DECIMATION_BINNING_1_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_2;
		break;
	case STC_MENU_DECIMATION_BINNING_1_4:
		val = STC_COM_VALUE_DECIMATION_BINNING_1_4;
		break;
	case STC_MENU_DECIMATION_BINNING_2_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_0;
		break;
	case STC_MENU_DECIMATION_BINNING_2_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_1;
		break;
	case STC_MENU_DECIMATION_BINNING_2_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_2_2;
		break;
	case STC_MENU_DECIMATION_BINNING_3_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_0;
		break;
	case STC_MENU_DECIMATION_BINNING_3_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_1;
		break;
	case STC_MENU_DECIMATION_BINNING_3_3:
		val = STC_COM_VALUE_DECIMATION_BINNING_3_3;
		break;
	case STC_MENU_DECIMATION_BINNING_4_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_0;
		break;
	case STC_MENU_DECIMATION_BINNING_4_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_1;
		break;
	case STC_MENU_DECIMATION_BINNING_4_2:
		val = STC_COM_VALUE_DECIMATION_BINNING_4_2;
		break;
	case STC_MENU_DECIMATION_BINNING_5_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_5_0;
		break;
	case STC_MENU_DECIMATION_BINNING_5_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_5_1;
		break;
	case STC_MENU_DECIMATION_BINNING_6_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_6_0;
		break;
	case STC_MENU_DECIMATION_BINNING_7_0:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_0;
		break;
	case STC_MENU_DECIMATION_BINNING_7_1:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_1;
		break;
	case STC_MENU_DECIMATION_BINNING_7_3:
		val = STC_COM_VALUE_DECIMATION_BINNING_7_3;
		break;
	default:
		break;
	}
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_cancel_bulk(ddata);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] &= 0x00ff;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_DECIMATION] |= (val << 8 & 0xff00);
	
	write_fpga_s(ddata, STC_COM_ADR_FPGA_BINNING_DECIMATION, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	read_picture_size(ddata, ddata->picture_size_length);
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_send_bulk(ddata);
	}
	ddata->dev_ctrl->device_status = device_status;
	return 0;
}

int stcam_dd_cam_s_binningsum_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val =0;
	switch(param) {
	case STC_MENU_BINNINGSUM_MODE_OFF:
		val = STC_COM_VALUE_BINNINGSUM_MODE_OFF;
		break;
	case STC_MENU_BINNINGSUM_MODE_H_ON:
		val = STC_COM_VALUE_BINNINGSUM_MODE_H_ON;
		break;
	case STC_MENU_BINNINGSUM_MODE_V_ON:
		val = STC_COM_VALUE_BINNINGSUM_MODE_V_ON;
		break;
	default:
		break;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_BINNING_SUM] = val;
	
	write_fpga_s(ddata, STC_COM_ADR_FPGA_BINNING_SUM, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	return 0;
}

int stcam_dd_cam_s_clock(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 val =0;
	switch(param) {
	case STC_MENU_CLOCK_NORMAL:
		val = STC_COM_VALUE_CLOCK_NORMAL;
		break;
	case STC_MENU_CLOCK_DIV_2:
		val = STC_COM_VALUE_CLOCK_DIV_2;
		break;
	case STC_MENU_CLOCK_DIV_4:
		val = STC_COM_VALUE_CLOCK_DIV_4;
		break;
	case STC_MENU_CLOCK_VGA_90FPS:
		val = STC_COM_VALUE_CLOCK_VGA_90FPS;
		break;
	default:
		break;
	}
	MACRO_SET_CAM_VALUE_32(cam_param_clock, STC_COM_ADR_CLOCK_MODE, val)
	
	write_clock(ddata);
	read_clock(ddata);
	return 0;
}

int stcam_dd_cam_s_v_blank(struct stcam_st_dev_data *ddata, __u32 param) {
	if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_4) {
		ddata->cam_param_fpga2[e_fpga2_sub_fps_by_v_blanking] = param;
		return write_fpga2(ddata, e_fpga2_sub_fps_by_v_blanking);
	}
	
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_V_BLNAK_FOR_FPS] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_V_BLNAK_FOR_FPS, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_digital_gain(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_DIGITAL_GAIN] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_DIGITAL_GAIN, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_mode);
	}
	value = param - 1;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_EXPOSURE_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_EXPOSURE_MODE) & STC_COM_BITMASK_FPGA_EXPOSURE_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_sensor_shutter_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_sensor_shutter_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_sensor_shutter_mode);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_CMOS_RESET;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (param << STC_COM_BITSHIFT_FPGA_CMOS_RESET) & STC_COM_BITMASK_FPGA_CMOS_RESET;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_alc_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_ALC_MODE] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_ALC_MODE, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_alc_target_level(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_ALC_TARGET_LEVEL] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_ALC_TARGET_LEVEL, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_agc_min_gain(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AGC_MIN_GAIN] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_AGC_MIN_GAIN, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_agc_max_gain(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AGC_MAX_GAIN] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_AGC_MAX_GAIN, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_ae_min_exposure_clock(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MIN_EXP_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MIN_EXP_H] = ((param >> 16) & 0xffff);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_AE_MIN_EXP_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_ae_max_exposure_clock(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MAX_EXP_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_AE_MAX_EXP_H] = ((param >> 16) & 0xffff);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_AE_MAX_EXP_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_start_trigger_mode);
	}
	if(param == 1) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_source] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_start_trigger_source);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_delay] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_start_trigger_delay);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_start_trigger_overlap] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_start_trigger_overlap);
	}
	if(param == STC_MENU_TRIGGER_OVERLAP_OFF) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_burst_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_burst_trigger_mode);
	}
	if(param == 1) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_burst_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_source] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_burst_trigger_source);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_burst_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_delay] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_burst_trigger_delay);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_frame_burst_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_frame_burst_trigger_overlap] = param;
		return write_fpga2(ddata, e_fpga2_sub_frame_burst_trigger_overlap);
	}
	if(param == STC_MENU_TRIGGER_OVERLAP_OFF) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_start_trigger_mode);
	}
	if(param == 1) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_source] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_start_trigger_source);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_delay] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_start_trigger_delay);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_start_trigger_overlap] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_start_trigger_overlap);
	}
	if(param == STC_MENU_TRIGGER_OVERLAP_OFF) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_end_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_mode] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_end_trigger_mode);
	}
	if(param == 1) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_end_trigger_source(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_source] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_end_trigger_source);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_TRG_IN_SELECT) & STC_COM_BITMASK_FPGA_TRG_IN_SELECT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_end_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_delay] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_end_trigger_delay);
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_exposure_end_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		ddata->cam_param_fpga2[e_fpga2_sub_exposure_end_trigger_overlap] = param;
		return write_fpga2(ddata, e_fpga2_sub_exposure_end_trigger_overlap);
	}
	if(param == STC_MENU_TRIGGER_OVERLAP_OFF) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_read_out_start_trigger_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	if(param == 1) {
		value = 2;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MODE) & STC_COM_BITMASK_FPGA_TRIGGER_MODE;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_read_out_start_trigger_source(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	if(param == STC_MENU_TRIGGER_SOURCE_HARDWARE) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_READOUT_SELECT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (value << STC_COM_BITSHIFT_FPGA_READOUT_SELECT) & STC_COM_BITMASK_FPGA_READOUT_SELECT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_read_out_start_trigger_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_read_out_start_trigger_overlap(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 value;
	if(param == STC_MENU_TRIGGER_OVERLAP_OFF) {
		value = 1;
	}else {
		value = 0;
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] &= ~STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_H] |= (value << STC_COM_BITSHIFT_FPGA_TRIGGER_MASK) & STC_COM_BITMASK_FPGA_TRIGGER_MASK;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_H, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_trigger_software(struct stcam_st_dev_data *ddata, __u32 param) {
	int ret;
	__u32 value;
	__u32 data = ddata->cam_param_fpga2[e_fpga2_sub_camera_feature_a];
	if(( data >> STC_COM_BIT_FPGA2_FEATURE_A_STCAM_CAMERA_FUNCTION_GENICAM_IO ) & 1) {
		switch(param) {
		case STC_MENU_TRIGGER_SOFTWARE_FRAME_BURST_START:
			value = 0x00000008;
			break;
		case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_START:
			value = 0x00000010;
			break;
		case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_END:
			value = 0x00000002;
			break;
		case STC_MENU_TRIGGER_SOFTWARE_READ_OUT_START:
			value = 0x00000004;
			break;
		case STC_MENU_TRIGGER_SOFTWARE_FRAME_START:
		default:
			value = 0x00000001;
			break;
		}
		ddata->cam_param_fpga2[e_fpga2_sub_trigger_software] = value;
		ret = write_fpga2(ddata, e_fpga2_sub_trigger_software);
		ddata->cam_param_fpga2[e_fpga2_sub_trigger_software] = 0x00000000;
		return ret;
	}
	switch(param) {
	case STC_MENU_TRIGGER_SOFTWARE_FRAME_START:
	case STC_MENU_TRIGGER_SOFTWARE_FRAME_BURST_START:
	case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_START:
		ret = write_software_triger(ddata);
		if(ddata->usb_ctrl->timeout_soft_trigger > 0) {
			ddata->usb_ctrl->current_timer = STC_ERROR_TIMEOUT_ST2EE;
			ddata->usb_ctrl->timer_list.expires = ddata->usb_ctrl->timeout_soft_trigger;
			add_timer(&ddata->usb_ctrl->timer_list);
		}
		break;
	case STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_END:
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] |= STC_COM_BITMASK_FPGA_SUBTRIGGER;
		ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_REFRESH_RC_RO, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] = 0x0000;
		break;
	case STC_MENU_TRIGGER_SOFTWARE_READ_OUT_START:
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] |= STC_COM_BITMASK_FPGA_RO_CONTROL;
		ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_REFRESH_RC_RO, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] = 0x0000;
		if(ddata->usb_ctrl->timeout_readout > 0) {
			ddata->usb_ctrl->current_timer = STC_ERROR_TIMEOUT_RO2TE;
			ddata->usb_ctrl->timer_list.expires = ddata->usb_ctrl->timeout_readout;
			add_timer(&ddata->usb_ctrl->timer_list);
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

int stcam_dd_cam_s_exposure_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_EXPOSURE_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_EXPOSURE_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_strobe_start_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_STROBO_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_strobe_end_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_END_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_STROBO_END_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_STROBO_END_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_output_pulse_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_DELAY_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_DELAY_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_TRIGGER_DELAY_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_output_pulse_duration(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_PULSE_WIDTH_L] = param;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_PULSE_WIDTH_H] = ((param >> 16) & 0003);
	return write_fpga_l(ddata, STC_COM_ADR_FPGA_TRIGGER_PULSE_WIDTH_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_readout_delay(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_READOUT_DELAY] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_READOUT_DELAY, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_line_debounce_time(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga2[e_fpga2_sub_line_debounce_time] = param;
	return write_fpga2(ddata, e_fpga2_sub_line_debounce_time);
}

int stcam_dd_cam_s_io0_direction(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] &= ~STC_COM_BITMASK_FPGA_IO0;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] |= (param << STC_COM_BITSHIFT_FPGA_IO0) & STC_COM_BITMASK_FPGA_IO0;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_DIRECTION, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io0_polarity(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] &= ~STC_COM_BITMASK_FPGA_IO0;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] |= (param << STC_COM_BITSHIFT_FPGA_IO0) & STC_COM_BITMASK_FPGA_IO0;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_POLARITY, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io0_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val;
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] & STC_COM_BITMASK_FPGA_IO0) >> STC_COM_BITSHIFT_FPGA_IO0) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(param) {
		case STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT:
			val = STC_COM_VALUE_OUT_GENERAL_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_MENU_IO_MODE_OUT_EXPOSURE_END:
			val = STC_COM_VALUE_OUT_EXPOSURE_END;
			break;
		case STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT:
			val = STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT:
			val = STC_COM_VALUE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRANSFER_END:
			val = STC_COM_VALUE_OUT_TRANSFER_END;
			break;
		case STC_MENU_IO_MODE_OUT_DISABLE:
		default:
			val = STC_COM_VALUE_OUT_DISABLE;
			break;
		}
	} else {
		switch(param) {
		case STC_MENU_IO_MODE_IN_GENERAL_INPUT:
			val = STC_COM_VALUE_IN_GENERAL_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_READOUT_INPUT:
			val = STC_COM_VALUE_IN_READOUT_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_DISABLE:
		default:
			val = STC_COM_VALUE_IN_DISABLE;
			break;
		}
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_0] = val;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_MODE_0, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io0_status(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] &= ~STC_COM_BITMASK_FPGA_IO0;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] |= (param << STC_COM_BITSHIFT_FPGA_IO0) & STC_COM_BITMASK_FPGA_IO0;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_STATUS, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io1_direction(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] &= ~STC_COM_BITMASK_FPGA_IO1;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] |= (param << STC_COM_BITSHIFT_FPGA_IO1) & STC_COM_BITMASK_FPGA_IO1;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_DIRECTION, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io1_polarity(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] &= ~STC_COM_BITMASK_FPGA_IO1;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] |= (param << STC_COM_BITSHIFT_FPGA_IO1) & STC_COM_BITMASK_FPGA_IO1;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_POLARITY, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io1_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val;
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] & STC_COM_BITMASK_FPGA_IO1) >> STC_COM_BITSHIFT_FPGA_IO1) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(param) {
		case STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT:
			val = STC_COM_VALUE_OUT_GENERAL_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_MENU_IO_MODE_OUT_EXPOSURE_END:
			val = STC_COM_VALUE_OUT_EXPOSURE_END;
			break;
		case STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT:
			val = STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT:
			val = STC_COM_VALUE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRANSFER_END:
			val = STC_COM_VALUE_OUT_TRANSFER_END;
			break;
		case STC_MENU_IO_MODE_OUT_DISABLE:
		default:
			val = STC_COM_VALUE_OUT_DISABLE;
			break;
		}
	} else {
		switch(param) {
		case STC_MENU_IO_MODE_IN_GENERAL_INPUT:
			val = STC_COM_VALUE_IN_GENERAL_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_READOUT_INPUT:
			val = STC_COM_VALUE_IN_READOUT_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_DISABLE:
		default:
			val = STC_COM_VALUE_IN_DISABLE;
			break;
		}
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_1] = val;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_MODE_1, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io1_status(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] &= ~STC_COM_BITMASK_FPGA_IO1;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] |= (param << STC_COM_BITSHIFT_FPGA_IO1) & STC_COM_BITMASK_FPGA_IO1;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_STATUS, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io2_direction(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] &= ~STC_COM_BITMASK_FPGA_IO2;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] |= (param << STC_COM_BITSHIFT_FPGA_IO2) & STC_COM_BITMASK_FPGA_IO2;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_DIRECTION, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io2_polarity(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] &= ~STC_COM_BITMASK_FPGA_IO2;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] |= (param << STC_COM_BITSHIFT_FPGA_IO2) & STC_COM_BITMASK_FPGA_IO2;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_POLARITY, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io2_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val;
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] & STC_COM_BITMASK_FPGA_IO2) >> STC_COM_BITSHIFT_FPGA_IO2) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(param) {
		case STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT:
			val = STC_COM_VALUE_OUT_GENERAL_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_MENU_IO_MODE_OUT_EXPOSURE_END:
			val = STC_COM_VALUE_OUT_EXPOSURE_END;
			break;
		case STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT:
			val = STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT:
			val = STC_COM_VALUE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRANSFER_END:
			val = STC_COM_VALUE_OUT_TRANSFER_END;
			break;
		case STC_MENU_IO_MODE_OUT_DISABLE:
		default:
			val = STC_COM_VALUE_OUT_DISABLE;
			break;
		}
	} else {
		switch(param) {
		case STC_MENU_IO_MODE_IN_GENERAL_INPUT:
			val = STC_COM_VALUE_IN_GENERAL_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_READOUT_INPUT:
			val = STC_COM_VALUE_IN_READOUT_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_DISABLE:
		default:
			val = STC_COM_VALUE_IN_DISABLE;
			break;
		}
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_2] = val;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_MODE_2, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io2_status(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] &= ~STC_COM_BITMASK_FPGA_IO2;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] |= (param << STC_COM_BITSHIFT_FPGA_IO2) & STC_COM_BITMASK_FPGA_IO2;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_STATUS, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io3_direction(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] &= ~STC_COM_BITMASK_FPGA_IO3;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] |= (param << STC_COM_BITSHIFT_FPGA_IO3) & STC_COM_BITMASK_FPGA_IO3;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_DIRECTION, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io3_polarity(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] &= ~STC_COM_BITMASK_FPGA_IO3;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_POLARITY] |= (param << STC_COM_BITSHIFT_FPGA_IO3) & STC_COM_BITMASK_FPGA_IO3;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_POLARITY, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io3_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	__u16 val;
	if(((ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_DIRECTION] & STC_COM_BITMASK_FPGA_IO3) >> STC_COM_BITSHIFT_FPGA_IO3) == STC_MENU_IO_DIRECTION_OUTPUT) {
		switch(param) {
		case STC_MENU_IO_MODE_OUT_GENERAL_OUTPUT:
			val = STC_COM_VALUE_OUT_GENERAL_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH:
			val = STC_COM_VALUE_OUT_TRIGGER_OUTPUT_LOOP_THROUGH;
			break;
		case STC_MENU_IO_MODE_OUT_EXPOSURE_END:
			val = STC_COM_VALUE_OUT_EXPOSURE_END;
			break;
		case STC_MENU_IO_MODE_OUT_CCD_READ_END_OUTPUT:
			val = STC_COM_VALUE_OUT_CCD_READ_END_OUTPUT;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_PROGRAMMABLE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_PROGRAMMABLE;
			break;
		case STC_MENU_IO_MODE_OUT_STROBE_OUTPUT_EXPOSURE:
			val = STC_COM_VALUE_OUT_STROBE_OUTPUT_EXPOSURE;
			break;
		case STC_MENU_IO_MODE_OUT_TRIGGER_VALID_OUT:
			val = STC_COM_VALUE_OUT_TRIGGER_VALID_OUT;
			break;
		case STC_MENU_IO_MODE_OUT_TRANSFER_END:
			val = STC_COM_VALUE_OUT_TRANSFER_END;
			break;
		case STC_MENU_IO_MODE_OUT_DISABLE:
		default:
			val = STC_COM_VALUE_OUT_DISABLE;
			break;
		}
	} else {
		switch(param) {
		case STC_MENU_IO_MODE_IN_GENERAL_INPUT:
			val = STC_COM_VALUE_IN_GENERAL_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_READOUT_INPUT:
			val = STC_COM_VALUE_IN_READOUT_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_SUB_TRIGGER_INPUT:
			val = STC_COM_VALUE_IN_SUB_TRIGGER_INPUT;
			break;
		case STC_MENU_IO_MODE_IN_DISABLE:
		default:
			val = STC_COM_VALUE_IN_DISABLE;
			break;
		}
	}
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_MODE_3] = val;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_MODE_3, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_io3_status(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] &= ~STC_COM_BITMASK_FPGA_IO3;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_IO_PIN_STATUS] |= (param << STC_COM_BITSHIFT_FPGA_IO3) & STC_COM_BITMASK_FPGA_IO3;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_IO_PIN_STATUS, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_led_status(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_LED] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_LED, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_reset_switch(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_RESET_SW_ENABLED] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_RESET_SW_ENABLED, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_white_balance_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_MODE] = param;
	write_fpga_s(ddata, STC_COM_ADR_FPGA_WB_MODE, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	return 0;
}

int stcam_dd_cam_s_white_balance_gain_r(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_R] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_WB_GAIN_R, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_white_balance_gain_gr(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_GR] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_WB_GAIN_GR, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_white_balance_gain_gb(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_GB] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_WB_GAIN_GB, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_white_balance_gain_b(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_WB_GAIN_B] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_WB_GAIN_B, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
}

int stcam_dd_cam_s_camera_gamma(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_GAMMA] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_GAMMA, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_defect_pixel_correction_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_DEFFECT_PIXEL_CORRECTION_MODE] = param;
	write_fpga_s(ddata, STC_COM_ADR_FPGA_DEFFECT_PIXEL_CORRECTION_MODE, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	refresh_fpga(ddata);
	return 0;
}

int stcam_dd_cam_s_digital_clamp(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_DIGITAL_CLAMP] = param;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_DIGITAL_CLAMP, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_analog_black_level(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga2[e_fpga2_sub_analog_offset] = param;
	return write_fpga2(ddata, e_fpga2_sub_analog_offset);
}

int stcam_dd_cam_s_adjustment_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga2[e_fpga2_sub_digital_gain_enabled] = param;
	return write_fpga2(ddata, e_fpga2_sub_digital_gain_enabled);
}

int stcam_dd_cam_s_user_data(struct stcam_st_dev_data *ddata, struct stcam_st_user_data *param) {
	int ret = 0;
	__u8 *data = kzalloc(param->length, GFP_KERNEL);
	__u16 firm_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FIRM_VERSION);
	memcpy(data, param->data, param->length);
	if(firm_version >= 0x0100) {
		ret = write_eeprom_info(ddata, (STC_COM_ADR_EEPROM_INFO_USER_DATA_START + param->offset), param->length, data);
	} else {
		ret = write_eeprom_w(ddata, (STC_COM_ADR_EEPROM_W_USER_DATA_START + param->offset), param->length, data);
	}
	kfree(data);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
	}
	return ret;
}

int stcam_dd_cam_s_cam_user_id(struct stcam_st_dev_data *ddata, struct stcam_st_cam_user_id *param) {
	int ret = 0;
	__u8 *data = kzalloc(256, GFP_KERNEL);
	__u16 firm_version = MACRO_GET_CAM_VALUE_16(cam_param_camera_info, STC_COM_ADR_CAMERA_INFO_FIRM_VERSION);
	memset(data, 0, 256);
	memcpy(data, (__u8*)&param->camera_id, 4);
	data[5] = param->length;
	memcpy(&data[6], param->camera_name, param->length);
	if(firm_version >= 0x0100) {
		ret = write_eeprom_info(ddata, STC_COM_ADR_EEPROM_INFO_CAMERA_ID_START, 256, data);
	} else {
		ret = write_eeprom_w(ddata, STC_COM_ADR_EEPROM_W_CAMERA_ID_START, 256, data);
	}
	kfree(data);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
	}
	return ret;
}

int stcam_dd_cam_s_scan_size(struct stcam_st_dev_data *ddata, struct stcam_st_scan_size *param) {
	__u32 roi_count = stcam_dd_cam_g_max_roi_count(ddata);
	enum stcam_e_device_status device_status = ddata->dev_ctrl->device_status;
	if((roi_count == 0) || (param->roi.index == 0)) {
		MACRO_SET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_OFFSET_X, param->roi.offset_x)
		MACRO_SET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_OFFSET_Y, param->roi.offset_y)
		MACRO_SET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_WIDTH, param->roi.width)
		MACRO_SET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_HEIGHT, param->roi.height)
	} else {
		if(param->roi.index < roi_count) {
			__u16 roi_pos = param->roi.index - 1;
			__u32 region_mask = 0x00000001 << param->roi.index;
			__u32 region_mode = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_REGION_MODE);
			if(param->roi.region_mode) {
				region_mode |= region_mask;
			} else {
				region_mode &= ~region_mask;
			}
			MACRO_SET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_OFFSET_X + (roi_pos *16)), param->roi.offset_x)
			MACRO_SET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_OFFSET_Y + (roi_pos *16)), param->roi.offset_y)
			MACRO_SET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_WIDTH + (roi_pos *16)), param->roi.width)
			MACRO_SET_CAM_VALUE_32(cam_param_picture_size, (STC_COM_ADR_PICTURE_SIZE_ROI_HEIGHT + (roi_pos *16)), param->roi.height)
			MACRO_SET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_REGION_MODE, region_mode)
		} else {
			return -EINVAL;
		}
	}
	
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_cancel_bulk(ddata);
	}
	write_picture_size(ddata);
	read_picture_size(ddata, ddata->picture_size_length);
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_send_bulk(ddata);
	}
	ddata->dev_ctrl->device_status = device_status;
	return 0;
}

int stcam_dd_cam_s_hdr(struct stcam_st_dev_data *ddata, struct stcam_st_hdr *param) {
	int ret = 0;
	if(ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_size] != 0) {
		ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_0] = (param->cmosis_4m.mode & 0x00000001) | (param->cmosis_4m.slope << 8 &0x00000300);
		ret = write_fpga2(ddata, e_fpga2_sub_hdr_control_parametor_0);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
		ddata->cam_param_fpga2[e_fpga2_sub_hdr_control_parametor_1] = (param->cmosis_4m.vlow3 & 0x0000003F) | (param->cmosis_4m.vlow2 << 8 &0x00003F00) | (param->cmosis_4m.knee2 << 16 &0x00FF0000) | (param->cmosis_4m.knee1 << 24 &0xFF000000);
		ret = write_fpga2(ddata, e_fpga2_sub_hdr_control_parametor_1);
	} else {
		return -EINVAL;
	}
	return ret;
}

int stcam_dd_cam_s_def_pix_correct_pos(struct stcam_st_dev_data *ddata, struct stcam_st_def_pix_correct_pos *param) {
	int ret = 0;
	__u32 value;
	if(stcam_dd_cam_is_enable_defect_pixel_correction(ddata)) {
		if(param->index < stcam_dd_cam_g_defect_pixel_correction_count(ddata)) {
			value = (param->pos_x & 0x0000ffff) | (param->pos_y << 16 & 0xffff0000);
			ret = write_fpga_pix_correct(ddata, param->index, &value);
			refresh_fpga(ddata);
		} else {
			return -EINVAL;
		}
	} else {
		return -EINVAL;
	}
	return ret;
}

int stcam_dd_cam_s_sentech_mode(struct stcam_st_dev_data *ddata, __u32 param) {
	if(cam_model_table[ddata->dev_ctrl->product].category >= e_camera_category_type_2) {
		ddata->cam_param_fpga[STC_COM_ADR_FPGA_SENTECH_U3V_MODE] = param;
		return write_fpga_s(ddata, STC_COM_ADR_FPGA_SENTECH_U3V_MODE, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	}
	return 0;
}

int stcam_dd_cam_s_pixel_format(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 val = 0;
	__u32 mask = stcam_dd_cam_bitmask_transfer_bits_per_pixel(ddata);
	
	switch(param) {
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SBGGR8:
		val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08;
		break;
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SBGGR10:
		val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10;
		break;
	case V4L2_PIX_FMT_SRGGB12:
	case V4L2_PIX_FMT_SGRBG12:
	case V4L2_PIX_FMT_SGBRG12:
	case V4L2_PIX_FMT_SBGGR12:
		val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12;
		break;
	case V4L2_PIX_FMT_GREY:
		if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08 == (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08)) {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08;
		} else {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08;
		}
		break;
	case V4L2_PIX_FMT_Y10:
		if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10 == (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10)) {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10;
		} else {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10;
		}
		break;
	case V4L2_PIX_FMT_Y12:
		if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12 == (mask & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12)) {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12;
		} else {
			val = STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12;
		}
		break;
	case V4L2_PIX_FMT_BGR24:
		val = STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08;
		break;
	case V4L2_PIX_FMT_BGR32:
		val = STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10;
		break;
	default:
		return -EINVAL;
		break;
	}
	return stcam_dd_cam_s_transfer_bits_per_pixel(ddata, val);
}

int stcam_dd_cam_s_trigger_wait_hd(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_NEXT_H;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_NEXT_H) & STC_COM_BITMASK_FPGA_NEXT_H;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_trigger_wait_readout(struct stcam_st_dev_data *ddata, __u32 param) {
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_COMPLETE_READOUT;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (param << STC_COM_BITSHIFT_FPGA_COMPLETE_READOUT) & STC_COM_BITMASK_FPGA_COMPLETE_READOUT;
	return write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
}

int stcam_dd_cam_s_transfer_bits_per_pixel(struct stcam_st_dev_data *ddata, __u32 param) {
	__u32 val =0;
	enum stcam_e_device_status device_status = ddata->dev_ctrl->device_status;
	switch(param) {
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_08;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10P:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10P;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12P:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12P;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_08;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10P:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10P;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12P:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12P;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10:
		val = STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10;
		break;
	default:
		break;
	}
	MACRO_SET_CAM_VALUE_32(cam_param_pixel_byte, STC_COM_ADR_PIXEL_BYTE_TRANSFER_BITS_PER_PIXEL, val)
	
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_cancel_bulk(ddata);
	}
	write_pixel_byte(ddata);
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	if(device_status == e_device_status_streaming) {
		stcam_dd_usb_send_bulk(ddata);
	}
	ddata->dev_ctrl->device_status = device_status;
	return 0;
}

int stcam_dd_cam_update_crop_info(struct stcam_st_dev_data *ddata){
	ddata->dev_ctrl->crop_info.left = 0;
	ddata->dev_ctrl->crop_info.top = 0;
	ddata->dev_ctrl->crop_info.width = ddata->dev_ctrl->scan_info.width;
	ddata->dev_ctrl->crop_info.height = ddata->dev_ctrl->scan_info.height;
	return 0;
}

int stcam_dd_cam_update_format_info(struct stcam_st_dev_data *ddata) {
	ddata->dev_ctrl->format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ddata->dev_ctrl->format.fmt.pix.field = V4L2_FIELD_NONE;
	ddata->dev_ctrl->format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
	ddata->dev_ctrl->format.fmt.pix.priv = 0;
	
	ddata->dev_ctrl->format.fmt.pix.width = ddata->dev_ctrl->crop_info.width;
	ddata->dev_ctrl->format.fmt.pix.height = ddata->dev_ctrl->crop_info.height;
	ddata->dev_ctrl->format.fmt.pix.pixelformat = stcam_dd_cam_g_pixel_format(stcam_dd_cam_g_color_array(ddata), stcam_dd_cam_g_transfer_bits_per_pixel(ddata));
	ddata->dev_ctrl->format.fmt.pix.bytesperline = ddata->dev_ctrl->crop_info.width * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator;
	ddata->dev_ctrl->format.fmt.pix.sizeimage = ddata->dev_ctrl->format.fmt.pix.bytesperline * ddata->dev_ctrl->format.fmt.pix.height;
	return 0;
}

// static functions
int read_camera_info(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_CAMERA_INFO;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_CAMERA_INFO;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_camera_info);
}

int read_picture_size(struct stcam_st_dev_data *ddata, __u16 length) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_PICTURE_SIZE;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_picture_size);
}

int read_pixel_byte(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_PIXEL_BYTE;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_PIXEL_BYTE;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_pixel_byte);
}

int read_shutter_gain(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_SHUTTER_GAIN;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_SHUTTER_GAIN;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_shutter_gain);
}

int read_clock(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_CLOCK;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_CLOCK;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_clock);
}

int read_fpga(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_FPGA;
	ctrl_request.wValue = STC_COM_VAL_FPGA_READ;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_FPGA;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_fpga);
}

int read_fpga_pix_correct(struct stcam_st_dev_data *ddata, __u16 index, __u32 *param) {
	struct usb_ctrlrequest ctrl_request;
	__u16 addr = index * 2;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_FPGA;
	ctrl_request.wValue = STC_COM_VAL_FPGA_WRITE | addr;
	ctrl_request.wIndex = 0x81;
	ctrl_request.wLength = 4;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int read_fpga2(struct stcam_st_dev_data *ddata, __u16 index) {
	struct usb_ctrlrequest ctrl_request;
	__u32 offset;
	if(cam_model_table[ddata->dev_ctrl->product].category < e_camera_category_type_2) {
		return 0;
	}
	if(ddata->cam_param_fpga2_category_offset[cam_fpga2_table[index].category] == 0x00000000) {
		return 0;
	}
	offset = ddata->cam_param_fpga2_category_offset[cam_fpga2_table[index].category] + cam_fpga2_table[index].sub_offset;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_FPGA2;
	ctrl_request.wValue = (offset & 0x000000ff) << 8 | 0x0001;
	ctrl_request.wIndex = (offset & 0x00ffff00) >> 8;
	ctrl_request.wLength = 4;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga2[index]);
}

int read_fpga2_category_offset(struct stcam_st_dev_data *ddata, __u16 index) {
	int ret;
	struct usb_ctrlrequest ctrl_request;
	if(cam_model_table[ddata->dev_ctrl->product].category < e_camera_category_type_2) {
		return 0;
	}
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_FPGA2;
	ctrl_request.wValue = cam_fpga2_category_table[index] << 8 | 0x0001;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = 4;
	ret = stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga2_category_offset[index]);
	if(!(ddata->cam_param_fpga2_category_offset[index] & 0x01000000)) {
		ddata->cam_param_fpga2_category_offset[index] = 0x00000000;
	}
	ddata->cam_param_fpga2_category_offset[index] &= 0x00ffffff;
	return ret;
}

int read_eeprom_w(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_EEPROM_W;
	ctrl_request.wValue = addr;
	ctrl_request.wIndex = STC_COM_ADR_EEPROM_W_INDEX;
	ctrl_request.wLength = length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int read_eeprom_info(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_IN;
	ctrl_request.bRequest = STC_COM_REQ_EEPROM_INFO;
	ctrl_request.wValue = addr;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int write_picture_size(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_PICTURE_SIZE;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = ddata->picture_size_length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_picture_size);
}

int write_pixel_byte(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_PIXEL_BYTE;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_PIXEL_BYTE;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_pixel_byte);
}

int write_shutter_gain(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_SHUTTER_GAIN;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_SHUTTER_GAIN;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_shutter_gain);
}

int write_clock(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_CLOCK;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = STC_COM_LEN_CLOCK;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, ddata->cam_param_clock);
}

int write_software_triger(struct stcam_st_dev_data *ddata) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_SOFTWARE_TRIGER;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = 0;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, NULL);
}

int write_fpga_s(struct stcam_st_dev_data *ddata, __u16 addr, __u16 option) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_FPGA;
	ctrl_request.wValue = option | addr;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = 2;
	stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga[addr]);
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga[addr]);
}

int write_fpga_l(struct stcam_st_dev_data *ddata, __u16 addr, __u16 option) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_FPGA;
	ctrl_request.wValue = option | addr;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = 4;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga[addr]);
}

int write_fpga_pix_correct(struct stcam_st_dev_data *ddata, __u16 index, __u32 *param) {
	struct usb_ctrlrequest ctrl_request;
	__u16 addr = index * 2;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_FPGA;
	ctrl_request.wValue = addr;
	ctrl_request.wIndex = 0x81;
	ctrl_request.wLength = 4;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int refresh_fpga(struct stcam_st_dev_data *ddata) {
	int ret =0;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] |= STC_COM_BITMASK_FPGA_REFRESH;
	ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_REFRESH_RC_RO, STC_COM_VAL_FPGA_WRITE_NO_REFRESH);
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_REFRESH_RC_RO] = 0x0000;
	return ret;
}

int write_fx3(struct stcam_st_dev_data *ddata, __u16 *param) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_FX3;
	ctrl_request.wValue = 0;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = 2;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int write_fpga2(struct stcam_st_dev_data *ddata, __u16 index) {
	struct usb_ctrlrequest ctrl_request;
	__u32 offset;
	if(cam_model_table[ddata->dev_ctrl->product].category < e_camera_category_type_2) {
		return 0;
	}
	if(ddata->cam_param_fpga2_category_offset[cam_fpga2_table[index].category] == 0x00000000) {
		return 0;
	}
	offset = ddata->cam_param_fpga2_category_offset[cam_fpga2_table[index].category] + cam_fpga2_table[index].sub_offset;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_FPGA2;
	ctrl_request.wValue = (offset & 0x000000ff) << 8 | 0x0001;
	ctrl_request.wIndex = (offset & 0x00ffff00) >> 8;
	ctrl_request.wLength = 4;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, &ddata->cam_param_fpga2[index]);
}

int write_eeprom_w(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_EEPROM_W;
	ctrl_request.wValue = addr;
	ctrl_request.wIndex = STC_COM_ADR_EEPROM_W_INDEX;
	ctrl_request.wLength = length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int write_eeprom_info(struct stcam_st_dev_data *ddata, __u16 addr,  __u16 length, __u8 *param) {
	struct usb_ctrlrequest ctrl_request;
	ctrl_request.bRequestType = STC_COM_REQ_TYPE_OUT;
	ctrl_request.bRequest = STC_COM_REQ_EEPROM_INFO;
	ctrl_request.wValue = addr;
	ctrl_request.wIndex = 0;
	ctrl_request.wLength = length;
	return stcam_dd_usb_send_ctrl(ddata->usb_dev, ctrl_request, param);
}

int stcam_dd_cam_update_scan_info(struct stcam_st_dev_data *ddata) {
	ddata->dev_ctrl->scan_info.bytes_per_pixel = stcam_dd_cam_g_bytes_per_pixel(stcam_dd_cam_g_transfer_bits_per_pixel(ddata));
	if(16 == stcam_dd_cam_g_max_roi_count(ddata)) {
		struct stcam_st_scan_size scan_size;
		int i;
		scan_size.roi.index = 0;
		stcam_dd_cam_g_scan_size(ddata, &scan_size);
		ddata->dev_ctrl->scan_info.width = scan_size.roi.width;
		ddata->dev_ctrl->scan_info.height = scan_size.roi.height;
		for(i=1; i<4; i++) {
			scan_size.roi.index = i;
			stcam_dd_cam_g_scan_size(ddata, &scan_size);
			if(1 == scan_size.roi.region_mode) {
				ddata->dev_ctrl->scan_info.width += scan_size.roi.width;
			}
		}
		for(i=1; i<4; i++) {
			scan_size.roi.index = i * 4;
			stcam_dd_cam_g_scan_size(ddata, &scan_size);
			if(1 == scan_size.roi.region_mode) {
				ddata->dev_ctrl->scan_info.height += scan_size.roi.height;
			}
		ddata->dev_ctrl->scan_info.image_size = ddata->dev_ctrl->scan_info.width * ddata->dev_ctrl->scan_info.height * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator;
		}
	} else if(8 == stcam_dd_cam_g_max_roi_count(ddata)) {
		__u32 width, height;
		struct stcam_st_scan_size scan_size;
		int i;
		ddata->dev_ctrl->scan_info.width = 0;
		ddata->dev_ctrl->scan_info.height = 0;
		ddata->dev_ctrl->scan_info.image_size = 0;
		for(i=0; i<8; i++) {
			scan_size.roi.index = i;
			stcam_dd_cam_g_scan_size(ddata, &scan_size);
			if(1 == scan_size.roi.region_mode) {
				width = scan_size.roi.width;
				height = scan_size.roi.height;
				ddata->dev_ctrl->scan_info.image_size += width * height * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator;
				if(ddata->dev_ctrl->scan_info.width < width) {
					ddata->dev_ctrl->scan_info.width = width;
				}
				ddata->dev_ctrl->scan_info.height += height;
			}
		}
	} else if(4 == stcam_dd_cam_g_max_roi_count(ddata)) {
		struct stcam_st_scan_size scan_size;
		int i;
		scan_size.roi.index = 0;
		stcam_dd_cam_g_scan_size(ddata, &scan_size);
		ddata->dev_ctrl->scan_info.width = scan_size.roi.width;
		ddata->dev_ctrl->scan_info.height = scan_size.roi.height;
		for(i=1; i<2; i++) {
			scan_size.roi.index = i;
			stcam_dd_cam_g_scan_size(ddata, &scan_size);
			if(1 == scan_size.roi.region_mode) {
				ddata->dev_ctrl->scan_info.width += scan_size.roi.width;
			}
		}
		for(i=1; i<2; i++) {
			scan_size.roi.index = i * 2;
			stcam_dd_cam_g_scan_size(ddata, &scan_size);
			if(1 == scan_size.roi.region_mode) {
				ddata->dev_ctrl->scan_info.height += scan_size.roi.height;
			}
		ddata->dev_ctrl->scan_info.image_size = ddata->dev_ctrl->scan_info.width * ddata->dev_ctrl->scan_info.height * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator;
		}
	} else {
		ddata->dev_ctrl->scan_info.width = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_WIDTH);
		ddata->dev_ctrl->scan_info.height = MACRO_GET_CAM_VALUE_32(cam_param_picture_size, STC_COM_ADR_PICTURE_SIZE_HEIGHT);
		ddata->dev_ctrl->scan_info.image_size = ddata->dev_ctrl->scan_info.width * ddata->dev_ctrl->scan_info.height * ddata->dev_ctrl->scan_info.bytes_per_pixel.numerator / ddata->dev_ctrl->scan_info.bytes_per_pixel.denominator;
	}
	if(ddata->usb_ctrl) {
		ddata->usb_ctrl->image_size = ddata->dev_ctrl->scan_info.image_size;
	}
	return 0;
}

__u32 stcam_dd_cam_g_pixel_format(__u16 color_array, __u16 bits_per_pixel) {
	return stcam_dd_cam_pixelformat_table[bits_per_pixel][color_array-1];
}

struct v4l2_fract stcam_dd_cam_g_bytes_per_pixel(__u16 bits_per_pixel) {
	struct v4l2_fract bytes_per_pixel;
	switch(bits_per_pixel) {
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12:
		bytes_per_pixel.numerator = 2;
		bytes_per_pixel.denominator = 1;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10P:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10P:
		bytes_per_pixel.numerator = 5;
		bytes_per_pixel.denominator = 4;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12P:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12P:
		bytes_per_pixel.numerator = 3;
		bytes_per_pixel.denominator = 2;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08:
		bytes_per_pixel.numerator = 3;
		bytes_per_pixel.denominator = 1;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_10:
		bytes_per_pixel.numerator = 6;
		bytes_per_pixel.denominator = 1;
		break;
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_08:
	case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_08:
	default:
		bytes_per_pixel.numerator = 1;
		bytes_per_pixel.denominator = 1;
		break;
	}
	return bytes_per_pixel;
}

int stcam_dd_cam_update_camera_info(struct stcam_st_dev_data *ddata) {
	int ret, i;
	
	// Camera Info
	ret = read_camera_info(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Picture Size
	ret = read_picture_size(ddata, ddata->picture_size_length);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Pixel Byte
	ret = read_pixel_byte(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Shutter Gain
	ret = read_shutter_gain(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// Clock
	ret = read_clock(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// FPGA
	ret = read_fpga(ddata);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// FPGA2
	for(i=0; i<e_fpga2_sub_category_max; i++) {
		ret = read_fpga2(ddata, i);
		if(ret < 0) {
			stcam_dd_util_error_log(ret);
			return ret;
		}
	}
	
	stcam_dd_cam_update_scan_info(ddata);
	stcam_dd_cam_update_crop_info(ddata);
	stcam_dd_cam_update_format_info(ddata);
	
	// split mode
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] &= ~STC_COM_BITMASK_FPGA_PACKET_SPLIT_MODE;
	ddata->cam_param_fpga[STC_COM_ADR_FPGA_TRIGGER_MODE_L] |= (1 << STC_COM_BITSHIFT_FPGA_PACKET_SPLIT_MODE) & STC_COM_BITMASK_FPGA_PACKET_SPLIT_MODE;
	ret = write_fpga_s(ddata, STC_COM_ADR_FPGA_TRIGGER_MODE_L, STC_COM_VAL_FPGA_WRITE);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	// sentech mode
	ret = stcam_dd_cam_s_sentech_mode(ddata, 0x0000);
	if(ret < 0) {
		stcam_dd_util_error_log(ret);
		return ret;
	}
	
	return 0;
}

struct v4l2_fract stcam_dd_cam_g_max_bytes_per_pixel(__u32 bitmask_transfer_bits_per_pixel) {
	struct v4l2_fract bytes_per_pixel;
	if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_10)) {
		bytes_per_pixel.numerator = 6;
		bytes_per_pixel.denominator = 1;
	} else if(STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_BGR_08)) {
		bytes_per_pixel.numerator = 3;
		bytes_per_pixel.denominator = 1;
	} else if((STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_10)) || (STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_RAW_12))
				|| (STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_10)) || (STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12 == (bitmask_transfer_bits_per_pixel & STC_COM_VALUE_TRANSFER_BITS_PER_PIXEL_MONO_12))) {
		bytes_per_pixel.numerator = 2;
		bytes_per_pixel.denominator = 1;
	} else {
		bytes_per_pixel.numerator = 1;
		bytes_per_pixel.denominator = 1;
	}
	return bytes_per_pixel;
}
