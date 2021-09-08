/*
 * (C) Copyright 2012
 * Steve Shih <shihsung@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MICON_H
#define __MICON_H

#define MICON_BUTTON_POWER(button)	!((button) & 0x01)
#define MICON_BUTTON_INIT(button)	!((button) & 0x08)

void micon_init(void);
void micon_boot(void);
int micon_buttons(void);
bool micon_emergency(void);
void micon_hdd(bool on);
void micon_error(void);
void micon_off(void);
void micon_reboot(void);
void micon_up(void);

#endif	/* __MICON_H */
