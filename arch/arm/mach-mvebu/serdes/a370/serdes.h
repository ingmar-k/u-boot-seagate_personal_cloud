/*
 * (C) Copyright 2014, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __A370_SERDES_H
#define __A370_SERDES_H

#define A370_SERDES_MAX			4

enum serdes_sel {
	SERDES_SEL_NONE,
	SERDES_SEL_PCIE,
	SERDES_SEL_SATA,
	SERDES_SEL_SGMII,

	SERDES_SEL_MAX
};

enum serdes_opt {
	SERDES_OPT_SPD_HI,
	SERDES_OPT_SPD_LO,

	SERES_OPT__MAX
};

struct serdes_cfg {
	enum serdes_sel	sel;
	enum serdes_opt	opt;
};

struct serdes_cfg* board_serdes_cfg(void);

#endif /* __A370_SERDES_H */
