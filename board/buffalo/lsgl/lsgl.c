/*
 * (C) Copyright 2012, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <console.h>
#include <mtd/cfi_flash.h>
#include <netdev.h>
#include <miiphy.h>
#include <usb.h>
#include "micon.h"

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f (void)
{
	micon_init();
	micon_boot();

	return 0;
}

int board_init (void)
{
	gd->bd->bi_boot_params = gd->bd->bi_dram[0].start + CONFIG_SYS_PARAMS_PHYS;

	mdelay(500);

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
#endif

	micon_up();

#ifdef CONFIG_NETCONSOLE
	if (!strcmp(stdinname, "nc") || !strcmp(stdoutname, "nc") ||
	    !strcmp(stdoutname, "nc"))
		nc = true;

	/*
	 * swap nc and serial if init switch pressed and
	 * read twice to make sure it's really pressed
	 */
	if (MICON_BUTTON_INIT(micon_buttons()) &&
	    MICON_BUTTON_INIT(micon_buttons())) {
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

ulong board_flash_get_legacy (ulong base, int banknum, flash_info_t *info)
{
	ulong i;

	if (banknum)
		return 0;

	/* SST 39VF020 256KB (64 uniform 4KB sectors) */
	info->size = 0x00040000; /* 256K */
	info->sector_count = CONFIG_SYS_MAX_FLASH_SECT;
	info->flash_id = FLASH_MAN_CFI;
	for (i = 0; i < info->sector_count; i++) {
		info->start[i] = base + (i << 12);
		info->protect[i] = 1;
	}

	info->portwidth = FLASH_CFI_8BIT;
	info->chipwidth = FLASH_CFI_BY8;
	info->buffer_size = 0;
	info->erase_blk_tout = 25;	/* max sector erase time: 25 ms */
	info->write_tout = 1;		/* max byte program time: 20 us */
	info->buffer_write_tout = 0;
	info->vendor = CFI_CMDSET_AMD_LEGACY;
	info->cmd_reset = AMD_CMD_RESET;
	info->cmd_erase_sector = AMD_CMD_ERASE_SECTOR;
	info->interface = FLASH_CFI_X8;
	info->legacy_unlock = 0;
	info->manufacturer_id = (ushort)SST_MANUFACT;
	info->device_id = (ushort)SST_ID_xF020;
	info->device_id2 = 0;
	info->ext_addr = 0;
	info->cfi_version = 0;
	info->cfi_offset = 0;
	info->addr_unlock1 = 0x00005555;
	info->addr_unlock2 = 0x00002aaa;
	info->name = "SST 39VF020";

	return 1;
}

int board_eth_init (bd_t *bis)
{
	struct eth_device *eth;
	u16 phyaddr;
	u16 id2, id3, reg;

	if (mvgbe_initialize(bis))
		return -1;

	eth = eth_get_dev();
	if (!eth) {
		printf("%s: can't get ETH device\n", __FUNCTION__);
		return -1;
	}

	if (miiphy_read(eth->name, 0xEE, 0xEE, &phyaddr)) {
		printf("%s: can't get PHY device address\n", __FUNCTION__);
		return -1;
	}

#ifdef CONFIG_SYS_RAMBOOT
	/* workaround RAMBOOT netconsole issue loaded from BUFFALO U-Boot */
	eth_halt();
#endif

	/* read PHY Ident 2 and 3 */
	miiphy_read(eth->name, phyaddr, 2, &id2);
	miiphy_read(eth->name, phyaddr, 3, &id3);
	id3 &= 0xfff0;

	/* Marvell 88E1111 (implies Linkstation Pro v1) */
	if ((id2 == 0x0141) && (id3 == 0x0cc0)) {
		/* PHY rx and tx delay */
		miiphy_read(eth->name, phyaddr, 20, &reg);
		reg |= 0x0082;
		miiphy_write(eth->name, phyaddr, 20, reg);

		/* Link and Activity LED (derived from PHY register dump) */
		miiphy_write(eth->name, phyaddr, 24, 0x4111);

		/* SATA Port 1 for Linkstation v1 */
		ide_bus_offset[0] = ORION5X_SATA_PORT1_OFFSET;
	/* Marvell 88E1118 (implies Linkstation Pro v2 or Kurobox-Pro) */
	} else if ((id2 == 0x0141) && (id3 == 0x0e10)) {
		/* Switch to page 3 */
		miiphy_write(eth->name, phyaddr, 22, 3);

		/* Link and Activity LED (derived from PHY register dump) */
		miiphy_write(eth->name, phyaddr, 16, 0x0201);

		/* switch back to page 0 */
		miiphy_write(eth->name, phyaddr, 22, 0);
	}

	/* reset the phy */
	miiphy_reset(eth->name, phyaddr);

	return 0;
}

unsigned long do_go_exec (ulong (*entry)(int, char* const[]), int argc,
				char* const argv[])
{
	eth_halt();
	usb_stop();

	cleanup_before_linux();

	return entry(argc, argv);
}

int do_poweroff (cmd_tbl_t *cmdtp, int flag, int argc, char* const argv[])
{
	micon_off();

	return CMD_RET_SUCCESS;
}
