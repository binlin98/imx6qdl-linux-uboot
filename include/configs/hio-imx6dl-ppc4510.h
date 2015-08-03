/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Freescale i.MX6Q SabreSD board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __MX6QSABRESD_CONFIG_H
#define __MX6QSABRESD_CONFIG_H

#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

//+++MQ
//#define CONFIG_FOR_100M
#define CONFIG_RANDOM_MACADDR
#define CONFIG_CMD_LINK_LOCAL
#define CONFIG_SUPPORT_BOX
#define CONFIG_VKI_USB_UPDATE

//#define CONFIG_RTL8169
//#define DEBUG_RTL8169
//#define CONFIG_PCI_SCAN_SHOW 1

//#define CONFIG_PCI
//#define CONFIG_CMD_PCI
//#define CONFIG_PCIE_IMX
//#define CONFIG_PCI_PNP
//#define CONFIG_PCIE_IMX_PERST_GPIO IMX_GPIO_NR(5,28)
//+++MQ
#define CONFIG_MACH_TYPE	3980
#define CONFIG_MXC_UART_BASE	UART2_BASE
#define CONFIG_CONSOLE_DEV		"ttymxc1"
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"
#if defined(CONFIG_MX6Q)
#define CONFIG_DEFAULT_FDT_FILE	"imx6q-sabresd-ldo.dtb"
#elif defined(CONFIG_MX6DL)
#define CONFIG_DEFAULT_FDT_FILE	"hio-imx6dl-ppc4510.dtb"
#endif
#define PHYS_SDRAM_SIZE		(1u * 1024 * 1024 * 1024)

#define CONFIG_SUPPORT_EMMC_BOOT /* eMMC specific */

#include "mx6sabre_common.h"

#define CONFIG_SYS_FSL_USDHC_NUM	3
#if defined(CONFIG_ENV_IS_IN_MMC)
#define CONFIG_SYS_MMC_ENV_DEV		1	/* SDHC3 */
#endif

/* Framebuffer */
//#define CONFIG_VIDEO
#define CONFIG_VIDEO_IPUV3
//#define CONFIG_CFB_CONSOLE
#define CONFIG_VGA_AS_SINGLE_DEVICE
#define CONFIG_SYS_CONSOLE_IS_IN_ENV
#define CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
//#define CONFIG_VIDEO_BMP_RLE8
//#define CONFIG_SPLASH_SCREEN
//#define CONFIG_SPLASH_SCREEN_ALIGN
//#define CONFIG_BMP_16BPP
//#define CONFIG_VIDEO_LOGO
//#define CONFIG_VIDEO_BMP_LOGO
#define CONFIG_IPUV3_CLK 260000000
#define CONFIG_IMX_HDMI
//#define CONFIG_BOARD_EARLY_INIT_F
//#define CONFIG_CMD_BMP

/*
* usb configs
*/
#define CONFIG_CMD_USB
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_MX6
#define CONFIG_USB_STORAGE
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_EHCI_HCD_INIT_AFTER_RESET        /* For OTG port */
#define CONFIG_MXC_USB_PORTSC   (PORT_PTS_UTMI | PORT_PTS_PTW)
#define CONFIG_MXC_USB_FLAGS    0

/*
* ext4 filesystem configs
*/
#define CONFIG_FS_EXT4
#define CONFIG_EXT4_WRITE
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_CMD_FS_GENERIC

/*
* lcd hardware parames
*/
#define CONFIG_LCD_CONF 1

//+++WWJ
/*
* recovery configure
*/
//#define CONFIG_RECOVERY

/*
*
* I2C Configs
*/
#define CONFIG_CMD_I2C
#ifdef CONFIG_CMD_I2C
	#define CONFIG_HARD_I2C         1
	#define CONFIG_I2C_MXC          1
	#define CONFIG_SYS_I2C_PORT             I2C2_BASE_ADDR
	#define CONFIG_SYS_I2C_SPEED            100000
	#define CONFIG_SYS_I2C_SLAVE            0x08
//	#define CONFIG_MX6_INTER_LDO_BYPASS	0
#endif

#endif                         /* __MX6QSABRESD_CONFIG_H */
