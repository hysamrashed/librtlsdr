/*
 * Driver for the MaxLinear MxL5007T silicon tuner
 *
 * Copyright (C) 2008, 2009 Michael Krufky <mkrufky@linuxtv.org>
 * Adapted for RTL-SDR by OneTransistor <one1transistor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#ifndef MXL5007T_H
#define MXL5007T_H

#define MXL5007T_I2C_ADDR	0xC0
#define MXL5007T_CHECK_REG	0xD9

enum mxl5007t_chip_version {
	MxL_UNKNOWN_ID     = 0x00,
	MxL_5007_V1_F1     = 0x11,
	MxL_5007_V1_F2     = 0x12,
	MxL_5007_V4        = 0x14,
	MxL_5007_V2_100_F1 = 0x21,
	MxL_5007_V2_100_F2 = 0x22,
	MxL_5007_V2_200_F1 = 0x23,
	MxL_5007_V2_200_F2 = 0x24,
};

/*enum mxl5007t_if_freq {
	MxL_IF_4_MHZ,      //  4000000
	MxL_IF_4_5_MHZ,    //  4500000
	MxL_IF_4_57_MHZ,   //  4570000
	MxL_IF_5_MHZ,      //  5000000
	MxL_IF_5_38_MHZ,   //  5380000
	MxL_IF_6_MHZ,      //  6000000
	MxL_IF_6_28_MHZ,   //  6280000
	MxL_IF_9_1915_MHZ, //  9191500
	MxL_IF_35_25_MHZ,  // 35250000
	MxL_IF_36_15_MHZ,  // 36150000
	MxL_IF_44_MHZ,     // 44000000
}; */

/*enum mxl5007t_bw_mhz {
	MxL_BW_6MHz = 6,
	MxL_BW_7MHz = 7,
	MxL_BW_8MHz = 8,
};*/

/*enum mxl5007t_mode {
	MxL_MODE_ISDBT     =    0,
	MxL_MODE_DVBT      =    1,
	MxL_MODE_ATSC      =    2,
	MxL_MODE_CABLE     = 0x10,
};*/

int mxl5007t_print_version(uint8_t chip_id);
int mxl5007t_init(void *dev);
int mxl5007t_standby(void *dev);
int mxl5007t_set_freq(void *dev, uint32_t freq);

#endif