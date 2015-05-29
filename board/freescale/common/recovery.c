/*
 * Freescale Android Recovery mode checking routing
 *
 * Copyright (C) 2010-2012 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <common.h>
#include <malloc.h>
#include <recovery.h>

extern int check_recovery_cmd_file(void);

void setup_recovery_env(void)
{
	puts("set boxroot to /dev/mmcblk0p6\n"); 
	setenv("boxargs", "setenv bootargs console=${console},${baudrate} root=/dev/mmcblk0p6 rootwait rw init=/linuxrc ${hdmi} fbcon");
}

/* export to lib_arm/board.c */
void check_recovery_mode(void)
{
	if (check_recovery_cmd_file()) 
	{
		puts("Recovery command file found!\n");
		setup_recovery_env();
	}
}
