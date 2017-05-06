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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "rtlsdr_i2c.h"
#include "tuner_mxl5007t.h"

struct reg_pair_t {
	unsigned char reg;
	unsigned char val;
};

static struct reg_pair_t init_tab[] = {
	{ 0x02, 0x06 },
	{ 0x03, 0x48 },
	{ 0x05, 0x04 },
	{ 0x06, 0x10 },
	{ 0x2e, 0x15 }, /* OVERRIDE */
	{ 0x30, 0x10 }, /* OVERRIDE */
	{ 0x45, 0x58 }, /* OVERRIDE */
	{ 0x48, 0x19 }, /* OVERRIDE */
	{ 0x52, 0x03 }, /* OVERRIDE */
	{ 0x53, 0x44 }, /* OVERRIDE */
	{ 0x6a, 0x4b }, /* OVERRIDE */
	{ 0x76, 0x00 }, /* OVERRIDE */
	{ 0x78, 0x18 }, /* OVERRIDE */
	{ 0x7a, 0x17 }, /* OVERRIDE */
	{ 0x85, 0x06 }, /* OVERRIDE */
	{ 0x01, 0x01 }, /* TOP_MASTER_ENABLE */
	{ 0, 0 }
};

static struct reg_pair_t init_tab_cable[] = {
	{ 0x02, 0x06 },
	{ 0x03, 0x48 },
	{ 0x05, 0x04 },
	{ 0x06, 0x10 },
	{ 0x09, 0x3f },
	{ 0x0a, 0x3f },
	{ 0x0b, 0x3f },
	{ 0x2e, 0x15 }, /* OVERRIDE */
	{ 0x30, 0x10 }, /* OVERRIDE */
	{ 0x45, 0x58 }, /* OVERRIDE */
	{ 0x48, 0x19 }, /* OVERRIDE */
	{ 0x52, 0x03 }, /* OVERRIDE */
	{ 0x53, 0x44 }, /* OVERRIDE */
	{ 0x6a, 0x4b }, /* OVERRIDE */
	{ 0x76, 0x00 }, /* OVERRIDE */
	{ 0x78, 0x18 }, /* OVERRIDE */
	{ 0x7a, 0x17 }, /* OVERRIDE */
	{ 0x85, 0x06 }, /* OVERRIDE */
	{ 0x01, 0x01 }, /* TOP_MASTER_ENABLE */
	{ 0, 0 }
};

static struct reg_pair_t reg_pair_rftune[] = {
	{ 0x0f, 0x00 }, /* abort tune */
	{ 0x0c, 0x15 },
	{ 0x0d, 0x40 },
	{ 0x0e, 0x0e },
	{ 0x1f, 0x87 }, /* OVERRIDE */
	{ 0x20, 0x1f }, /* OVERRIDE */
	{ 0x21, 0x87 }, /* OVERRIDE */
	{ 0x22, 0x1f }, /* OVERRIDE */
	{ 0x80, 0x01 }, /* freq dependent */
	{ 0x0f, 0x01 }, /* start tune */
	{ 0, 0 }
};

/* Private functions */
static int mxl5007t_write_reg(void *dev, uint8_t reg, const uint8_t val)
{
	int rc = 0;
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = val;

	rc = rtlsdr_i2c_write_fn(dev, MXL5007T_I2C_ADDR, buf, 2);

	return rc;
}

static int mxl5007t_read_reg(void *dev, uint8_t reg, uint8_t *val)
{
	int rc = 0;
	uint8_t buf[2];
	buf[0] = 0xFB;
	buf[1] = reg;
	uint8_t data = 0;

	rc = rtlsdr_i2c_write_fn(dev, MXL5007T_I2C_ADDR, buf, 2);
	if (rc < 2)
		return -1;
	
	rc = rtlsdr_i2c_read_fn(dev, MXL5007T_I2C_ADDR, &data, 1);
	*val = data;
	return rc;
}

static int mxl5007t_write_regs(void *dev, struct reg_pair_t *reg_pair)
{
	int i = 0;
	int x = 1;
	while (reg_pair[i].reg || reg_pair[i].val)
	{
		x = x & mxl5007t_write_reg(dev, reg_pair[i].reg, reg_pair[i].val);
		++i;
	}
	return x;
}

static void mxl5007t_set_reg_bits(struct reg_pair_t *reg_pair, unsigned char reg, unsigned char mask, unsigned char val)
{
	unsigned int i = 0;

	while (reg_pair[i].reg || reg_pair[i].val) {
		if (reg_pair[i].reg == reg) {
			reg_pair[i].val &= ~mask;
			reg_pair[i].val |= val;
		}
		i++;

	}
	return;
}

static void mxl5007t_copy_reg_bits(struct reg_pair_t *reg_pair1,
			  struct reg_pair_t *reg_pair2)
{
	unsigned int i, j;

	i = j = 0;

	while (reg_pair1[i].reg || reg_pair1[i].val) {
		while (reg_pair2[j].reg || reg_pair2[j].val) {
			if (reg_pair1[i].reg != reg_pair2[j].reg) {
				j++;
				continue;
			}
			reg_pair2[j].val = reg_pair1[i].val;
			break;
		}
		i++;
	}
	return;
}

/* Public functions */
int mxl5007t_print_version(uint8_t chip_id) {
	switch (chip_id) {
		case MxL_5007_V1_F1:
			fprintf(stderr, "Found MxL5007T version 1.f1 tuner\n");
			break;
		case MxL_5007_V1_F2:
			fprintf(stderr, "Found MxL5007T version 1.f2 tuner\n");
			break;
		case MxL_5007_V4:
			fprintf(stderr, "Found MxL5007T version 4 tuner\n");
			break;
		case MxL_5007_V2_100_F1:
			fprintf(stderr, "Found MxL5007T version 2.100.f1 tuner\n");
			break;
		case MxL_5007_V2_100_F2:
			fprintf(stderr, "Found MxL5007T version 2.100.f2 tuner\n");
			break;	
		case MxL_5007_V2_200_F1:
			fprintf(stderr, "Found MxL5007T version 2.200.f1 tuner\n");
			break;
		case MxL_5007_V2_200_F2:
			fprintf(stderr, "Found MxL5007T version 2.200.f2 tuner\n");
			break;
		default:
			return 0;
	}
	return 1;
}

int mxl5007t_init(void *dev)
{
	mxl5007t_set_reg_bits(init_tab, 0x06, 0x1f, 0x11); // DVB-T
	
	mxl5007t_set_reg_bits(init_tab, 0x02, 0x0f, 0x03); // set 4.57 MHz IF
	mxl5007t_set_reg_bits(reg_pair_rftune, 0x0c, 0x3f, 0x15); // set 8 MHz bandwidth
	mxl5007t_set_reg_bits(init_tab, 0x03, 0xF0, 0x40); // 24 MHz XTAL
	mxl5007t_set_reg_bits(init_tab, 0x05, 0x0F, 0x04); // 24 MHz XTAL

	mxl5007t_write_regs(dev, init_tab);

	return 0;
}

int mxl5007t_standby(void *dev)
{
	mxl5007t_write_reg(dev, 0x01, 0x00);
	mxl5007t_write_reg(dev, 0x0F, 0x00);
	return 0;
}

int mxl5007t_set_freq(void *dev, uint32_t freq)
{
	unsigned long int dig_rf_freq = 0;
	unsigned long int temp;
	unsigned long int frac_divider = 1000000;
	unsigned int i;
	uint8_t lock = 0;
	int lock_rf = 0, lock_ref = 0;
	
	fprintf(stderr, "MxL5007T: Tuning to %u Hz\n", freq);

	dig_rf_freq = freq / 1000000;
	temp = freq % 1000000;

    for (i = 0; i < 6; i++)
    {
        dig_rf_freq <<= 1;
        frac_divider /= 2;
        if (temp > frac_divider)
        {
            temp -= frac_divider;
            dig_rf_freq++;
        }
    }

    if (temp > 7812) dig_rf_freq++;
	
	mxl5007t_set_reg_bits(reg_pair_rftune, 0x0D, 0xFF, (unsigned int)dig_rf_freq);
	mxl5007t_set_reg_bits(reg_pair_rftune, 0x0E, 0xFF, (unsigned int)(dig_rf_freq >> 8));
	
	if (freq > 333000000)
		mxl5007t_set_reg_bits(reg_pair_rftune, 0x80, 0x40, 0x40);
		
	mxl5007t_write_regs(dev, reg_pair_rftune); 

	
	mxl5007t_read_reg(dev, 0xd8, &lock);

	lock_ref = lock & 0x03;
	lock_rf = lock & 0x0c;
	
	if (lock_ref == 0x03)
		fprintf(stderr, "MxL5007T: REF locked\n");
	else
		fprintf(stderr, "MxL5007T: REF NOT locked\n");
	
	if (lock_rf == 0x0c)
		fprintf(stderr, "MxL5007T: RF locked\n");
	else
		fprintf(stderr, "MxL5007T: RF NOT locked\n");

	return 0;
}