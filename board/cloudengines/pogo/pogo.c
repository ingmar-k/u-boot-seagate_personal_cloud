/*
 * (C) Copyright 2013, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <console.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gpio.h>
#include <netdev.h>
#include <miiphy.h>
#include <usb.h>
#include "pogo.h"

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	struct kwgpio_registers *gpio0reg =
		(struct kwgpio_registers *)MVEBU_GPIO0_BASE;
	struct kwgpio_registers *gpio1reg =
		(struct kwgpio_registers *)MVEBU_GPIO1_BASE;
	u32 *mpp_ctrl = (u32 *)KW_MPP_BASE;

	mvebu_config_gpio(KIRKWOOD_GPIO_LO_OUT_VAL, KIRKWOOD_GPIO_HI_OUT_VAL,
			KIRKWOOD_GPIO_LO_OUT_ENA, KIRKWOOD_GPIO_HI_OUT_ENA);
	writel(KIRKWOOD_GPIO_LO_BLINK_ENA, &gpio0reg->blink_en);
	writel(KIRKWOOD_GPIO_HI_BLINK_ENA, &gpio1reg->blink_en);

	writel(KIRKWOOD_MPP_0_7, &mpp_ctrl[0]);
	writel(KIRKWOOD_MPP_8_15, &mpp_ctrl[1]);
	writel(KIRKWOOD_MPP_16_23, &mpp_ctrl[2]);
	writel(KIRKWOOD_MPP_24_31, &mpp_ctrl[3]);
	writel(KIRKWOOD_MPP_32_39, &mpp_ctrl[4]);
	writel(KIRKWOOD_MPP_40_47, &mpp_ctrl[5]);
	writel(KIRKWOOD_MPP_48_49, &mpp_ctrl[6]);

#if defined(CONFIG_POGO_EJECT_BUTTON)
	kw_gpio_set_valid(CONFIG_POGO_EJECT_BUTTON, GPIO_INPUT_OK);
	kw_gpio_direction_input(CONFIG_POGO_EJECT_BUTTON);
#endif

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + CONFIG_SYS_PARAMS_PHYS;

	return 0;
}

/* don't use netconsole until eth initialized */
int overwrite_console (void)
{
	return 1;
}

int board_late_init (void)
{
	char *ethaddr = getenv("ethaddr");

	if (!ethaddr)
		setenv("ethaddr", __stringify(CONFIG_ETHADDR));

	return 0;
}

int last_stage_init (void)
{
#ifdef CONFIG_NETCONSOLE
	bool nc = false;
	char *stdinname = getenv("stdin");
	char *stdoutname = getenv("stdout");
	char *stderrname = getenv("stderr");
	struct eth_device *eth;
	u16 phyaddr;

	if (!strcmp(stdinname, "nc") || !strcmp(stdoutname, "nc") ||
	    !strcmp(stdoutname, "nc"))
		nc = true;

#ifdef CONFIG_POGO_EJECT_BUTTON
	/* swap nc and serial if init switch pressed */
	if (!kw_gpio_get_value(CONFIG_POGO_EJECT_BUTTON)) {
		if (!nc) {
			stdinname = "nc";
			stdoutname = "nc";
			stderrname = "nc";
		} else {
			stdinname = "serial";
			stdoutname = "serial";
			stderrname = "serial";
		}
		nc = !nc;
	}
#endif

	if (nc) {
		eth_init();

		/* switch to serial if not linked up */
		eth = eth_get_dev();
		if (!eth || miiphy_read(eth->name, 0xEE, 0xEE, &phyaddr) ||
		    !miiphy_link(eth->name, phyaddr)) {
			nc = false;
			stdinname = "serial";
			stdoutname = "serial";
			stderrname = "serial";
		}
	}

	console_assign(stdin, stdinname);
	console_assign(stdout, stdoutname);
	console_assign(stderr, stderrname);

	if (nc)
		net_start_again();
#endif

	return 0;
}

void enable_caches (void)
{
	icache_enable();
	dcache_disable();
}

int board_eth_init (bd_t *bis)
{
	if (mvgbe_initialize(bis))
		return -1;

	/* workaround RAMBOOT netconsole issue loaded from CE U-Boot */
	eth_halt();

	return 0;
}

/* Configure and enable MV88E1116 PHY */
void reset_phy(void)
{
	struct eth_device *eth;
	u16 phyaddr;
	u16 reg;

	eth = eth_get_dev();
	if (!eth) {
		printf("%s: can't get ETH device\n", __FUNCTION__);
		return;
	}

	if (miiphy_read(eth->name, 0xEE, 0xEE, &phyaddr)) {
		printf("%s: can't read PHY device address\n", __FUNCTION__);
		return;
	}

	/*
	 * Enable RGMII delay on Tx and Rx for CPU port
	 * Ref: sec 4.7.2 of chip datasheet
	 */
	miiphy_write(eth->name, phyaddr, MV88E1116_PGADR_REG, 2);
	miiphy_read(eth->name, phyaddr, MV88E1116_MAC_CTRL_REG, &reg);
	reg |= (MV88E1116_RGMII_RXTM_CTRL | MV88E1116_RGMII_TXTM_CTRL);
	miiphy_write(eth->name, phyaddr, MV88E1116_MAC_CTRL_REG, reg);
	miiphy_write(eth->name, phyaddr, MV88E1116_PGADR_REG, 0);

	/* reset the phy */
	miiphy_reset(eth->name, phyaddr);
}

unsigned long do_go_exec (ulong (*entry)(int, char* const[]), int argc,
				char* const argv[])
{
	eth_halt();
	usb_stop();

	cleanup_before_linux();

	return entry(argc, argv);
}
