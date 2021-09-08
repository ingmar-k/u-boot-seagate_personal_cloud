/*
 * (C) Copyright 2012
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

.section	".romboot", #alloc, #execinstr

_romboot:
	/*
	 * start DDR SDRAM initialization sequence here to workaround
	 * "polling SDRAM operation until back in 'normal' mode" hangs.
	 */
	ldr	r0, =0xd0001480	/* DDR SDRAM initialization control register */
	ldr	r1, =0x00000001	/* DDR SDRAM initialization enable */
	str	r1, [r0]

	b	_start
