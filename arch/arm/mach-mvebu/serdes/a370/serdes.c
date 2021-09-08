/*
 * (C) Copyright 2014, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include "serdes.h"

#define SERDES_TIMEOUT			(3 * CONFIG_SYS_HZ)

#define INDEX_NONE			~0U

#define A370_PCIE_MAX			2
#define A370_SATA_MAX			2
#define A370_ETH_MAX			2

#define A370_REGISTER(r)		MVEBU_REGISTER(r)
#define A370_PCIE_BASE(p)		A370_REGISTER(0x40000 * ((p) + 1))
#define A370_SATA_BASE(p)		A370_REGISTER(0xa2000 + 0x2000 * (p))
#define A370_ETH_BASE(p)		A370_REGISTER(0x70000 + 0x4000 * (p))

/* set in Boot ROM Routine and Error Code Register 0x182d0 by DDR3 training code */
#define A370_DRAM_INIT			(1 << 12)

struct serdes_map {
	u32	index;
	u32	select;
};

static struct serdes_map mv88f67xx_map_tbl[A370_SERDES_MAX][SERDES_SEL_MAX] =
{
	/*	index		select	*/
	{
		{ INDEX_NONE,	0 },	/* SERDES_SEL_NONE */
		{ 0,		1 },	/* SERDES_SEL_PCIE */
		{ 0,		2 },	/* SERDES_SEL_SATA */
		{ 1,		3 }	/* SERDES_SEL_SGMII */
	},
	{
		{ INDEX_NONE,	0 },
		{ 1,		1 },
		{ INDEX_NONE,	0 },
		{ 0,		2 }
	},
	{
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ 0,		1 },
		{ 0,		2 }
	},
	{
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ 1,		1 },
		{ 1,		2 }
	}
};

static struct serdes_map mv88f6w11_map_tbl[A370_SERDES_MAX][SERDES_SEL_MAX] =
{
	/*	index		select	*/
	{
		{ INDEX_NONE,	0 },	/* SERDES_SEL_NONE */
		{ 0,		1 },	/* SERDES_SEL_PCIE */
		{ INDEX_NONE,	0 },	/* SERDES_SEL_SATA */
		{ 1,		3 }	/* SERDES_SEL_SGMII */
	},
	{
		{ INDEX_NONE,	0 },
		{ 1,		1 },
		{ INDEX_NONE,	0 },
		{ 0,		2 }
	},
	{
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ 0,		2 }
	},
	{
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ INDEX_NONE,	0 },
		{ 1,		2 }
	}
};

static void serdes_init_none (struct serdes_map *map, enum serdes_opt opt)
{
}

static void serdes_init_pcie (struct serdes_map *map, enum serdes_opt opt)
{
	if (map->index >= A370_PCIE_MAX)
		return;

	/* PCIe Pipe Setting Phase I */
	writel(0x00c10025, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00c3000f, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00c5011f, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00c80005, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00d00100, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00d13014, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00801000, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x00810011, A370_PCIE_BASE(map->index) + 0x1b00);

	/* Operatonal Mode and Reference Clock Setting */
	writel(0x0001fc60, A370_PCIE_BASE(map->index) + 0x1b00);

	/* Reference Clock Select Setting */
	writel(0x00460000, A370_PCIE_BASE(map->index) + 0x1b00);

	/* Impedance Tuning */
	writel(0x00489080, A370_PCIE_BASE(map->index) + 0x1b00);

	/* PCIe Pipe Setting Phase II */
	writel(0x00c10024, A370_PCIE_BASE(map->index) + 0x1b00);

	/* enable PEX */
	setbits_le32(A370_REGISTER(0x18204), 1 << map->index);

	/* fixup */
	writel(0x000f2a21, A370_PCIE_BASE(map->index) + 0x1b00);
	writel(0x004f6219, A370_PCIE_BASE(map->index) + 0x1b00);
}

static void serdes_init_sata (struct serdes_map *map, enum serdes_opt opt)
{
	unsigned long start;

	if (map->index >= A370_SATA_MAX)
		return;

	/* Operatonal Mode and Reference Clock Setting */
	writel(0x0000f801, A370_SATA_BASE(map->index) + 0x0804);

	/* Bits Decode Setting */
	writel(0x00000400, A370_SATA_BASE(map->index) + 0x088c);

	/* Reference Clock Select Setting */
	writel(0x00000400, A370_SATA_BASE(map->index) + 0x0918);

	/* Impedance Tuning */
	writel(0x00008080, A370_SATA_BASE(map->index) + 0x0920);

	/* Protocol Generation Setting */
	if (opt == SERDES_OPT_SPD_LO)
		writel(0x00000007, A370_SATA_BASE(map->index) + 0x0058);
	else
		writel(0x00000227, A370_SATA_BASE(map->index) + 0x0058);

	/* Poll for Initialization Completed */
	start = get_timer(0);
	do {
		if ((readl(A370_SATA_BASE(map->index) + 0x005c) & 7) == 7)
			break;
	} while (get_timer(start) < SERDES_TIMEOUT);

	if (get_timer(start) >= SERDES_TIMEOUT)
		printf("SATA%d SerDes initialization failed\n", map->index);

	/* fixup */
	writel(0x0000c928, A370_SATA_BASE(map->index) + 0x0834);
	writel(0x000030f0, A370_SATA_BASE(map->index) + 0x0838);
	writel(0x0000aa31, A370_SATA_BASE(map->index) + 0x083c);
	writel(0x000030f5, A370_SATA_BASE(map->index) + 0x0840);
}

static void serdes_init_sgmii (struct serdes_map *map, enum serdes_opt opt)
{
	unsigned long start;

	if (map->index >= A370_ETH_MAX)
		return;

	/* Operatonal Mode and Reference Clock Setting */
	writel(0x0000f881, A370_ETH_BASE(map->index) + 0x2e04);

	/* Bits Decode Setting */
	writel(0x00000000, A370_ETH_BASE(map->index) + 0x2e8c);

	/* Reference Clock Select Setting */
	writel(0x00000400, A370_ETH_BASE(map->index) + 0x2f18);

	/* Impedance Tuning */
	writel(0x00008080, A370_ETH_BASE(map->index) + 0x2f20);

	/* Protocol Generation Setting */
	if (opt == SERDES_OPT_SPD_HI)
		writel(0x00001107, A370_ETH_BASE(map->index) + 0x24a0);
	else
		writel(0x00000cc7, A370_ETH_BASE(map->index) + 0x24a0);

	/* Poll for Initialization Completed */
	start = get_timer(0);
	do {
		if ((readl(A370_ETH_BASE(map->index) + 0x24a4) & 7) == 7);
			break;
	} while (get_timer(start) < SERDES_TIMEOUT);

	if (get_timer(start) >= SERDES_TIMEOUT)
		printf("SGMII%d SerDes initialization failed\n", map->index);

	/* fixup */
	writel(0x0000c964, A370_ETH_BASE(map->index) + 0x2e34);
}

typedef void (*serdes_init)(struct serdes_map *map, enum serdes_opt opt);

static serdes_init init_tbl[SERDES_SEL_MAX] = {
	serdes_init_none,
	serdes_init_pcie,
	serdes_init_sata,
	serdes_init_sgmii
};

static void a370_serdes_config(struct serdes_cfg setup[A370_SERDES_MAX])
{
	u16 devid = (readl(MVEBU_REG_PCIE_DEVID) >> 16) & 0xffff;
	int i;
	struct serdes_cfg *cfg;
	struct serdes_map *map;
	u32 reg = 0;

	/* avoid re-init SerDes which often causes hang */
	if (readl(CONFIG_BOOTROM_ERR_REG) & A370_DRAM_INIT)
		return;

	/* power down PCIe */
	writel(0x00017c60, A370_PCIE_BASE(0) + 0x1b00);
	writel(0x00017c60, A370_PCIE_BASE(1) + 0x1b00);
	/* disable PCIe */
	writel(0x00000000, A370_REGISTER(0x18204));

	/* power down SATA */
	writel(0x00000220, A370_SATA_BASE(0) + 0x0058);
	writel(0x00000220, A370_SATA_BASE(1) + 0x0058);

	/* power down SGMII */
	writel(0x00000cc0, A370_ETH_BASE(0) + 0x24a0);
	writel(0x00000cc0, A370_ETH_BASE(1) + 0x24a0);

	/* configure serdes */
	for (i = 0, cfg = setup; i < A370_SERDES_MAX; i++, cfg++) {
		if (cfg->sel >= SERDES_SEL_MAX) {
			printf("Invalid SerDes%d config %d\n", i, cfg->sel);
			continue;
		}

		if (devid == SOC_88F6W11_ID)
			map = &mv88f6w11_map_tbl[i][cfg->sel];
		else
			map = &mv88f67xx_map_tbl[i][cfg->sel];

		if (map->index == INDEX_NONE) {
			if (cfg->sel != SERDES_SEL_NONE)
				printf("Invalid SerDes%d config %d\n", i, cfg->sel);
			continue;
		}
		reg |= map->select << ( i * 4);
	}
	writel(reg, A370_REGISTER(0x18270));

	/* initialize serdes */
	for (i = 0, cfg = setup; i < A370_SERDES_MAX; i++, cfg++) {
		if (cfg->sel >= SERDES_SEL_MAX)
			continue;

		if (devid == SOC_88F6W11_ID)
			map = &mv88f6w11_map_tbl[i][cfg->sel];
		else
			map = &mv88f67xx_map_tbl[i][cfg->sel];

		if (map->index != INDEX_NONE)
			init_tbl[cfg->sel](map, cfg->opt);
	}
}

__weak struct serdes_cfg* board_serdes_cfg (void)
{
	return NULL;
}

int serdes_phy_config (void)
{
	struct serdes_cfg *cfg = board_serdes_cfg();

	if (cfg)
		a370_serdes_config(cfg);

	return 0;
}
