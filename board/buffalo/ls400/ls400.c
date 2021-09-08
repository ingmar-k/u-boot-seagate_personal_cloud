/*
 * (C) Copyright 2014, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <console.h>
#include <dm.h>
#include <netdev.h>
#include <miiphy.h>
#include <pci.h>
#include <usb.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/cpu.h>
#include "../arch/arm/mach-mvebu/serdes/a370/serdes.h"

#define LS400				"ls400"

#define HD_POWERUP_DELAY		1000
#define HD_SPINUP_DELAY			10000

#define GPIO_POWER_ON			1
#define GPIO_POWER_OFF			0

#define GPIO_FAN_PIN0			13
#define GPIO_FAN_PIN1			14

#define GPIO_FAN_OFF			3
#define GPIO_FAN_LO			2
#define GPIO_FAN_MED			1
#define GPIO_FAN_HI			0

#define POWER_BUTTON_PIN		15
#define FUNCTION_BUTTON_PIN		16

#if defined(CONFIG_TARGET_LS420)
#define GPIO_USB_PWR_PIN		5
#define GPIO_HD0_PWR_PIN		8
#define GPIO_HD1_PWR_PIN		9

#define GPIO_HD0_GONE_PIN		11
#define GPIO_HD1_GONE_PIN		12
#elif defined(CONFIG_TARGET_LS440)
#define GPIO_HD0_PWR_PIN		8
#define GPIO_HD1_PWR_PIN		7
#define GPIO_HD2_PWR_PIN		52
#define GPIO_HD3_PWR_PIN		49

#define GPIO_HD0_GONE_PIN		10
#define GPIO_HD1_GONE_PIN		11
#define GPIO_HD2_GONE_PIN		50
#define GPIO_HD3_GONE_PIN		51

#define USB_HC_LENGTH(v)		((v) & 0xff)
#define USB_CMD_RESET			(1 << 1)
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_BUILD
static struct serdes_cfg ls400_serdes_cfg[A370_SERDES_MAX] = {
	{ SERDES_SEL_PCIE,	SERDES_OPT_SPD_HI },
#if defined(CONFIG_TARGET_LS420)
	{ SERDES_SEL_NONE,	SERDES_OPT_SPD_LO },
	{ SERDES_SEL_SATA,	SERDES_OPT_SPD_HI },
	{ SERDES_SEL_SATA,	SERDES_OPT_SPD_HI }
#elif defined(CONFIG_TARGET_LS440)
	{ SERDES_SEL_PCIE,	SERDES_OPT_SPD_HI },
	{ SERDES_SEL_NONE,	SERDES_OPT_SPD_LO },
	{ SERDES_SEL_NONE,	SERDES_OPT_SPD_LO }
#endif
};

struct serdes_cfg* board_serdes_cfg (void)
{
	return ls400_serdes_cfg;
}

int board_early_init_f (void)
{
	u32 out;

	/* disable clocks for unused devices */
	writel(LS400_PM_CLK_GATING_CTRL, MVEBU_REGISTER(0x18220));

	/* Disable TDM clock if TDM clock gate is disabled */
	if (!(LS400_PM_CLK_GATING_CTRL & 0x02000000))
		writel(0, MVEBU_REGISTER(0x18770));

	/* init GPIO */
	writel(LS400_GPIO_LO_IN_POL, MVEBU_GPIO0_BASE + 0x0c);
	out = LS400_GPIO_LO_OUT_VAL & ~LS400_GPIO_LO_OUT_KEEP;
	out |= readl(MVEBU_GPIO0_BASE + 0x10) & LS400_GPIO_LO_OUT_KEEP;
	writel(out, MVEBU_GPIO0_BASE + 0x00);
	writel(LS400_GPIO_LO_OUT_ENA, MVEBU_GPIO0_BASE + 0x04);
	writel(LS400_GPIO_LO_BLINK_SEL, MVEBU_GPIO0_BASE + 0x20);
	writel(LS400_GPIO_LO_BLINK_ENA, MVEBU_GPIO0_BASE + 0x08);

	writel(LS400_GPIO_MID_IN_POL, MVEBU_GPIO1_BASE + 0x0c);
#if defined(CONFIG_TARGET_LS420)
	out = LS400_GPIO_MID_OUT_VAL;
#elif defined(CONFIG_TARGET_LS440)
	out = LS400_GPIO_MID_OUT_VAL & ~LS400_GPIO_MID_OUT_KEEP;
	out |= readl(MVEBU_GPIO1_BASE + 0x10) & LS400_GPIO_MID_OUT_KEEP;
#endif
	writel(out, MVEBU_GPIO1_BASE + 0x00);
	writel(LS400_GPIO_MID_OUT_ENA, MVEBU_GPIO1_BASE + 0x04);
	writel(LS400_GPIO_MID_BLINK_SEL, MVEBU_GPIO1_BASE + 0x20);
	writel(LS400_GPIO_MID_BLINK_ENA, MVEBU_GPIO1_BASE + 0x08);

	writel(LS400_GPIO_HI_IN_POL, MVEBU_GPIO2_BASE + 0x0c);
	writel(LS400_GPIO_HI_OUT_VAL, MVEBU_GPIO2_BASE + 0x00);
	writel(LS400_GPIO_HI_OUT_ENA, MVEBU_GPIO2_BASE + 0x04);
	writel(LS400_GPIO_HI_BLINK_SEL, MVEBU_GPIO2_BASE + 0x20);
	writel(LS400_GPIO_HI_BLINK_ENA, MVEBU_GPIO2_BASE + 0x08);

	/* init MPP */
	writel(LS400_MPP_0_7, MVEBU_MPP_BASE + 0x00);
	writel(LS400_MPP_8_15, MVEBU_MPP_BASE + 0x04);
	writel(LS400_MPP_16_23, MVEBU_MPP_BASE + 0x08);
	writel(LS400_MPP_24_31, MVEBU_MPP_BASE + 0x0c);
	writel(LS400_MPP_32_39, MVEBU_MPP_BASE + 0x10);
	writel(LS400_MPP_40_47, MVEBU_MPP_BASE + 0x14);
	writel(LS400_MPP_48_55, MVEBU_MPP_BASE + 0x18);
	writel(LS400_MPP_56_63, MVEBU_MPP_BASE + 0x1c);
	writel(LS400_MPP_64_65, MVEBU_MPP_BASE + 0x20);

	return 0;
}
#else
int board_early_init_f (void)
{
	return 0;
}

int board_init (void)
{
	int rc;
	int val;
	int hd0, hd1;
#if defined(CONFIG_TARGET_LS440)
	int hd2, hd3;
#endif

#if defined(CONFIG_TARGET_LS420)
	/* power up USB */
	rc = gpio_request(GPIO_USB_PWR_PIN, LS400);
	if (rc) {
		printf("%s: can't get USB power pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_output(GPIO_USB_PWR_PIN, GPIO_POWER_ON);
#endif

	/* keep the current power state for hard disk 1 */
	rc = gpio_request(GPIO_HD0_PWR_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD0 power pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD0_PWR_PIN);
	val = gpio_get_value(GPIO_HD0_PWR_PIN);
	gpio_direction_output(GPIO_HD0_PWR_PIN, val);

	/* keep the current power state for hard disk 2 */
	rc = gpio_request(GPIO_HD1_PWR_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD1 power pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD1_PWR_PIN);
	val = gpio_get_value(GPIO_HD1_PWR_PIN);
	gpio_direction_output(GPIO_HD1_PWR_PIN, val);

#if defined(CONFIG_TARGET_LS440)
	/* keep the current power state for hard disk 3 */
	rc = gpio_request(GPIO_HD2_PWR_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD2 power pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD2_PWR_PIN);
	val = gpio_get_value(GPIO_HD2_PWR_PIN);
	gpio_direction_output(GPIO_HD2_PWR_PIN, val);

	/* keep the current power state for hard disk 4 */
	rc = gpio_request(GPIO_HD3_PWR_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD3 power pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD3_PWR_PIN);
	val = gpio_get_value(GPIO_HD3_PWR_PIN);
	gpio_direction_output(GPIO_HD3_PWR_PIN, val);
#endif

	rc = gpio_request(GPIO_HD0_GONE_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD0 missing pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD0_GONE_PIN);

	rc = gpio_request(GPIO_HD1_GONE_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD1 missing pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD1_GONE_PIN);

#if defined(CONFIG_TARGET_LS440)
	rc = gpio_request(GPIO_HD2_GONE_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD2 missing pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD2_GONE_PIN);

	rc = gpio_request(GPIO_HD3_GONE_PIN, LS400);
	if (rc) {
		printf("%s: can't get HD3 missing pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(GPIO_HD3_GONE_PIN);
#endif

	rc = gpio_request(GPIO_FAN_PIN0, LS400);
	if (rc) {
		printf("%s: can't get FAN pin 0\n", __FUNCTION__);
		return rc;
	}
	rc = gpio_request(GPIO_FAN_PIN1, LS400);
	if (rc) {
		printf("%s: can't get FAN pin 1\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_output(GPIO_FAN_PIN0, GPIO_FAN_MED & 1);
	gpio_direction_output(GPIO_FAN_PIN1, GPIO_FAN_MED & 2);

	rc = gpio_request(POWER_BUTTON_PIN, LS400);
	if (rc) {
		printf("%s: can't get power button pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(POWER_BUTTON_PIN);

	rc = gpio_request(FUNCTION_BUTTON_PIN, LS400);
	if (rc) {
		printf("%s: can't get function button pin\n", __FUNCTION__);
		return rc;
	}
	gpio_direction_input(FUNCTION_BUTTON_PIN);

	/* power on the hard disk if present otherwise power off */
	hd0 = !gpio_get_value(GPIO_HD0_GONE_PIN);
	hd1 = !gpio_get_value(GPIO_HD1_GONE_PIN);
#if defined(CONFIG_TARGET_LS440)
	hd2 = !gpio_get_value(GPIO_HD2_GONE_PIN);
	hd3 = !gpio_get_value(GPIO_HD3_GONE_PIN);
#endif
	if (hd0)
		gpio_set_value(GPIO_HD0_PWR_PIN, GPIO_POWER_ON);
	else
		gpio_set_value(GPIO_HD0_PWR_PIN, GPIO_POWER_OFF);
	if (hd0 && hd1)
		mdelay(HD_POWERUP_DELAY);
	if (hd1)
		gpio_set_value(GPIO_HD1_PWR_PIN, GPIO_POWER_ON);
	else
		gpio_set_value(GPIO_HD1_PWR_PIN, GPIO_POWER_OFF);
#if defined(CONFIG_TARGET_LS420)
	if (hd0 || hd1)
		mdelay(HD_SPINUP_DELAY);
#elif defined(CONFIG_TARGET_LS440)
	if ((hd0 || hd1) && hd2)
		mdelay(HD_POWERUP_DELAY);
	if (hd2)
		gpio_set_value(GPIO_HD2_PWR_PIN, GPIO_POWER_ON);
	else
		gpio_set_value(GPIO_HD2_PWR_PIN, GPIO_POWER_OFF);
	if ((hd0 || hd1 || hd2) && hd3)
		mdelay(HD_POWERUP_DELAY);
	if (hd3)
		gpio_set_value(GPIO_HD3_PWR_PIN, GPIO_POWER_ON);
	else
		gpio_set_value(GPIO_HD3_PWR_PIN, GPIO_POWER_OFF);
	if (hd0 || hd1 || hd2 || hd3)
		mdelay(HD_SPINUP_DELAY);
#endif

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
	char *ethaddr = getenv("eth1addr");

	if (!ethaddr) {
		ethaddr = getenv("ethaddr");
		if (!ethaddr)
			ethaddr = __stringify(CONFIG_ETHADDR);
	}
	setenv("ethaddr", ethaddr);

	return 0;
}

int last_stage_init (void)
{
#if defined(CONFIG_TARGET_LS440) && defined(CONFIG_PCI) && defined(CONFIG_USB_EHCI)
	struct pci_device_id xhci_pci_ids[] = {
		{0x1912, 0x0015},
		{}
	};
	pci_dev_t pdev;
	u8 *hccr, *hcor;
#endif
#ifdef CONFIG_NETCONSOLE
	bool nc = false;
	char *stdinname = getenv("stdin");
	char *stdoutname = getenv("stdout");
	char *stderrname = getenv("stderr");
	struct udevice *eth;
	u16 phyaddr;
#endif

#if defined(CONFIG_TARGET_LS440) && defined(CONFIG_PCI) && defined(CONFIG_USB_EHCI)
	/*
	 * It seems EHCI port is powered only after XHCI reset!
	 */
	pdev = pci_find_devices(xhci_pci_ids, 0);;
	if (pdev > 0) {
		hccr = pci_map_bar(pdev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
		hcor = hccr + USB_HC_LENGTH(readl(hccr));
		setbits_le32(hcor, USB_CMD_RESET);
	}
#endif

#ifdef CONFIG_NETCONSOLE
	if (!strcmp(stdinname, "nc") || !strcmp(stdoutname, "nc") ||
	    !strcmp(stdoutname, "nc"))
		nc = true;

	/* swap nc and serial if function switch pressed */
	if (!gpio_get_value(FUNCTION_BUTTON_PIN)) {
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

void reset_phy (void)
{
	struct udevice *eth;
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

	miiphy_write(eth->name, phyaddr, 22, 3);
	/* set status[0] polarity to high active */
	miiphy_read(eth->name, phyaddr, 17, &reg);
	reg &= ~0x000f;
	reg |= 0x0001;
	miiphy_write(eth->name, phyaddr, 17, reg);
	/* force INIT active */
	miiphy_read(eth->name, phyaddr, 16, &reg);
	reg &= ~0x0fff;
	reg |= 0x091e;
	miiphy_write(eth->name, phyaddr, 16, reg);
	miiphy_write(eth->name, phyaddr, 22, 0);

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

void reset_misc (void)
{
	struct udevice *eth;
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

	miiphy_write(eth->name, phyaddr, 0x16, 0x0003);
	miiphy_read(eth->name, phyaddr, 0x10, &reg);
	reg &= ~0x0f00;
	reg |= 0x0900;
	miiphy_write(eth->name, phyaddr, 0x10, reg);
	miiphy_write(eth->name, phyaddr, 0x16, 0x0000);

	mdelay(10);
}

int do_poweroff (cmd_tbl_t *cmdtp, int flag, int argc,
			char* const argv[])
{
	struct udevice *eth;
	u16 phyaddr;
	u16 reg;

	eth = eth_get_dev();
	if (!eth) {
		printf("%s: can't get ETH device\n", __FUNCTION__);
		return -1;
	}

	if (miiphy_read(eth->name, 0xEE, 0xEE, &phyaddr)) {
		printf("%s: can't read PHY device address\n", __FUNCTION__);
		return -1;
	}

	miiphy_write(eth->name, phyaddr, 0x16, 0x0003);
	miiphy_read(eth->name, phyaddr, 0x10, &reg);
	reg &= ~0x0f00;
	reg |= 0x0800;
	miiphy_write(eth->name, phyaddr, 0x10, reg);
	miiphy_write(eth->name, phyaddr, 0x16, 0x0000);

	mdelay(10);

	reset_cpu(0);

	return CMD_RET_SUCCESS;
}

static int do_power (cmd_tbl_t *cmdtp, int flag, int argc, char* const argv[])
{
	if (argc == 3) {
		u32 pin, val;

		if (!strcasecmp(argv[1], "on"))
			val = GPIO_POWER_ON;
		else if (!strcasecmp(argv[1], "off"))
			val = GPIO_POWER_OFF;
		else
			return CMD_RET_USAGE;

		if (!strcasecmp(argv[2], "hd1"))
			pin = GPIO_HD0_PWR_PIN;
		else if (!strcasecmp(argv[2], "hd2"))
			pin = GPIO_HD1_PWR_PIN;
#if defined(CONFIG_TARGET_LS420)
		else if (!strcasecmp(argv[2], "usb"))
			pin = GPIO_USB_PWR_PIN;
#elif defined(CONFIG_TARGET_LS440)
		else if (!strcasecmp(argv[2], "hd3"))
			pin = GPIO_HD2_PWR_PIN;
		else if (!strcasecmp(argv[2], "hd4"))
			pin = GPIO_HD3_PWR_PIN;
#endif
		else
			return CMD_RET_USAGE;

		gpio_set_value(pin, val);

		return CMD_RET_SUCCESS;
	}

	return CMD_RET_USAGE;
}

#if defined(CONFIG_TARGET_LS420)
U_BOOT_CMD(power, 3, 0, do_power, "power on/off disk/USB",
	"<on|off> hd1    - turn on/off hard disk 1 power\n"
	"power <on|off> hd2    - turn on/off hard disk 2 power\n"
	"power <on|off> usb    - turn on/off USB power");
#elif defined(CONFIG_TARGET_LS440)
U_BOOT_CMD(power, 3, 0, do_power, "power on/off disk/USB",
	"<on|off> hd1    - turn on/off hard disk 1 power\n"
	"power <on|off> hd2    - turn on/off hard disk 2 power\n"
	"power <on|off> hd3    - turn on/off hard disk 3 power\n"
	"power <on|off> hd4    - turn on/off hard disk 4 power");
#endif

static int do_fan (cmd_tbl_t *cmdtp, int flag, int argc, char* const argv[])
{
	if (argc == 2) {
		u32 val;

		if (!strcasecmp(argv[1], "off"))
			val = GPIO_FAN_OFF;
		else if (!strcasecmp(argv[1], "low"))
			val = GPIO_FAN_LO;
		else if (!strcasecmp(argv[1], "med"))
			val = GPIO_FAN_MED;
		else if (!strcasecmp(argv[1], "high"))
			val = GPIO_FAN_HI;
		else
			return CMD_RET_USAGE;

		gpio_set_value(GPIO_FAN_PIN0, val & 1);
		gpio_set_value(GPIO_FAN_PIN1, val & 2);

		return CMD_RET_SUCCESS;
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(fan, 2, 0, do_fan, "fan speed control",
	"off       - turn off fan\n"
	"fan low       - set low fan speed\n"
	"fan med       - set medium fan speed\n"
	"fan high      - set high fan speed");
#endif
