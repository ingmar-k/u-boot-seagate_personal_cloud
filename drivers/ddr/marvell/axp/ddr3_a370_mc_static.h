/*
 * Copyright (C) Marvell International Ltd. and its affiliates
 *
 * SPDX-License-Identifier:	GPL-2.0
 *
 * Taken from u-boot-armada-2011.12
 */

#ifndef __A370_MC_STATIC_H
#define __A370_MC_STATIC_H

/* MC - DUNIT registers values - relevant only if SPD_SUPPORT is NOT defined */
MV_DRAM_MC_INIT ddr3_A0_db_400[MV_MAX_DDR3_STATIC_SIZE] =
{
	{0x00001400, 0x73004926}, 	/*DDR SDRAM Configuration Register */
	{0x00001404, 0x3630B800},	/*Dunit Control Low Register */
	{0x00001408, 0x33137663},	/*DDR SDRAM Timing (Low) Register */
	{0x0000140C, 0x38000C30},	/*DDR SDRAM Timing (High) Register */
	{0x00001410, 0x020F0000}, 	/*DDR SDRAM Open Pages Control Register */
	{0x00001414, 0x00000000},	/*DDR SDRAM Open Pages Control Register */
	{0x00001418, 0x00000e00},	/*DDR SDRAM Operation Register */
	{0x00001420, 0x00000004},	/*DDR SDRAM Extended Mode Register */
	{0x00001424, 0x0100D1FF},	/*Dunit Control High Register */
	{0x00001428, 0x000D6720},	/*Dunit Control High Register */
	{0x0000142C, 0x014C2F38},	/*Dunit Control High Register */
	{0x0000147C, 0x00006571},

	{0x00001484, 0x0000E907},	/* AXI/Mbus Arbiter  */

	{0x00001494, 0x00010000},	/*DDR SDRAM ODT Control (Low) Register */
	{0x00001498, 0x00000000},	/*DDR SDRAM ODT Control (High) Register */
	{0x0000149C, 0x00000001},	/*DDR Dunit ODT Control Register */

	{0x000014C0, 0x192424C8}, 	/* DRAM address and Control Driving Strenght  */
	{0x000014C4, 0xEFB24C8}, 	/* DRAM Data and DQS Driving Strenght  */

	{0x000200e8, 0x1FFF0E01}, 	/* DO NOT Modify - Open Mbus Window - 2G - Mbus is required for the training sequence*/
 	{0x00020184, 0x1FFFFFE0}, 	/* DO NOT Modify - Close fast path Window to - 2G */

	{0x0001504, 0x1FFFFFF1}, 	/* CS0 Size */
	{0x000150C, 0x00000000}, 	/* CS1 Size */
	{0x0001514, 0x00000000}, 	/* CS2 Size */
	{0x000151C, 0x00000000}, 	/* CS3 Size */
 
	{0x00001538, 0x00000006},	/*Read Data Sample Delays Register */
	{0x0000153C, 0x00000007},	/*Read Data Ready Delay Register */

	{0x000015D0, 0x00000610},	/*MR0 */
	{0x000015D4, 0x00000046},	/*MR1 */
	{0x000015D8, 0x00000008},	/*MR2 */
	{0x000015DC, 0x00000000},	/*MR3 */

	{0x000015E4, 0x00203c18},	/*ZQDS Configuration Register */
	{0x000015EC, 0xDE000025},	/*DDR PHY */
	{0x0, 0x0}
};

/* MC - DUNIT registers values - relevant only if SPD_SUPPORT is NOT defined */
MV_DRAM_MC_INIT ddr3_A0_db_600[MV_MAX_DDR3_STATIC_SIZE] =
{
	{0x00001400, 0x73014924}, 	/*DDR SDRAM Configuration Register */
	{0x00001404, 0x3000B800}, 	/*Dunit Control Low Register */
	{0x00001408, 0x45148886}, 	/*DDR SDRAM Timing (Low) Register */
	{0x0000140C, 0x3A000C5F}, 	/*DDR SDRAM Timing (High) Register */

	{0x00001410, 0x040F0000}, 	/*DDR SDRAM Open Pages Control Register */

	{0x00001414, 0x00000000}, 	/*DDR SDRAM Open Pages Control Register */
	{0x00001418, 0x00000e00}, 	/*DDR SDRAM Operation Register */
	{0x00001420, 0x00000004}, 	/*DDR SDRAM Extended Mode Register */
	{0x00001424, 0x0100D1FF}, 	/*Dunit Control High Register */
	{0x00001428, 0x000E7830}, 	/*Dunit Control High Register */
	{0x0000142C, 0x214C2F38}, 	/*Dunit Control High Register */
	{0x0000147C, 0x0000c671},

	{0x00001484, 0x0000E907},	/* AXI/Mbus Arbiter  */
	{0x000014a8, 0x00000100}, 	/* AXI Control */

	{0x00001494, 0x00010000}, 	/*DDR SDRAM ODT Control (Low) Register */
	{0x00001498, 0x00000000}, 	/*DDR SDRAM ODT Control (High) Register */
	{0x0000149C, 0x00000001}, 	/*DDR Dunit ODT Control Register */

	{0x000014C0, 0x192424C8}, 	/* DRAM address and Control Driving Strenght  */
	{0x000014C4, 0xEFB24C8}, 	/* DRAM Data and DQS Driving Strenght  */

	{0x000200e8, 0x1FFF0E01}, 	/*Open Mbus Window - 2G */
	{0x00020184, 0x1FFFFFE0}, 	/*Close fast path Window to - 2G */

	{0x00001504, 0x1FFFFFF1}, 	/*Dummy */
	{0x0000150C, 0x00000000}, 	/*Close all 15xx registers they are not used */
	{0x00001514, 0x00000000}, 	/*Close all 15xx registers they are not used */
	{0x0000151C, 0x00000000}, 	/*Close all 15xx registers they are not used */

	{0x00001538, 0x0000000b}, 	/*Read Data Sample Delays Register */
	{0x0000153C, 0x0000000d}, 	/*Read Data Ready Delay Register */

	{0x000015D0, 0x00000640}, 	/*MR0 */
	{0x000015D4, 0x00000046}, 	/*MR1 */
	{0x000015D8, 0x00000010}, 	/*MR2 */
	{0x000015DC, 0x00000000}, 	/*MR3 */

	{0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
	{0x000015EC, 0xDE000025}, 	/*DDR PHY */
	{0x0, 0x0}
};

/* MC - DUNIT registers values - relevant only if SPD_SUPPORT is NOT defined */
MV_DRAM_MC_INIT ddr3_A0_rd_600[MV_MAX_DDR3_STATIC_SIZE] =
{
	{0x00001400, 0x73014924}, 	/*DDR SDRAM Configuration Register */
	{0x00001404, 0x3000B810}, 	/*Dunit Control Low Register */
	{0x00001408, 0x45148886}, 	/*DDR SDRAM Timing (Low) Register */
	{0x0000140C, 0x3A000C5F}, 	/*DDR SDRAM Timing (High) Register */

	{0x00001410, 0x040F0000}, 	/*DDR SDRAM Open Pages Control Register */

	{0x00001414, 0x00000000}, 	/*DDR SDRAM Open Pages Control Register */
	{0x00001418, 0x00000e00}, 	/*DDR SDRAM Operation Register */
	{0x00001420, 0x00000004}, 	/*DDR SDRAM Extended Mode Register */
	{0x00001424, 0x0100D1FF}, 	/*Dunit Control High Register */
	{0x00001428, 0x000E7830}, 	/*Dunit Control High Register */
	{0x0000142C, 0x214C2F38}, 	/*Dunit Control High Register */
	{0x0000147C, 0x0000c671},

	{0x00001484, 0x0000E907},	/* AXI/Mbus Arbiter  */
	{0x000014a8, 0x00000100}, 	/* AXI Control */

	{0x00001494, 0x00010000}, 	/*DDR SDRAM ODT Control (Low) Register */
	{0x00001498, 0x00000000}, 	/*DDR SDRAM ODT Control (High) Register */
	{0x0000149C, 0x00000001}, 	/*DDR Dunit ODT Control Register */

	{0x000014C0, 0x192424C8}, 	/* DRAM address and Control Driving Strenght  */
	{0x000014C4, 0xEFB24C8}, 	/* DRAM Data and DQS Driving Strenght  */

	{0x000200e8, 0x1FFF0E01}, 	/*Open Mbus Window - 2G */
	{0x00020184, 0x1FFFFFE0}, 	/*Close fast path Window to - 2G */

	{0x00001504, 0x1FFFFFF1}, 	/*Dummy */
	{0x0000150C, 0x00000000}, 	/*Close all 15xx registers they are not used */
	{0x00001514, 0x00000000}, 	/*Close all 15xx registers they are not used */
	{0x0000151C, 0x00000000}, 	/*Close all 15xx registers they are not used */

	{0x00001538, 0x0000000b}, 	/*Read Data Sample Delays Register */
	{0x0000153C, 0x0000000d}, 	/*Read Data Ready Delay Register */

	{0x000015D0, 0x00000640}, 	/*MR0 */
	{0x000015D4, 0x00000046}, 	/*MR1 */
	{0x000015D8, 0x00000010}, 	/*MR2 */
	{0x000015DC, 0x00000000}, 	/*MR3 */

	{0x000015E4, 0x00203c18}, 	/*ZQC Configuration Register */
	{0x000015EC, 0xDE000025}, 	/*DDR PHY */
	{0x0, 0x0}
};

#endif /* __A370_MC_STATIC_H */
