/*
 * Android Recovery supported header file
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

#ifndef __RECOVERY_H_
#define __RECOVERY_H_
enum boot_device {
        WEIM_NOR_BOOT,
        ONE_NAND_BOOT,
        PATA_BOOT,
        SATA_BOOT,
        I2C_BOOT,
        SPI_NOR_BOOT,
        SD_BOOT,
        MMC_BOOT,
        NAND_BOOT,
        UNKNOWN_BOOT,
        BOOT_DEV_NUM = UNKNOWN_BOOT,
};
struct reco_envs {
	char *cmd;
	char *args;
};

int check_and_clean_recovery_flag(void);
void check_recovery_mode(void);
#if 0
struct reco_envs supported_reco_envs[BOOT_DEV_NUM] = {
        {
         .cmd = NULL,
         .args = NULL,
         },     
        {       
         .cmd = NULL,
         .args = NULL,
         },
        {
         .cmd = NULL,
         .args = NULL,
         },
        {       
         .cmd = NULL,
         .args = NULL,
         },
        {
         .cmd = NULL,
         .args = NULL,
         },
        {
         .cmd = NULL,
         .args = NULL,
         },
        {
         .cmd = CONFIG_ANDROID_RECOVERY_BOOTCMD_MMC,
         .args = CONFIG_ANDROID_RECOVERY_BOOTARGS_MMC,
         },
        {
         .cmd = CONFIG_ANDROID_RECOVERY_BOOTCMD_MMC,
         .args = CONFIG_ANDROID_RECOVERY_BOOTARGS_MMC,
         },
#ifdef CONFIG_CMD_UBI
        {
         .cmd = CONFIG_ANDROID_RECOVERY_BOOTCMD_NAND,
         .args = CONFIG_ANDROID_RECOVERY_BOOTARGS_NAND,
         },
#endif
};
#endif
#endif

