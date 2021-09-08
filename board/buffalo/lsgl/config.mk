#
# (C) Copyright 2012
# Steve Shih <shihsung@gmail.com>
#
# SPDX-License-Identifier:	GPL-2.0+
#

ifdef CONFIG_SYS_ROMBOOT
CONFIG_BOARD_SIZE_LIMIT = 0x0003f000
CONFIG_SYS_LDSCRIPT = $(TOPDIR)/board/$(BOARDDIR)/romboot.lds
OBJCOPYFLAGS += -j .romboot
endif
