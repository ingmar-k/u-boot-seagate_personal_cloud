/*
 * Copyright (C) Marvell International Ltd. and its affiliates
 *
 * SPDX-License-Identifier:	GPL-2.0
 *
 * Taken from u-boot-armada-2011.12
 */

#ifndef __DDR3_A370_CONFIG_H
#define __DDR3_A370_CONFIG_H

/*
 * DDR3_LOG_LEVEL Information
 *
 * Level 0: Provides an error code in a case of failure, RL, WL errors
 *          and other algorithm failure
 * Level 1: Provides the D-Unit setup (SPD/Static configuration)
 * Level 2: Provides the windows margin as a results of DQS centeralization
 * Level 3: Provides the windows margin of each DQ as a results of DQS
 *          centeralization
 */
#ifdef CONFIG_DDR_LOG_LEVEL
#define DDR3_LOG_LEVEL	CONFIG_DDR_LOG_LEVEL
#else
#define DDR3_LOG_LEVEL  0
#endif

#define DDR3_PBS        0

/* This flag allows the execution of SW WL/RL upon HW failure */
#define DDR3_RUN_SW_WHEN_HW_FAIL	1

/*
 * General Configurations
 *
 * The following parameters are required for proper setup:
 *
 * DRAM_ECC            - Must be set to FALSE
 * DQS_CLK_ALIGNED     - Set this if CLK and DQS signals are aligned on board
 * DDR3_TRAINING_DEBUG - debug prints of internal code
 * A370_A0             - must be defined if using Marvell DB board and A370 A0 device
 *                       otherwise dont care
 */
#define DUNIT_STATIC
#define DRAM_ECC			0
#undef DQS_CLK_ALIGNED
#define DDR3_TRAINING_DEBUG		0
#undef A370_A0
#define REG_DIMM_SKIP_WL		1

/* Marvell boards specific configurations */
#if defined(DB_88F6710_PCAC)
#define STATIC_TRAINING
#endif

#if defined(DB_88F6710) && !defined(A370_A0)
#define AUTO_DETECTION_SUPPORT
#define SPD_SUPPORT
#define DRAM_2T				0x0
#define DIMM_CS_BITMAP			0xF
#define DUNIT_SPD
#undef DUNIT_STATIC
#endif

#endif /* __DDR3_A370_CONFIG_H */
