/*
 * Copyright (C) Marvell International Ltd. and its affiliates
 *
 * SPDX-License-Identifier:	GPL-2.0
 *
 * Taken from u-boot-armada-2011.12
 */

#ifndef __A370_TRAINING_STATIC_H
#define __A370_TRAINING_STATIC_H

/*
 * STATIC_TRAINING - Set only if static parameters for training are set and
 * required
 */
/*#define STATIC_TRAINING */

/* DDR3 Training static parameters - relevant only if STATIC_TRAINING is defined */
MV_DRAM_TRAINING_INIT ddr3_db_400[MV_MAX_DDR3_STATIC_SIZE] =
{
	/* Read Leveling */
	/*PUP	RdSampleDly (+CL)	Phase	RL ADLL value */
	/*0		2		3		1 */
	{0x000016A0, 0xC0020015},
	/*1		2		2		6 */
	{0x000016A0, 0xC0420019},

	/* Write Leveling */
	/*0 */
	{0x000016A0, 0xC0006C0E},
	/*1 */
	{0x000016A0, 0xC0409418},

	/*center DQS on read cycle */
	{0x000016A0, 0xC803000F},

	{0x00001538, 0x00000007}, 	/*Read Data Sample Delays Register */
	{0x0000153C, 0x00000009}, 	/*Read Data Ready Delay Register */
	/*init DRAM */
	{0x00001480, 0x00000001},
	{0x0, 0x0}
};

/* DDR3 Training static parameters - relevant only if STATIC_TRAINING is defined */
MV_DRAM_TRAINING_INIT ddr3_db_600[MV_MAX_DDR3_STATIC_SIZE] =
{
	/* Read Leveling */
	/*PUP	RdSampleDly (+CL)	Phase	RL ADLL value */
	/*0		2		3		1 */
	{0x000016A0, 0xC0020119},
	/*1		2		2		6 */
	{0x000016A0, 0xC042011E},

	/* Write Leveling */
	/*0 */
	{0x000016A0, 0xC0005508},
	/*1 */
	{0x000016A0, 0xC0409117},

	/*center DQS on read cycle */
	{0x000016A0, 0xC803000F},

	{0x00001538, 0x0000000B}, 	/*Read Data Sample Delays Register */
	{0x0000153C, 0x0000000F}, 	/*Read Data Ready Delay Register */
	/*init DRAM */
	{0x00001480, 0x00000001},
	{0x0, 0x0}
};

/* DDR3 Training static parameters - relevant only if STATIC_TRAINING is defined */
MV_DRAM_TRAINING_INIT ddr3_pcac_600[MV_MAX_DDR3_STATIC_SIZE] =
{
	/* Read Leveling */
	/*PUP	RdSampleDly (+CL)	Phase	RL ADLL value */
	/*0		2		3		1 */
	{0x000016A0, 0xC0020006},
	/*1		2		2		6 */
 	{0x000016A0, 0xC0420009},

	/* Write Leveling */
	/*0 */
	{0x000016A0, 0xC0005809},
	/*1 */
	{0x000016A0, 0xC040680D},

	/*center DQS on read cycle */
	{0x000016A0, 0xC803000F},

	{0x00001538, 0x0000000A}, 	/*Read Data Sample Delays Register */
	{0x0000153C, 0x0000000C}, 	/*Read Data Ready Delay Register */
	/*init DRAM */
	{0x00001480, 0x00000001},
	{0x0, 0x0}
};

#endif /* __A370_TRAINING_STATIC_H */
