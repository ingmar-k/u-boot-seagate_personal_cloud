/*
 * (C) Copyright 2012, 2016
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _CONFIG_LSGL_H
#define _CONFIG_LSGL_H

/*
 * Boot type
 */
#ifdef CONFIG_SYS_RAMBOOT
#define IDENT_SUFFIX			" (RAMBOOT)"
#else
#define IDENT_SUFFIX			""
#endif

/*
 * Board specific information
 */
#ifdef CONFIG_TARGET_KUROPRO
#define CONFIG_IDENT_STRING		" Kurobox-Pro" IDENT_SUFFIX
#define CONFIG_MACH_KUROBOX_PRO
#define CONFIG_MACH_TYPE		MACH_TYPE_KUROBOX_PRO
#define CONFIG_CMD_NAND
#elif defined(CONFIG_TARGET_LSGL)
#define CONFIG_IDENT_STRING		" Linkstation Pro/Live" IDENT_SUFFIX
#define CONFIG_MACH_LINKSTATION_PRO
#define CONFIG_MACH_TYPE		MACH_TYPE_LINKSTATION_PRO
#endif

/*
 * Board-specific values for Orion5x MPP low level init:
 * - MPPs 2 and 3 are GPIO inputs from Micon and RTC
 * - MPPs 6 and 7 are NAND REn and WEn
 * - MPPs 12 to 15 are SATA presence and LEDs (mode 5)
 * - MPPs 16 to 19 are UART1 RXD, TXD, CTS, and RTS with Micon
 */
#define ORION5X_MPP0_7			0x44000003
#define ORION5X_MPP8_15			0x55550000
#define ORION5X_MPP16_23		0x00000000

/*
 * Board-specific values for Orion5x GPIO low level init:
 * - GPIO2 is input Micon interrupt
 * - GPIO3 is input RTC interrupt
 */
#define ORION5X_GPIO_OUT_VALUE		0x00000000
#define ORION5X_GPIO_OUT_ENABLE		0x0000000c
#define ORION5X_GPIO_IN_POLARITY	0x00000000

/*
 * SoC configuration
 */
#define CONFIG_FEROCEON
#define CONFIG_88F5182

/*
 * Miscellaneous SOC configuration
 */
#define CONFIG_DISPLAY_CPUINFO

/*
 * Memory configuration
 */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_DDR2
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_MAX_RAM_BANK_SIZE	(128 * 1024 * 1024)
#define CONFIG_SYS_INIT_SP_ADDR		(0x1000 - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_MALLOC_LEN		0x00800000
#define CONFIG_SYS_LOAD_ADDR		0x00080000
#if defined(CONFIG_SYS_RAMBOOT)
#define CONFIG_SYS_TEXT_BASE		0x00080000
#else
#define CONFIG_SYS_TEXT_BASE		0xfffc0000
#endif

/*
 * Init configurations
 */
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_ARCH_MISC_INIT
#define CONFIG_BOARD_LATE_INIT
#define CONFIG_LAST_STAGE_INIT

/*
 * Console configuration
 */
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_TCLK
#define CONFIG_SYS_NS16550_COM1		ORION5X_UART0_BASE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
#define CONFIG_CONS_INDEX		1
#define CONFIG_BAUDRATE			115200
#define	CONFIG_SYS_CBSIZE		1024
#define	CONFIG_SYS_PBSIZE		2048
#define CONFIG_SYS_MAXARGS		36

#if defined(CONFIG_SYS_RAMBOOT)
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#endif

/*
 * Flash configuration
 */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_FLASH_CFI_LEGACY
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	64
#define CONFIG_SYS_FLASH_BASE		0xfffc0000
#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_FLASH_SHOW_PROGRESS	45

/*
 * Environment configuration
 */
#ifdef CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET		0xc0000
#define CONFIG_ENV_SIZE			0x20000
#else
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_OFFSET		0x3f000
#define CONFIG_ENV_SIZE			0x1000
#endif

/*
 * Auto-boot configuration
 */
#define CONFIG_BOOTDELAY		3
#define CONFIG_ZERO_BOOTDELAY_CHECK

/*
 * Linux boot configuration
 */
#define CONFIG_SYS_PARAMS_PHYS		0x00000100
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_SETUP_MEMORY_TAGS

#if defined(CONFIG_SYS_RAMBOOT) || !defined(CONFIG_TARGET_KUROPRO)
#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_FDT
#endif

/*
 * Command configuration
 */
#define CONFIG_CMD_DATE
#define CONFIG_CMD_IDE
#define CONFIG_CMD_TFTPPUT
#define CONFIG_CMD_USB

#if defined(CONFIG_SYS_RAMBOOT)
#define CONFIG_CMD_GREPENV
#define CONFIG_CMD_MEMINFO
#define CONFIG_CMD_READ
#define CONFIG_CMD_TIME
#define CONFIG_CMD_TIMER
#endif

/*
 * CMD_DATE configuration
 */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_RS5C372A
#define CONFIG_SYS_I2C
#define CONFIG_SYS_I2C_MVTWSI
#define CONFIG_I2C_MVTWSI_BASE0		ORION5X_TWSI_BASE
#define CONFIG_SYS_I2C_SLAVE		0x0
#define CONFIG_SYS_I2C_SPEED		100000

#if defined(CONFIG_SYS_RAMBOOT)
#define CONFIG_CMD_I2C
#endif

#endif

/*
 * CMD_IDE configuration
 */
#ifdef CONFIG_CMD_IDE
#define CONFIG_MVSATA_IDE
#define CONFIG_SYS_ATA_BASE_ADDR	ORION5X_SATA_BASE
/* Port 0 for Linkstation v2 and Kurobox-Pro; Port 1 for Linkstation v1 */
#define CONFIG_SYS_ATA_IDE0_OFFSET	ORION5X_SATA_PORT0_OFFSET
#define CONFIG_SYS_ATA_IDE1_OFFSET	ORION5X_SATA_PORT1_OFFSET
#define CONFIG_SYS_ATA_DATA_OFFSET	0x0100
#define CONFIG_SYS_ATA_REG_OFFSET	0x0100
#define CONFIG_SYS_ATA_ALT_OFFSET	0x0100
#define CONFIG_SYS_ATA_STRIDE		4
#define __io
#define CONFIG_IDE_SWAP_IO
#define CONFIG_SYS_IDE_MAXBUS		1
#define CONFIG_SYS_IDE_MAXDEVICE	1
#define CONFIG_IDE_PREINIT
#define CONFIG_LBA48
#define CONFIG_SYS_64BIT_LBA
#endif

/*
 * CMD_NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_ORION
#define CONFIG_SYS_NAND_SELF_INIT
#define CONFIG_SYS_NAND_BASE		0xfa000000
#define CONFIG_SYS_MAX_NAND_DEVICE	1

#if defined(CONFIG_SYS_RAMBOOT) || !defined(CONFIG_TARGET_KUROPRO)
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#define MTDIDS_DEFAULT			"nand0=orion-nand"
#define MTDPARTS_DEFAULT		"mtdparts=orion-nand:" \
		"0xc0000(U-Boot),0x40000@0xc0000(ENV),-@0x100000(UBIFS)"
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_LZO
#endif

#endif

/*
 * CMD_NET configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE
#define CONFIG_MVGBE_PORTS		{1}
#define CONFIG_PHY_BASE_ADR		8
#define CONFIG_CMD_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN
#define CONFIG_PHY_GIGE
#define CONFIG_ENV_OVERWRITE
#define CONFIG_ETHADDR			02:08:8f:51:82:a2
#define CONFIG_NETCONSOLE
#define CONFIG_CMD_PING
#define CONFIG_IPADDR			192.168.11.150
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_SERVERIP			192.168.11.1
#define CONFIG_EXTRA_ENV_SETTINGS	"stdin=nc\0" \
					"stdout=nc\0" \
					"stderr=nc"
#endif

/*
 * CMD_USB configuration
 */
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI_MARVELL
#define CONFIG_USB_EHCI
#define CONFIG_EHCI_IS_TDI
#define CONFIG_USB_MAX_CONTROLLER_COUNT	2
#define CONFIG_USB_STORAGE
#endif

/*
 * Filesystem configuration
 */
#if defined(CONFIG_CMD_IDE) || defined(CONFIG_CMD_USB)
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FS_GENERIC

#if defined(CONFIG_SYS_RAMBOOT) || !defined(CONFIG_TARGET_KUROPRO)
#define CONFIG_FAT_WRITE
#define CONFIG_CMD_EXT4_WRITE
#endif

#endif

/*
 * Reduce ROMBOOT image size
 */
#ifdef CONFIG_SYS_ROMBOOT
/* undef from config_defaults.h */
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS
#undef CONFIG_BOOTM_VXWORKS
/* undef from config_cmd_defaults.h */
#undef CONFIG_CMD_CRC32
#undef CONFIG_CMD_EXPORTENV
#undef CONFIG_CMD_IMPORTENV
/* undef from config_cmd_default.h */
#undef CONFIG_CMD_BDI
#undef CONFIG_CMD_ECHO
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_ITEST
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_LOADB
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_MISC
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_SETGETDCR
#undef CONFIG_CMD_SOURCE
#undef CONFIG_CMD_XIMG
#endif

#include <asm/arch/orion5x.h>

#endif /* _CONFIG_LSGL_H */
