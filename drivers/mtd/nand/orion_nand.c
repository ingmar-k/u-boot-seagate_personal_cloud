/*
 * (C) Copyright 2012
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <nand.h>
#include <malloc.h>
#include <asm/io.h>

#define NF_CLE			0x01	/* Commands with A[1:0] == 01 */
#define NF_ALE			0x02	/* Addresses with A[1:0] == 10 */

#define NF_CTRL			(u32*)(ORION5X_REGS_PHY_BASE + 0x104e8)
#define NF_CTRL_NF0		0x00000004
#define NF_CTRL_NFACTCEN0	0x00000008

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND_BASE }
#endif

#ifndef CONFIG_SYS_NAND_OPTIONS
#define CONFIG_SYS_NAND_OPTIONS	(NAND_COPYBACK|NAND_CACHEPRG|NAND_NO_PADDING)
#endif

#ifndef CONFIG_SYS_NAND_DELAY
#define CONFIG_SYS_NAND_DELAY	25	/* 25 micro seconds */
#endif

static void nand_cmd_ctrl (struct mtd_info *mtd, int cmd, unsigned ctrl)
{
	struct nand_chip *nc = mtd->priv;
	u32 offs;

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		offs = NF_CLE;
	else if (ctrl & NAND_ALE)
		offs = NF_ALE;
	else
		return;

	if (nc->options & NAND_BUSWIDTH_16)
		offs <<= 1;

	writeb(cmd, nc->IO_ADDR_W + offs);
}

static void nand_select_chip (struct mtd_info *mtd, int chip)
{
	u32 data;

	data = readl(NF_CTRL);
	data |= NF_CTRL_NF0 | NF_CTRL_NFACTCEN0;
	writel(data, NF_CTRL);
}

void board_nand_init (void)
{
	ulong io_addr[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_BASE_LIST;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	int i;

	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
		chip = malloc(sizeof(*chip));
		if (!chip) {
			printf("alloc nand resource failed\n");
			return;
		}

		memset(chip, 0, sizeof(*chip));
		mtd = &nand_info[i];
		mtd->priv = chip;
		chip->IO_ADDR_R = chip->IO_ADDR_W = (void __iomem *)io_addr[i];

		chip->options = CONFIG_SYS_NAND_OPTIONS;
#if defined(CONFIG_NAND_ECC_BCH)
		chip->ecc.mode = NAND_ECC_SOFT_BCH;
#else
		chip->ecc.mode = NAND_ECC_SOFT;
#endif
		chip->cmd_ctrl = nand_cmd_ctrl;
		chip->chip_delay = CONFIG_SYS_NAND_DELAY;
		chip->select_chip = nand_select_chip;

		if (nand_scan(mtd, i)) {
			free(chip);
			continue;
		}

		nand_register(i);
	}
}
