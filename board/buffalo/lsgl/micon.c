/*
 * (C) Copyright 2012
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <ns16550.h>
#include <linux/ctype.h>
#include "micon.h"

#define MICON			((NS16550_t)ORION5X_UART1_BASE)
#define MICON_DIV		(CONFIG_SYS_NS16550_CLK / 16 / 38400)
#define MICON_LCR		(UART_LCR_WLS_8 | \
				 UART_LCR_PEN | UART_LCR_EPS | \
				 UART_LCR_STB)

#define MICON_TIMEOUT		100
#define MICON_MIN		4
#define MICON_MAX		35

#define MICON_READ		0x80
#define MICON_LEN(c)		(((c) & 0x7f) + 3)

#define MICON_HDR		0
#define MICON_CMD		1
#define MICON_DATA		2

#define MICON_NOP		0xff

#define MICON_BOOT_START	0x02
#define MICON_BOOT_END		0x03
#define MICON_DRAM_DATA_ERROR	0x04
#define MICON_DRAM_ADDR_ERROR	0x05
#define MICON_POWER_OFF		0x06
#define MICON_DRAM_TEST		0x08
#define MICON_SHUT_DOWN		0x0c
#define MICON_SHUT_CANCEL	0x0d
#define MICON_REBOOT		0x0e
#define MICON_PHY_ERROR		0x28
#define MICON_MOUNT_ERROR	0x29
#define MICON_SYSTEM_ERROR	0x2a

#define MICON_BUZ		0x30
#define MICON_FAN		0x33
#define MICON_WDT		0x35
#define MICON_SW		0x36
#define MICON_TEMP		0x37
#define MICON_SPIN		0x38
#define MICON_BRIGHT		0x3a
#define MICON_HDD		0x3b
#define MICON_STATUS		0x3c
#define MICON_MODE		0x3e

#define MICON_CTRL		0x50
#define MICON_LED_ON		0x51	/* Power and Link LEDs */
#define MICON_LED_BLINK		0x52	/* Power and Link LEDs */
#define MICON_BUZ_FREQ		0x53
#define MICON_LED		0x54	/* Info and Error LEDs */

#define MICON_BUZ_SILENT	0x00
#define MICON_BUZ_BOOT		0x01
#define MICON_BUZ_ALARM		0x10

#define MICON_FAN_STOP		0x00
#define MICON_FAN_LOW		0x01
#define MICON_FAN_MID		0x02
#define MICON_FAN_HIGH		0x03

#define MICON_LED_INFO_OFF	0x00
#define MICON_LED_ERR_OFF	0x00
#define MICON_LED_INFO_WARN	0x00
#define MICON_LED_ERR_WARN	0x06

static void micon_putc (char c)
{
	NS16550_putc(MICON, c);
}

static int micon_getc (void)
{
	int i;

	for (i = 0; i < MICON_TIMEOUT; i++, mdelay(1))
		if (NS16550_tstc(MICON))
			break;

	return NS16550_tstc(MICON) ? NS16550_getc(MICON) : -1;
}

static void micon_drain (void)
{
	while (micon_getc() > 0);
}

static void micon_tx (int len, char* buf)
{
	u8 sum;
	int i;

	for (sum = 0, i = 0; i < len; sum += buf[i++])
		micon_putc(buf[i]);
	NS16550_putc(MICON, -sum);
	mdelay(1);
}

static int micon_rx (int len, char* buf)
{
	u8 sum;
	int tmp, i;

	if (len < MICON_MIN)
		return -1;

	if ((tmp = micon_getc()) < 0)
		return -1;

	if (MICON_LEN(tmp) > len || MICON_LEN(tmp) > MICON_MAX)
		goto cleanup;

	sum = buf[0] = tmp & 0xff;
	for (i = 1, len = MICON_LEN(tmp); i < len; i++)
		if ((tmp = micon_getc()) < 0)
			goto cleanup;
		else
			sum += buf[i] = tmp & 0xff;

	if (sum)
		goto cleanup;

	return len;

cleanup:
	micon_drain();
	return -1;
}

static void micon_sync (void)
{
	int i;

	for (i = 0; i < MICON_MAX; i++)
		micon_putc(MICON_NOP);
	micon_drain();
}

static void micon_cmd0 (bool read, char cmd)
{
	char tx[2] = {'\x00'};
	char rx[MICON_MAX];

	if (read)
		tx[MICON_HDR] |= MICON_READ;
	tx[MICON_CMD] = cmd;
	micon_tx(sizeof(tx), tx);
	if (!read)
		micon_rx(sizeof(rx), rx);
}

static void micon_cmd1 (bool read, char cmd, char arg)
{
	char tx[3] = {'\x01'};
	char rx[MICON_MAX];

	if (read)
		tx[MICON_HDR] |= MICON_READ;
	tx[MICON_CMD] = cmd;
	tx[MICON_DATA] = arg;
	micon_tx(sizeof(tx), tx);
	if (!read)
		micon_rx(sizeof(rx), rx);
}

static void micon_cmd2 (bool read, char cmd, char arg0, char arg1)
{
	char tx[4] = {'\x02'};
	char rx[MICON_MAX];

	if (read)
		tx[MICON_HDR] |= MICON_READ;
	tx[MICON_CMD] = cmd;
	tx[MICON_DATA] = arg0;
	tx[MICON_DATA + 1] = arg1;
	micon_tx(sizeof(tx), tx);
	if (!read)
		micon_rx(sizeof(rx), rx);
}

static void micon_shut (void)
{
	micon_cmd0(false, MICON_SHUT_DOWN);
	micon_cmd1(false, MICON_BUZ, MICON_BUZ_SILENT);
	micon_cmd2(false, MICON_LED, MICON_LED_INFO_OFF, MICON_LED_ERR_OFF);
}

void micon_init (void)
{
	MICON->ier = 0;
	MICON->lcr = UART_LCR_DLAB | MICON_LCR;
	MICON->dll = MICON_DIV & 0xff;
	MICON->dlm = (MICON_DIV >> 8) & 0xff;
	mdelay(1);
	MICON->lcr = MICON_LCR;
	MICON->mcr = UART_MCR_DTR | UART_MCR_RTS;
	MICON->fcr = UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR;

	micon_sync();
}

void micon_boot (void)
{
	micon_cmd0(false, MICON_BOOT_START);
	micon_cmd1(false, MICON_BUZ, MICON_BUZ_BOOT);
}

int micon_buttons (void)
{
	char rx[MICON_MAX];
	int len;

	micon_cmd0(true, MICON_SW);

	len = micon_rx(sizeof(rx), rx);
	if (len < 0) {
		micon_sync();
		return 0;
	}

	return rx[MICON_DATA];
}

bool micon_emergency (void)
{
	char cmd[] = {MICON_READ, MICON_MODE};
	char ack[MICON_MAX];

	micon_tx(sizeof(cmd), cmd);
	if (micon_rx(sizeof(ack), ack) < 0)
		return 0;

	return ack[MICON_DATA];
}

void micon_hdd (bool on)
{
	micon_cmd1(false, MICON_HDD, on);
}

void micon_error (void)
{
	micon_cmd1(false, MICON_BUZ, MICON_BUZ_ALARM);
	micon_cmd2(false, MICON_LED, MICON_LED_INFO_WARN, MICON_LED_ERR_WARN);
}

void micon_off (void)
{
	micon_shut();
	micon_cmd0(false, MICON_POWER_OFF);
}

void micon_reboot (void)
{
	micon_shut();
	micon_cmd0(false, MICON_REBOOT);
}

void micon_up (void)
{
	micon_cmd0(false, MICON_BOOT_END);
	micon_cmd1(false, MICON_BUZ, MICON_BUZ_SILENT);
	micon_cmd2(false, MICON_LED, MICON_LED_INFO_OFF, MICON_LED_ERR_OFF);
	micon_cmd1(false, MICON_FAN, MICON_FAN_LOW);
}

static int do_micon (cmd_tbl_t *cmdtp, int flag, int argc, char* const argv[])
{
	char tx[MICON_MAX];
	char rx[MICON_MAX];
	int i, button, on, len = 0;

	if (argc < 2 || argc > MICON_MAX + 1)
		return CMD_RET_USAGE;

	if (!strcasecmp(argv[1], "button")) {
		button = micon_buttons();
		printf("power: %s\ninit: %s\n",
			MICON_BUTTON_POWER(button) ? "on" : "off",
			MICON_BUTTON_INIT(button) ? "on" : "off");
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "error")) {
		micon_error();
		return CMD_RET_SUCCESS;
	} else if (argc == 3 && !strcasecmp(argv[1], "hdd")) {
		if (!strcasecmp(argv[2], "on"))
			on = 1;
		else if (!strcasecmp(argv[2], "off"))
			on = 0;
		else
			return CMD_RET_USAGE;
		micon_hdd(on);
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "mode")) {
		printf("mode: %s\n",
			micon_emergency() ? "emergency" : "normal");
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "off")) {
		micon_off();
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "reboot")) {
		micon_reboot();
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "sync")) {
		micon_sync();
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "up")) {
		micon_up();
		return CMD_RET_SUCCESS;
	} else if (argc > 2 && isxdigit(argv[1][0]) &&
		   (argv[1][1] == '\0' || isxdigit(argv[1][1]))) {
		for (i = 0; i < argc + 1; i++)
			tx[i] = simple_strtoul(argv[i + 1], NULL, 16);
		micon_tx(argc - 1, tx);
		len = micon_rx(sizeof(rx), rx);
		if (len < 0) {
			micon_sync();
			printf("communication error\n");
			return CMD_RET_FAILURE;
		}
		printf("reply:");
	} else
		return CMD_RET_USAGE;

	for (i = 0; i < len; i++)
		printf(" %02x", rx[i]);
	printf("\n");

	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(micon, MICON_MAX + 1, 0, do_micon,
	"micro-controller commands",
	"button             - show button status\n"
	"micon error              - sound alarm and flash error LED\n"
	"micon hdd <on|off>       - turn on/off hard disk power\n"
	"micon mode               - show system mode\n"
	"micon off                - power off system\n"
	"micon reboot             - reboot system\n"
	"micon sync               - synchronize\n"
	"micon up                 - turn off all warning and turn on fan\n"
	"micon <hex> ...          - issue command with raw byte stream");
