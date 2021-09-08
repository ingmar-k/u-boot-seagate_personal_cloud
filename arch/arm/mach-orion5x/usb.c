/*
 * (C) Copyright 2012
 * Steve Shih <shihsung@gmail.com>
 * Derived from u-boot-1.1.1_buf109
 *
 * Copyright (C) Marvell International Ltd. and its affiliates
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>

void orion5x_usb_workaround(unsigned long port_base)
{
	u32 reg, timeout, did, rev;

	/* clear interrupt cause and mask registers */
	writel(0, port_base + 0x310);
	writel(0, port_base + 0x314);

	/* reset controller */
	writel(readl(port_base + 0x140) | 0x00000002, port_base + 0x140);
	for (timeout = 10000; timeout; timeout--) {
		if (!(readl(port_base + 0x140) & 0x00000002))
			break;
		mdelay(1);
	}

	/******** Update USB PHY configuration ********/

	/* The new register 0x360 USB 2.0 IPG Metal Fix Register
	 * doesn't exist in the following chip revisions:
	 * Orion1 B1 (id=0x5181, rev=3) and before
	 * Orion1-VoIP A0 (id=0x5181, rev=8)
	 * Orion1-NAS A1 (id=0x5182, rev=1) and before
	 * Orion2 B0 (id=0x5281, rev=1) and before
	 */
	did = orion5x_device_id();
	rev = orion5x_device_rev();
	if (!(((did == 0x5181) && ((rev <= 3) || (rev == 8))) ||
	      ((did == 0x5182) && (rev <= 1)) ||
	      ((did == 0x5281) && (rev <= 1)))) {
		/* Change value of new register 0x360 */
		reg = readl(port_base + 0x360);

		/* Bit[14:8] (IPG for non Start of Frame packets) = 0xc */
		reg &= ~(0x7f << 8);
		reg |= (0xc << 8);

		writel(reg, port_base + 0x360);
	}

	/******** USB 2.0 Power Control Register 0x400 ********/
	reg = readl(port_base + 0x400);

	/* Bit[7:6] (BG_VSEL) = 0x1 */
	reg &= ~(0x3 << 6);
	reg |= (0x1 << 6);

	writel(reg, port_base + 0x400);

	/******** USB PHY Tx Control Register 0x420 ********/
	reg = readl(port_base + 0x420);

	if ((did == 0x5181) && (rev <= 1)) {
		/* For Orion1 A1/A0 rev: Bit[21] (TXDATA_BLOCK_EN) = 0 */
		reg &= ~(1 << 21);
	} else {
		reg |= (1 << 21);
	}

	/* Force Auto calibration to 0x8 */

	/* Bit[13] (REG_EXT_RCAL_EN) = 0x1 */
	reg |= (1 << 13);

	/* Bit[6:3] (IMP_CAL) = 0x8 */
	reg &= ~(0xf << 3);
	reg |= (0x8 << 3);

	writel(reg, port_base + 0x420);

	/******** USB PHY Rx Control Register 0x430 ********/
	reg = readl(port_base + 0x430);

	/* Bit[9:8] (DISCON_THRESHOLD) */
	/*
	 * Orion1-A0/A1/B0 = 0x3
	 * Orion2-A0 = 0x2
	 * Orion1-B1 and Orion2-B0 later = 0x0
	 */
	reg &= ~(0x3 << 8);
	if ((did == 0x5181) && (rev <= 2)) {
		reg |= (0x3 << 8);
	} else if ((did == 0x5281) && (rev == 0)) {
		reg |= (0x2 << 8);
	}

	/* Bit[21] (CDR_FASTLOCK_EN) = 0x0 */
	reg &= ~(1 << 21);

	/* Bit[27:26] (EDGE_DET_SEL) = 0x0 */
	reg &= ~(0x3 << 26);

	/* Bit[31:30] (RXDATA_BLOCK_LENGTH) = 0x3 */
	reg |= (0x3 << 30);

	/* Bit[7:4] (SQ_THRESH) = 0x1 */
	reg &= ~(0xf << 4);
	reg |= (0x1 << 4);

	writel(reg, port_base + 0x430);

	/******** USB PHY IVREF Control Register 0x440 ********/
	reg = readl(port_base + 0x440);

	/* Bit[1:0] (PLLVDD12) = 0x2 */
	reg &= ~(0x3 << 0);
	reg |= (0x2 << 0);

	/* Bit[5:4] (RXVDD) = 0x3 */
	reg |= (0x3 << 4);

	/* Bit[19] (Reserved) = 0x0 */
	reg &= ~(0x1 << 19);

	writel(reg, port_base + 0x440);

	/******** USB PHY TEST GROUP CONTROL Register: 0x450 ********/
	reg = readl(port_base + 0x450);

	/* Bit[15] (REG_FIFO_SQ_RST) = 0x0 */
	reg &= ~(0x1 << 15);

	writel(reg, port_base + 0x450);
}
