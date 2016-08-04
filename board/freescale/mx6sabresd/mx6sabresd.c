/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/mxc_hdmi.h>
#include <asm/arch/crm_regs.h>
#include <linux/fb.h>
#include <ipu_pixfmt.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <recovery.h>

//+++WWJ
#include <linux/shenonmxc.h>

//+++WWJ
#if CONFIG_I2C_MXC
#include <i2c.h>
#endif

DECLARE_GLOBAL_DATA_PTR;


//+++WWJ begin
#define msleep(a) udelay(a * 1000)
#ifdef CONFIG_QM215
#define CLKCTL_CCR              0x00
#define CLKCTL_CCDR             0x04
#define CLKCTL_CSR              0x08
#define CLKCTL_CCSR             0x0C
#define CLKCTL_CACRR            0x10
#define CLKCTL_CBCDR            0x14
#define CLKCTL_CBCMR            0x18
#define CLKCTL_CSCMR1           0x1C
#define CLKCTL_CSCMR2           0x20
#define CLKCTL_CSCDR1           0x24
#define CLKCTL_CS1CDR           0x28
#define CLKCTL_CS2CDR           0x2C
#define CLKCTL_CDCDR            0x30
#define CLKCTL_CHSCCDR          0x34
#define CLKCTL_CSCDR2           0x38
#define CLKCTL_CSCDR3           0x3C
#define CLKCTL_CSCDR4           0x40
#define CLKCTL_CWDR             0x44
#define CLKCTL_CDHIPR           0x48
#define CLKCTL_CDCR             0x4C
#define CLKCTL_CTOR             0x50
#define CLKCTL_CLPCR            0x54
#define CLKCTL_CISR             0x58
#define CLKCTL_CIMR             0x5C
#define CLKCTL_CCOSR            0x60
#define CLKCTL_CGPR             0x64
#define CLKCTL_CCGR0            0x68
#define CLKCTL_CCGR1            0x6C
#define CLKCTL_CCGR2            0x70
#define CLKCTL_CCGR3            0x74
#define CLKCTL_CCGR4            0x78
#define CLKCTL_CCGR5            0x7C
#define CLKCTL_CCGR6            0x80
#define CLKCTL_CCGR7            0x84
#define CLKCTL_CMEOR            0x88
#endif
//+++WWJ end

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED | \
		      PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)

//+++WWJ begin
#ifdef CONFIG_I2C_MXC
#define CLKCTL_CCGR2            0x70
#define MX6DL_I2C_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE   |              \
               PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |               \
               PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  |               \
               PAD_CTL_HYS         | PAD_CTL_ODE)
#endif
//+++WWJ end

#ifdef CONFIG_HDMI_DEC
static int hdmi_detect(void);
#endif

int dram_init(void)
{
	gd->ram_size = get_ram_size((void *)PHYS_SDRAM, PHYS_SDRAM_SIZE);

	return 0;
}

//+++MQ
iomux_v3_cfg_t const pin_pads[] = {
	MX6_PAD_EIM_DA5__GPIO3_IO05 | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_EIM_DA11__GPIO3_IO11 | MUX_PAD_CTRL(UART_PAD_CTRL),
};
//+++MQ

iomux_v3_cfg_t const uart1_pads[] = {
	//MX6_PAD_CSI0_DAT10__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	//MX6_PAD_CSI0_DAT11__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_EIM_D26__UART2_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_EIM_D27__UART2_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

iomux_v3_cfg_t const enet_pads[] = {
#ifdef CONFIG_PHY_ATHEROS
	MX6_PAD_ENET_MDIO__ENET_MDIO		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_MDC__ENET_MDC		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TXC__RGMII_TXC	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD0__RGMII_TD0	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD1__RGMII_TD1	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD2__RGMII_TD2	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD3__RGMII_TD3	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TX_CTL__RGMII_TX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_REF_CLK__ENET_TX_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RXC__RGMII_RXC	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD0__RGMII_RD0	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD1__RGMII_RD1	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD2__RGMII_RD2	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD3__RGMII_RD3	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RX_CTL__RGMII_RX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	/* AR8031 PHY Reset */
	MX6_PAD_ENET_CRS_DV__GPIO1_IO25		| MUX_PAD_CTRL(NO_PAD_CTRL),
#endif
//+++MQ
#ifdef CONFIG_PHY_SMSC
	MX6_PAD_ENET_MDC__ENET_MDC           | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_MDIO__ENET_MDIO            | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_CRS_DV__ENET_RX_EN      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RXD0__ENET_RX_DATA0       | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RXD1__ENET_RX_DATA1      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_TX_EN__ENET_TX_EN      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_TXD0__ENET_TX_DATA0      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_TXD1__ENET_TX_DATA1      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	//MX6_PAD_RGMII_TX_CTL__ENET_REF_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_RX_ER__ENET_RX_ER      | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* FEC Intterupt */
	//MX6_PAD_ENET_REF_CLK__GPIO1_IO23      | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* clock from anatop */
	MX6_PAD_GPIO_16__ENET_REF_CLK      | MUX_PAD_CTRL(ENET_PAD_CTRL),
	//MX6_PAD_ENET_REF_CLK__ENET_REF_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	
	/*
	MX6_PAD_FEC_MDC__FEC_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_MDIO__FEC_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_CRS_DV__FEC_RX_DV | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RXD0__FEC_RX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RXD1__FEC_RX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TX_EN__FEC_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TXD0__FEC_TX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_TXD1__FEC_TX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_REF_CLK__FEC_REF_OUT | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_FEC_RX_ER__GPIO_4_19 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_FEC_TX_CLK__GPIO_4_21 | MUX_PAD_CTRL(NO_PAD_CTRL),
	*/
//+	MX6Q_PAD_ENET_RXD0__ENET_RDATA_0,
//+	MX6Q_PAD_ENET_RXD1__ENET_RDATA_1,
//+	MX6Q_PAD_ENET_CRS_DV__ENET_RX_EN,
//+	MX6Q_PAD_ENET_TXD0__ENET_TDATA_0,
//+	MX6Q_PAD_ENET_TXD1__ENET_TDATA_1,
//+	MX6Q_PAD_ENET_TX_EN__ENET_TX_EN,
//+	MX6Q_PAD_RGMII_TX_CTL__ENET_ANATOP_ETHERNET_REF_OUT,
//+	MX6Q_PAD_KEY_ROW4__GPIO_4_15, /* reset phy */
//+
//	+	/*
//		   +	 * Since FEC_RX_ER is not connected with PHY(LAN8720A),
//		   +	 * configure FEC_RX_ER PAD to GPIO mode with pull down,
//		   +	 * which can avoid FEC MAC to report CRC error.
//		   +	 */
//	+	MX6Q_PAD_ENET_RX_ER__GPIO_1_24,
#endif
//+++MQ
};

static void setup_iomux_enet(void)
{
	printf("setup_iomux_enet\n");
#ifdef CONFIG_PHY_SMSC
	printf("CONFIG_PHY_SMSC\n");
#endif
	imx_iomux_v3_setup_multiple_pads(enet_pads, ARRAY_SIZE(enet_pads));

	/* Reset AR8031 PHY */
	gpio_direction_output(IMX_GPIO_NR(1, 25) , 0);
	udelay(500);
	gpio_set_value(IMX_GPIO_NR(1, 25), 1);
}

iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__SD2_CLK	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__SD2_CMD	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__SD2_DATA0	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__SD2_DATA1	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__SD2_DATA2	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__SD2_DATA3	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D4__SD2_DATA4	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D5__SD2_DATA5	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D6__SD2_DATA6	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D7__SD2_DATA7	| MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D2__GPIO2_IO02	| MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
	MX6_PAD_CSI0_DAT10__GPIO5_IO28 | MUX_PAD_CTRL(UART_PAD_CTRL),/*MQ*/
};

iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__SD3_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__SD3_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT4__SD3_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT5__SD3_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT6__SD3_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT7__SD3_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D0__GPIO2_IO00    | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
};

iomux_v3_cfg_t const usdhc4_pads[] = {
	MX6_PAD_SD4_CLK__SD4_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_CMD__SD4_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT0__SD4_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT1__SD4_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT2__SD4_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT3__SD4_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT4__SD4_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT5__SD4_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT6__SD4_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT7__SD4_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

iomux_v3_cfg_t const ecspi1_pads[] = {
	MX6_PAD_KEY_COL0__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_COL1__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_ROW0__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_KEY_ROW1__GPIO4_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_spi(void)
{
	imx_iomux_v3_setup_multiple_pads(ecspi1_pads, ARRAY_SIZE(ecspi1_pads));
}

//+++WWJ begin
#ifdef CONFIG_RII
iomux_v3_cfg_t const poweron[] = {
	MX6_PAD_KEY_ROW2__GPIO4_IO11,
};
#define SABRESD_POWER_ON IMX_GPIO_NR(4, 11)

void power_on(void){
	imx_iomux_v3_setup_multiple_pads(poweron, ARRAY_SIZE(poweron));
	gpio_direction_output(SABRESD_POWER_ON, 1);
}
#endif
//+++WWJ end

//add novo gpio
#define GPIO_P20_10	IMX_GPIO_NR(7, 12)
#define GPIO_P20_12	IMX_GPIO_NR(6, 4)
#define GPIO_P20_14 IMX_GPIO_NR(5, 26)
#define GPIO_P20_20 IMX_GPIO_NR(6, 5)
#define GPIO_P20_21 IMX_GPIO_NR(5, 27)
#define GPIO_P20_22 IMX_GPIO_NR(5, 28)
#define GPIO_P20_23 IMX_GPIO_NR(5, 29)
#define GPIO_P24_05 IMX_GPIO_NR(4, 5)
#define GPIO_P24_06 IMX_GPIO_NR(1, 3)
#define GPIO_P24_08 IMX_GPIO_NR(1, 8)
#define TSC2007_PWR	IMX_GPIO_NR(6, 3)

//add wifi
#define WIFI_SD0 IMX_GPIO_NR(1, 16)
#define WIFI_SD1 IMX_GPIO_NR(1, 17)
#define WIFI_SD2 IMX_GPIO_NR(1, 19)
#define WIFI_SD3 IMX_GPIO_NR(1, 21)

void novo_gpio(void)
{
	gpio_direction_output(GPIO_P20_10, 0);
	gpio_direction_output(GPIO_P20_12, 0);
	gpio_direction_output(GPIO_P20_14, 0);
	gpio_direction_output(GPIO_P20_20, 0);

	gpio_direction_output(GPIO_P20_22, 0);
	gpio_direction_output(GPIO_P20_23, 0);
	gpio_direction_output(GPIO_P24_05, 0);
	gpio_direction_output(GPIO_P24_06, 0);

	gpio_direction_output(GPIO_P24_08, 0);
	gpio_direction_output(TSC2007_PWR, 0);

	gpio_direction_output(WIFI_SD0, 0);
	gpio_direction_output(WIFI_SD1, 0);
	gpio_direction_output(WIFI_SD2, 0);
	gpio_direction_output(WIFI_SD3, 0);

	//backlight
	gpio_direction_output(GPIO_P20_21, 1);
}

iomux_v3_cfg_t const di0_pads[] = {
	MX6_PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK,	/* DISP0_CLK */
	MX6_PAD_DI0_PIN2__IPU1_DI0_PIN02,		/* DISP0_HSYNC */
	MX6_PAD_DI0_PIN3__IPU1_DI0_PIN03,		/* DISP0_VSYNC */
//+++WWJ begin
/*MQ remove useless
	MX6_PAD_DI0_PIN15__IPU1_DI0_PIN15,
	MX6_PAD_DI0_PIN4__GPIO4_IO20,
	MX6_PAD_DISP0_DAT0__IPU1_DISP0_DATA00,
	MX6_PAD_DISP0_DAT1__IPU1_DISP0_DATA01,
	MX6_PAD_DISP0_DAT2__IPU1_DISP0_DATA02,
	MX6_PAD_DISP0_DAT3__IPU1_DISP0_DATA03,
	MX6_PAD_DISP0_DAT4__IPU1_DISP0_DATA04,
	MX6_PAD_DISP0_DAT5__IPU1_DISP0_DATA05,
	MX6_PAD_DISP0_DAT6__IPU1_DISP0_DATA06,
	MX6_PAD_DISP0_DAT7__IPU1_DISP0_DATA07,
	MX6_PAD_DISP0_DAT8__IPU1_DISP0_DATA08,
	MX6_PAD_DISP0_DAT9__IPU1_DISP0_DATA09,
	MX6_PAD_DISP0_DAT10__IPU1_DISP0_DATA10,
	MX6_PAD_DISP0_DAT11__IPU1_DISP0_DATA11,
	MX6_PAD_DISP0_DAT12__IPU1_DISP0_DATA12,
	MX6_PAD_DISP0_DAT13__IPU1_DISP0_DATA13,
	MX6_PAD_DISP0_DAT14__IPU1_DISP0_DATA14,
	MX6_PAD_DISP0_DAT15__IPU1_DISP0_DATA15,
	MX6_PAD_DISP0_DAT16__IPU1_DISP0_DATA16,
	MX6_PAD_DISP0_DAT17__IPU1_DISP0_DATA17,
	MX6_PAD_DISP0_DAT18__IPU1_DISP0_DATA18,
	MX6_PAD_DISP0_DAT19__IPU1_DISP0_DATA19,
	MX6_PAD_DISP0_DAT20__IPU1_DISP0_DATA20,
	MX6_PAD_DISP0_DAT21__IPU1_DISP0_DATA21,
	MX6_PAD_DISP0_DAT22__IPU1_DISP0_DATA22,
	MX6_PAD_DISP0_DAT23__IPU1_DISP0_DATA23,
*/

	MX6_PAD_GPIO_19__GPIO4_IO05 | MUX_PAD_CTRL(NO_PAD_CTRL),	/*LCD 3.3V*/
	MX6_PAD_GPIO_17__GPIO7_IO12	| MUX_PAD_CTRL(NO_PAD_CTRL),	/*LCD backlight IC enable*/
	MX6_PAD_GPIO_8__GPIO1_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),	/*LCD backlight 12V*/

	//novo gpio	
	//p20
	MX6_PAD_GPIO_17__GPIO7_IO12	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_10*/
	MX6_PAD_CSI0_DAT18__GPIO6_IO04	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_12*/
	MX6_PAD_CSI0_DAT8__GPIO5_IO26	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_14*/
		
	MX6_PAD_CSI0_DAT19__GPIO6_IO05	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_20*/
	MX6_PAD_CSI0_DAT9__GPIO5_IO27	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_21*/
	MX6_PAD_CSI0_DAT10__GPIO5_IO28	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_22*/
	MX6_PAD_CSI0_DAT11__GPIO5_IO29	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p20_23*/
	
	//p24
	MX6_PAD_GPIO_19__GPIO4_IO05	| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p24_5*/
	MX6_PAD_GPIO_3__GPIO1_IO03 		| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p24_6*/
	MX6_PAD_GPIO_8__GPIO1_IO08 		| MUX_PAD_CTRL(NO_PAD_CTRL),	  /*out-p24_8*/

	MX6_PAD_CSI0_DAT17__GPIO6_IO03	| MUX_PAD_CTRL(NO_PAD_CTRL),      /*cts_gnd*/

	//wifi-sdio1
	MX6_PAD_SD1_DAT0__GPIO1_IO16  | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SD1_DAT1__GPIO1_IO17  | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SD1_DAT2__GPIO1_IO19  | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SD1_DAT3__GPIO1_IO21  | MUX_PAD_CTRL(NO_PAD_CTRL),	

//+++WWJ end
};


//+++WWJ begin
#define SABRESD_LCD_3v3 IMX_GPIO_NR(4, 5)
#define SABRESD_LCD_EN	IMX_GPIO_NR(7, 12)
#define SABRESD_LCD_12V IMX_GPIO_NR(1, 8)
//+++WWJ end

//+++MQ
#define PIN1_GPIO	IMX_GPIO_NR(3, 5)
#define PIN2_GPIO	IMX_GPIO_NR(3, 11)
static int hdmi_cd = 0;
//+++MQ

static void setup_iomux_pin(void)
{
	imx_iomux_v3_setup_multiple_pads(pin_pads, ARRAY_SIZE(pin_pads));
//+++WWJ
#ifdef CONFIG_EEPROM_RW
	imx_iomux_v3_setup_pad(MX6_PAD_KEY_COL2__GPIO4_IO10);
#endif
//+++WWJ
	check_pin_status();
}

void check_pin_status(void)
{
	int pin1 = 0, pin2 = 0;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
//+++WWJ begin
#ifdef CONFIG_HDMI_DEC
	if(hdmi_detect())
		hdmi_cd = 1;
	printf("WWJWWJWWJWWJ hdmi_cd = %d\n", hdmi_cd);
#endif

#if 0
//+++wwj begin
#ifdef CONFIG_RII
	setenv("hdmi", "video=mxcfb0:dev=hdmi,1280x720M@60,if=RGB24");
#elif defined(CONFIG_HKC215)
	setenv("hdmi", "video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24");
#else
//+++wwj end
	printf("MQ---hdmi_cd:%d\n", hdmi_cd);
	if (hdmi_cd) {
		printf("hdmi state 1, set mmcargs use hdmi\n");	
		setenv("hdmi", "video=mxcfb0:dev=hdmi,1280x720M@60,if=RGB24");
	} else {
		printf("hdmi state 2, set mmcargs use lvds\n");	
		setenv("hdmi", " ");
	}
//+++wwj
#endif

#endif
	gpio_direction_input(PIN1_GPIO);
	gpio_direction_input(PIN2_GPIO);
	pin1 = gpio_get_value(PIN1_GPIO);
	printf("MQ==%s, pin1:%d \n",__FUNCTION__, pin1);
	pin2 = gpio_get_value(PIN2_GPIO);
	printf("MQ==%s, pin2:%d \n",__FUNCTION__, pin2);
	if (pin1 == 0 && pin2 == 1) {
		printf("boot from tf....\n");	
		setenv("mmcroot", "/dev/mmcblk1p2 rootwait rw");
		setenv("boxroot", "/dev/mmcblk0p3 rootwait rw");
		setenv("mmcdev", "2");
	} else {
		printf("boot from inand....\n");	
		setenv("mmcroot", "/dev/mmcblk0p2 rootwait rw");
		setenv("boxroot", "/dev/mmcblk0p3 rootwait rw");
		setenv("mmcdev", "1");
	}
	debug("GPR2 = 0x%x, GPR3 = 0x%x\n", iomux->gpr[2], iomux->gpr[3]);
}
//+++MQ

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
}

#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg usdhc_cfg[3] = {
	{USDHC2_BASE_ADDR},
	{USDHC3_BASE_ADDR},
	{USDHC4_BASE_ADDR},
};

#define USDHC2_CD_GPIO	IMX_GPIO_NR(2, 2)
#define USDHC3_CD_GPIO	IMX_GPIO_NR(2, 0)

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC2_BASE_ADDR:
		ret = !gpio_get_value(USDHC2_CD_GPIO);
		break;
	case USDHC3_BASE_ADDR:
		//ret = !gpio_get_value(USDHC3_CD_GPIO);
	    ret = 1; 
		break;
	case USDHC4_BASE_ADDR:
		ret = 1; /* eMMC/uSDHC4 is always present */
		break;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	s32 status = 0;
	int i;

	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-boot device node)    (Physical Port)
	 * mmc0                    SD2
	 * mmc1                    SD3
	 * mmc2                    eMMC
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
			gpio_direction_input(USDHC2_CD_GPIO);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
			break;
		case 1:
			imx_iomux_v3_setup_multiple_pads(
				usdhc3_pads, ARRAY_SIZE(usdhc3_pads));
			gpio_direction_input(USDHC3_CD_GPIO);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
			break;
		case 2:
			imx_iomux_v3_setup_multiple_pads(
				usdhc4_pads, ARRAY_SIZE(usdhc4_pads));
			usdhc_cfg[2].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
			break;
		default:
			printf("Warning: you configured more USDHC controllers"
			       "(%d) then supported by the board (%d)\n",
			       i + 1, CONFIG_SYS_FSL_USDHC_NUM);
			return status;
		}

		status |= fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
	}

	return status;
}
#endif

int mx6_rgmii_rework(struct phy_device *phydev)
{
	unsigned short val;

	/* To enable AR8031 ouput a 125MHz clk from CLK_25M */
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x7);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, 0x8016);
	phy_write(phydev, MDIO_DEVAD_NONE, 0xd, 0x4007);

	val = phy_read(phydev, MDIO_DEVAD_NONE, 0xe);
	val &= 0xffe3;
	val |= 0x18;
	phy_write(phydev, MDIO_DEVAD_NONE, 0xe, val);

	/* introduce tx clock delay */
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1d, 0x5);
	val = phy_read(phydev, MDIO_DEVAD_NONE, 0x1e);
	val |= 0x0100;
	phy_write(phydev, MDIO_DEVAD_NONE, 0x1e, val);

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
	mx6_rgmii_rework(phydev);

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}

#if defined(CONFIG_VIDEO_IPUV3)
struct display_info_t {
	int	bus;
	int	addr;
	int	pixfmt;
	int	(*detect)(struct display_info_t const *dev);
	void	(*enable)(struct display_info_t const *dev);
	struct	fb_videomode mode;
};

static int detect_hdmi(struct display_info_t const *dev)
{
	printf("detect_hdmi\n");
	struct hdmi_regs *hdmi	= (struct hdmi_regs *)HDMI_ARB_BASE_ADDR;
	return readb(&hdmi->phy_stat0) & HDMI_DVI_STAT;
}


static void disable_lvds(struct display_info_t const *dev)
{
	
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	int reg = readl(&iomux->gpr[2]);
	printf("disable_lvds reg addr:0x%x reg1:0x%x\n", &iomux->gpr[2], reg);

	reg &= ~(IOMUXC_GPR2_LVDS_CH0_MODE_MASK |
		 IOMUXC_GPR2_LVDS_CH1_MODE_MASK);

	printf("reg2:0x%x\n", reg);
	writel(reg, &iomux->gpr[2]);
}

static void do_enable_hdmi(struct display_info_t const *dev)
{
	printf("do_enable_hdmi\n");
	disable_lvds(dev);
	imx_enable_hdmi_phy();
}

static void enable_lvds(struct display_info_t const *dev)
{
	printf("enable_lvds\n");
	struct iomuxc *iomux = (struct iomuxc *)
				IOMUXC_BASE_ADDR;
	u32 reg = readl(&iomux->gpr[2]);
//+++WWJ begin
	reg |= IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT |
	       IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT;
//+++WWJ end
	writel(reg, &iomux->gpr[2]);

//+++WWJ begin
	debug("GPR2 = 0x%x, GPR3 = 0x%x\n", iomux->gpr[2], iomux->gpr[3]);
	gpio_direction_output(SABRESD_LCD_3v3, 0);
	msleep(450);
	gpio_direction_output(SABRESD_LCD_12V, 1);
	gpio_direction_output(SABRESD_LCD_EN, 1);
//+++WWJ end
}

static struct display_info_t const displays[] = {{
	.bus	= -1,
	.addr	= 0,
//+++WWJ
	//.pixfmt	= IPU_PIX_FMT_RGB666,
	.pixfmt = IPU_PIX_FMT_RGB24,
//+++WWJ
	.detect	= NULL,
	.enable	= enable_lvds,

//+++WWJ begin 
#ifdef CONFIG_HK185
	.mode	= {
		.name           = "Hannstar-XGA",
		.refresh        = 60,
		.xres           = 1366,
		.yres           = 768,
		.pixclock       = 13871,
		.left_margin    = 80,
		.right_margin   = 10,
		.upper_margin   = 21,
		.lower_margin   = 5,
		.hsync_len      = 20,
		.vsync_len      = 20,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
	} 
#elif CONFIG_QM215
	.mode   = {
		.name           = "Hannstar-XGA",
		.refresh        = 60,
		.xres           = 1920,
		.yres           = 1080,
		.pixclock       = 6966,
		.left_margin    = 148,
		.right_margin   = 88,
		.upper_margin   = 30,
		.lower_margin   = 9,
		.hsync_len      = 44,
		.vsync_len      = 6,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
	} 

#else
	.mode	= {
		.name           = "Hannstar-XGA",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
	} 
#endif
//+++WWJ end

}, {
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_hdmi,
	.enable	= do_enable_hdmi,
	.mode	= {
		.name           = "HDMI",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} } };

#ifdef CONFIG_HDMI_DEC
static int hdmi_detect(void)
{
	return detect_hdmi(&displays[1]);
}
#endif

int board_video_skip(void)
{
	int i;
	int ret;
	char const *panel = getenv("panel");
//+++WWJ begin
	struct iomuxc *iomux = (struct iomuxc *)
				IOMUXC_BASE_ADDR;
	debug("board_video_skip *panel = %d\n", *panel);
	debug("GPR2 = 0x%x, GPR3 = 0x%x\n", iomux->gpr[2], iomux->gpr[3]);
//+++WWJ end
	if (!panel) {
		for (i = 0; i < ARRAY_SIZE(displays); i++) {
			struct display_info_t const *dev = displays+i;
			if (dev->detect && dev->detect(dev)) {
				panel = dev->mode.name;
				printf("auto-detected panel %s\n", panel);
				//+++MQ
				hdmi_cd = 1;
				//+++MQ
				break;
			}
		}
		if (!panel) {
			panel = displays[0].mode.name;
			printf("board_video_skip--No panel detected: default to %s\n", panel);
			//+++MQ
			hdmi_cd = 0;
			//+++MQ
			i = 0;
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(displays); i++) {
			if (!strcmp(panel, displays[i].mode.name))
				break;
		}
	}
	i = 0;
	if (i < ARRAY_SIZE(displays)) {
		ret = ipuv3_fb_init(&displays[i].mode, 0,
				    displays[i].pixfmt);
		if (!ret) {
			displays[i].enable(displays+i);
			printf("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}

static void setup_display(void)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int reg;
//+++WWJ begin
#ifdef CONFIG_QM215
	static int di=0;
#endif
//+++WWJ end
	printf("setup_display\n");

	/* Setup HSYNC, VSYNC, DISP_CLK for debugging purposes */
	imx_iomux_v3_setup_multiple_pads(di0_pads, ARRAY_SIZE(di0_pads));

	//add gpio
	novo_gpio();
	
	enable_ipu_clock();
	imx_setup_hdmi();
//+++WWJ begin
#ifdef CONFIG_QM215
	/* Disable ipu1_clk/ipu1_di_clk_x/ldb_dix_clk. */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR3);
	reg &= ~0xC033;
	writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR3);

#if defined CONFIG_MX6Q
	/*
	* Align IPU1 HSP clock and IPU1 DIx pixel clock
	* with kernel setting to avoid screen flick when
	* booting into kernel. Developer should change
	* the relevant setting if kernel setting changes.
	* IPU1 HSP clock tree:
	* osc_clk(24M)->pll2_528_bus_main_clk(528M)->
	* periph_clk(528M)->mmdc_ch0_axi_clk(528M)->
	* ipu1_clk(264M)
	*/
	/* pll2_528_bus_main_clk */
	/* divider */
	writel(0x1, ANATOP_BASE_ADDR + 0x34);
		
	/* periph_clk */
	/* source */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CBCMR);
	reg &= ~(0x3 << 18);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CBCMR);

	reg = readl(CCM_BASE_ADDR + CLKCTL_CBCDR);
	reg &= ~(0x1 << 25);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CBCDR);

	/*
	* Check PERIPH_CLK_SEL_BUSY in
	* MXC_CCM_CDHIPR register.
	*/
	do {
			udelay(5);
			reg = readl(CCM_BASE_ADDR + CLKCTL_CDHIPR);
	} while (reg & (0x1 << 5));
	
	/* mmdc_ch0_axi_clk */
	 /* divider */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CBCDR);
	reg &= ~(0x7 << 19);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CBCDR);
	
	/*
	* Check MMDC_CH0PODF_BUSY in
	* MXC_CCM_CDHIPR register.
	*/
	do {
			udelay(5);
			reg = readl(CCM_BASE_ADDR + CLKCTL_CDHIPR);
	} while (reg & (0x1 << 4));

	/* ipu1_clk */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CSCDR3);
	/* source */
	reg &= ~(0x3 << 9);
	/* divider */
	reg &= ~(0x7 << 11);
	reg |= (0x1 << 11);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CSCDR3);

	 /*
	* ipu1_pixel_clk_x clock tree:
	* osc_clk(24M)->pll2_528_bus_main_clk(528M)->
	* pll2_pfd_352M(452.57M)->ldb_dix_clk(64.65M)->
	* ipu1_di_clk_x(64.65M)->ipu1_pixel_clk_x(64.65M)
	*/
	/* pll2_pfd_352M */
	/* disable */
	 writel(0x1 << 7, ANATOP_BASE_ADDR + 0x104);
	/* divider */
	writel(0x3F, ANATOP_BASE_ADDR + 0x108);
	writel(0x15, ANATOP_BASE_ADDR + 0x104);
	
	/* ldb_dix_clk */
	/* source */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CS2CDR);
	reg &= ~(0x3F << 9);
	reg |= (0x9 << 9);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CS2CDR);
	/* divider */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CSCMR2);
	reg |= (0x3 << 10);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CSCMR2);
	
	/* pll2_pfd_352M */
	/* enable after ldb_dix_clk source is set */
	writel(0x1 << 7, ANATOP_BASE_ADDR + 0x108);

	/* ipu1_di_clk_x */
	/* source */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CHSCCDR);
	reg &= ~0xE07;
	reg |= 0x803;
	writel(reg, CCM_BASE_ADDR + CLKCTL_CHSCCDR);
#elif defined CONFIG_MX6DL /* CONFIG_MX6Q */
	/*
	* IPU1 HSP clock tree:
	* osc_clk(24M)->pll3_usb_otg_main_clk(480M)->
	* pll3_pfd_540M(540M)->ipu1_clk(270M)
	*/
	/* pll3_usb_otg_main_clk */
	/* divider */
	writel(0x3, ANATOP_BASE_ADDR + 0x18);
	
	/* pll3_pfd_540M */
	/* divider */
	writel(0x3F << 8, ANATOP_BASE_ADDR + 0xF8);
	writel(0x10 << 8, ANATOP_BASE_ADDR + 0xF4);
	/* enable */
	writel(0x1 << 15, ANATOP_BASE_ADDR + 0xF8);
	
	/* ipu1_clk */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CSCDR3);
	/* source */
	reg |= (0x3 << 9);
	/* divider */
	reg &= ~(0x7 << 11);
	reg |= (0x1 << 11);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CSCDR3);

	/*
	* ipu1_pixel_clk_x clock tree:
	* osc_clk(24M)->pll2_528_bus_main_clk(528M)->
	* pll2_pfd_352M(452.57M)->ldb_dix_clk(64.65M)->
	* ipu1_di_clk_x(64.65M)->ipu1_pixel_clk_x(64.65M)
	*/
	/* pll2_528_bus_main_clk */
	/* divider */
	writel(0x1, ANATOP_BASE_ADDR + 0x34);
	
	/* pll2_pfd_352M */
	/* disable */
	writel(0x1 << 7, ANATOP_BASE_ADDR + 0x104);
	/* divider */
	writel(0x3F, ANATOP_BASE_ADDR + 0x108);
	writel(0x12, ANATOP_BASE_ADDR + 0x104);
	
	/* ldb_dix_clk */
	/* source */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CS2CDR);
	reg &= ~(0x3F << 9);
	reg |= (0x9 << 9);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CS2CDR);
	/* divider */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CSCMR2);
	reg |= (0x3 << 10);
	writel(reg, CCM_BASE_ADDR + CLKCTL_CSCMR2);
	
	/* pll2_pfd_352M */
	/* enable after ldb_dix_clk source is set */
	writel(0x1 << 7, ANATOP_BASE_ADDR + 0x108);

	/* ipu1_di_clk_x */
	/* source */
	reg = readl(CCM_BASE_ADDR + CLKCTL_CHSCCDR);
	reg &= ~0xE07;
	reg |= 0x803;
	writel(reg, CCM_BASE_ADDR + CLKCTL_CHSCCDR);
#endif /* CONFIG_MX6DL */
	
	/* Enable ipu1/ipu1_dix/ldb_dix clocks. */
	if (di == 1) {
		reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR3);
		reg |= 0xC033;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR3);
	} else {
	    reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR3);
		reg |= 0x300F;
		writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR3);
	}
	
	//ret = ipuv3_fb_init(&lvds_xga, di, IPU_PIX_FMT_RGB666,
	//DI_PCLK_LDB, 65000000);
	//if (ret)
	//puts("LCD cannot be configured\n");
	
	/*
	* LVDS0 mux to IPU1 DI0.
	* LVDS1 mux to IPU1 DI1.
	*/
	reg = readl(IOMUXC_BASE_ADDR + 0xC);
	reg &= ~(0x000003C0);
	reg |= 0x00000100;
	writel(reg, IOMUXC_BASE_ADDR + 0xC);

	if (di == 1)
		writel(0x40C, IOMUXC_BASE_ADDR + 0x8);
	else
		writel(0x201, IOMUXC_BASE_ADDR + 0x8);
#else
	/* Turn on LDB0, LDB1, IPU,IPU DI0 clocks */
	reg = readl(&mxc_ccm->CCGR3);
	reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK | MXC_CCM_CCGR3_LDB_DI1_MASK;
	writel(reg, &mxc_ccm->CCGR3);

	/* set LDB0, LDB1 clk select to 011/011 */
	reg = readl(&mxc_ccm->cs2cdr);
	reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK
		 | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
	reg |= (3 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET)
	      | (3 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->cs2cdr);
#endif
//+++WWJ end

	reg = readl(&mxc_ccm->cscmr2);
//+++WWJ begin
#ifdef CONFIG_QM215
	reg &= ~(MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV | MXC_CCM_CSCMR2_LDB_DI1_IPU_DIV);
#else
	reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV | MXC_CCM_CSCMR2_LDB_DI1_IPU_DIV;
#endif
//+++WWJ end
	writel(reg, &mxc_ccm->cscmr2);

//+++WWJ begin 
#ifdef CONFIG_QM215
#else
	reg = readl(&mxc_ccm->chsccdr);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->chsccdr);
#endif

#ifdef CONFIG_QM215
	 reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
			| IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
			| IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
			| IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
			| IOMUXC_GPR2_DATA_WIDTH_CH1_18BIT
			| IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT
			| IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
			| IOMUXC_GPR2_DATA_WIDTH_CH0_18BIT
			| IOMUXC_GPR2_LVDS_CH0_MODE_DISABLED
			| IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
			| IOMUXC_GPR2_SPLIT_MODE_EN_MASK
			| IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0
			| IOMUXC_GPR2_LVDS_CH1_MODE_ENABLED_DI0;

#else
	reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
	     | IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH1_24BIT
	     | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
	     | IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0
	     | IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED;
#endif
//+++WWJ end
	     
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
//+++WWJ begin
	reg &= ~IOMUXC_GPR3_OCRAM_CTL_MASK;
/*
	reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       << IOMUXC_GPR3_LVDS1_MUX_CTL_OFFSET);
*/

#ifdef CONFIG_QM215

	reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK|IOMUXC_GPR3_LVDS0_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
			| (IOMUXC_GPR3_MUX_SRC_IPU1_DI0 << IOMUXC_GPR3_LVDS0_MUX_CTL_MASK);
#endif

#ifdef CONFIG_HK185
	reg = (reg & ~(IOMUXC_GPR3_LVDS0_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
		| (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
			<< IOMUXC_GPR3_LVDS0_MUX_CTL_MASK);
#endif

//+++WWJ end
	writel(reg, &iomux->gpr[3]);

}
#endif /* CONFIG_VIDEO_IPUV3 */

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

//+++WWJ begin
#ifdef CONFIG_RECOVERY

iomux_v3_cfg_t const power_key[] = {
	MX6_PAD_GPIO_18__GPIO7_IO13,
};

#define SABRESD_POWER_KEY IMX_GPIO_NR(7, 13)

int check_recovery_cmd_file(void)
{
	int button_pressed = 0;
	int recovery_mode = 0;
	int file_mode = 0;
	int time = 100;
	u32 reg;

	imx_iomux_v3_setup_multiple_pads(power_key, ARRAY_SIZE(power_key));

	gpio_direction_input(SABRESD_POWER_KEY);

	while(1){
		//reg = readl(GPIO7_BASE_ADDR + GPIO_PSR);
		reg = gpio_get_value(SABRESD_POWER_KEY);
		time ++;
		if(!(time % 10000))
			printf("power key down %ds \n", time/10000);	
		udelay(100);
		/*
		if(reg & (1 << 13)){
			break;
		}*/
		if(reg)
			break;

		if(time == 50000) {
			button_pressed = 1;
			break;
		}
	}
	printf("Checking for mmc recovery disk file ...\n");

	return recovery_mode || button_pressed || file_mode;
}
#endif  /*CONFIG_RECOVERY*/


//+++WWJ end

int board_eth_init(bd_t *bis)
{
	setup_iomux_enet();

	return cpu_eth_init(bis);
}

int board_early_init_f(void)
{
	printf("board_early_init_f\n");

	setup_iomux_uart();
#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_MXC_SPI
	setup_spi();
#endif

	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"sd2",	 MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
	{"sd3",	 MAKE_CFGVAL(0x40, 0x30, 0x00, 0x00)},
	/* 8 bit bus width */
	{"emmc", MAKE_CFGVAL(0x40, 0x38, 0x00, 0x00)},
	{NULL,	 0},
};
#endif

//+++WWJ begin
#ifdef CONFIG_I2C_MXC
#define I2C1_SDA_GPIO5_26_BIT_MASK  (1 << 26)
#define I2C1_SCL_GPIO5_27_BIT_MASK  (1 << 27)
#define I2C2_SCL_GPIO4_12_BIT_MASK  (1 << 12)
#define I2C2_SDA_GPIO4_13_BIT_MASK  (1 << 13)
#define I2C3_SCL_GPIO1_3_BIT_MASK   (1 << 3)
#define I2C3_SDA_GPIO1_6_BIT_MASK   (1 << 6)

#define GPIO_DR                 0x00
#define GPIO_GDIR               0x04
#define GPIO_PSR                0x08

static void setup_i2c(unsigned int module_base)
{
        unsigned int reg;
 
        switch (module_base) {
        case I2C1_BASE_ADDR:
#if defined CONFIG_MX6Q
                /* i2c1 SDA */
                imx_iomux_v3_setup_pad((MX6_PAD_CSI0_DAT8__I2C1_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
       
                /* i2c1 SCL */
                //imx_iomux_v3_setup_pad((MX6_PAD_CSI0_DAT9__I2C1_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));

                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#elif defined CONFIG_MX6DL
                /* i2c1 SDA */
                imx_iomux_v3_setup_pad((MX6_PAD_CSI0_DAT8__I2C1_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                /* i2c1 SCL */
                //imx_iomux_v3_setup_pad((MX6_PAD_CSI0_DAT9__I2C1_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#endif
 
                /* Enable i2c clock */
                reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
                reg |= 0xC0;
                writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);
 
                break;
        case I2C2_BASE_ADDR:
#if defined CONFIG_MX6Q
                /* i2c2 SDA */
                imx_iomux_v3_setup_pad()MX6_PAD_KEY_ROW3__I2C2_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
 
                /* i2c2 SCL */
                imx_iomux_v3_setup_pad((MX6_PAD_KEY_COL3__I2C2_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#elif defined CONFIG_MX6DL
                /* i2c2 SDA */
                imx_iomux_v3_setup_pad((MX6_PAD_KEY_ROW3__I2C2_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
 
                /* i2c2 SCL */
                imx_iomux_v3_setup_pad((MX6_PAD_KEY_COL3__I2C2_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#endif
 
                /* Enable i2c clock */
                reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
                reg |= 0x300;
                writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);
 
                break;
        case I2C3_BASE_ADDR:
#if defined CONFIG_MX6Q
                /* GPIO_3 for I2C3_SCL */
                imx_iomux_v3_setup_pad((MX6_PAD_GPIO_3__I2C3_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                /* GPIO_6 for I2C3_SDA */
                imx_iomux_v3_setup_pad((MX6_PAD_GPIO_6__I2C3_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#elif defined CONFIG_MX6DL
                /* GPIO_3 for I2C3_SCL */
                imx_iomux_v3_setup_pad((MX6_PAD_GPIO_3__I2C3_SCL|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                /* GPIO_6 for I2C3_SDA */
                imx_iomux_v3_setup_pad((MX6_PAD_GPIO_6__I2C3_SDA|MUX_PAD_CTRL(MX6DL_I2C_PAD_CTRL)));
                //imx_iomux_v3_setup_multiple_pads(i2c1_pads, ARRAY_SIZE(i2c1_pads));
#endif
                /* Enable i2c clock */
                reg = readl(CCM_BASE_ADDR + CLKCTL_CCGR2);
                reg |= 0xC00;
                writel(reg, CCM_BASE_ADDR + CLKCTL_CCGR2);
 
                break;
        default:
                printf("Invalid I2C base: 0x%x\n", module_base);
                break;
        }
}

static void mx6q_i2c_gpio_scl_direction(int bus, int output)
{
        u32 reg;
 
        switch (bus){ 
        case 1:
#if defined CONFIG_MX6Q
                //imx_iomux_v3_setup_pad(MX6_PAD_CSI0_DAT9__GPIO5_IO27);
#elif defined CONFIG_MX6DL
                //imx_iomux_v3_setup_pad(MX6_PAD_CSI0_DAT9__GPIO5_IO27);
#endif
                reg = readl(GPIO5_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C1_SCL_GPIO5_27_BIT_MASK;
                else
                        reg &= ~I2C1_SCL_GPIO5_27_BIT_MASK;
                writel(reg, GPIO5_BASE_ADDR + GPIO_GDIR);
                break;
        case 2:
#if defined CONFIG_MX6Q
                imx_iomux_v3_setup_pad(MX6_PAD_KEY_COL3__GPIO4_IO12);
#elif defined CONFIG_MX6DL
                imx_iomux_v3_setup_pad(MX6_PAD_KEY_COL3__GPIO4_IO12);
#endif
                reg = readl(GPIO4_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C2_SCL_GPIO4_12_BIT_MASK;
                else
                        reg &= ~I2C2_SCL_GPIO4_12_BIT_MASK;
                writel(reg, GPIO4_BASE_ADDR + GPIO_GDIR);
                break;
        case 3:
#if defined CONFIG_MX6Q
                imx_iomux_v3_setup_pad(MX6_PAD_GPIO_3__GPIO1_IO03);
#elif defined CONFIG_MX6DL
                imx_iomux_v3_setup_pad(MX6_PAD_GPIO_3__GPIO1_IO03);
#endif
                reg = readl(GPIO1_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C3_SCL_GPIO1_3_BIT_MASK;
                else
                        reg &= I2C3_SCL_GPIO1_3_BIT_MASK;
                writel(reg, GPIO1_BASE_ADDR + GPIO_GDIR);
                break;
        }
}

/* set 1 to output, sent 0 to input */
static void mx6q_i2c_gpio_sda_direction(int bus, int output)
{
        u32 reg;
 
        switch (bus) {
        case 1:
#if defined CONFIG_MX6Q
                imx_iomux_v3_setup_pad(MX6_PAD_CSI0_DAT8__GPIO5_IO26);
#elif defined CONFIG_MX6DL
                imx_iomux_v3_setup_pad(MX6_PAD_CSI0_DAT8__GPIO5_IO26);
#endif
                reg = readl(GPIO5_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C1_SDA_GPIO5_26_BIT_MASK;
                else
                        reg &= ~I2C1_SDA_GPIO5_26_BIT_MASK;
                writel(reg, GPIO5_BASE_ADDR + GPIO_GDIR);
                break;
        case 2:
#if defined CONFIG_MX6Q
                imx_iomux_v3_setup_pad(MX6_PAD_KEY_ROW3__GPIO4_IO13);
#elif defined CONFIG_MX6DL
                imx_iomux_v3_setup_pad(MX6_PAD_KEY_ROW3__GPIO4_IO13);
#endif
                reg = readl(GPIO4_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C2_SDA_GPIO4_13_BIT_MASK;
                else
                        reg &= ~I2C2_SDA_GPIO4_13_BIT_MASK;
                writel(reg, GPIO4_BASE_ADDR + GPIO_GDIR);
        case 3:
#if defined CONFIG_MX6Q
				imx_iomux_v3_setup_pad(MX6_PAD_GPIO_6__GPIO1_IO06);
#elif defined CONFIG_MX6DL
                imx_iomux_v3_setup_pad(MX6_PAD_GPIO_6__GPIO1_IO06);
#endif
                reg = readl(GPIO1_BASE_ADDR + GPIO_GDIR);
                if (output)
                        reg |= I2C3_SDA_GPIO1_6_BIT_MASK;
                else
                        reg &= ~I2C3_SDA_GPIO1_6_BIT_MASK;
                writel(reg, GPIO1_BASE_ADDR + GPIO_GDIR);
        default:
                break;
        }
}

/* set 1 to high 0 to low */
static void mx6q_i2c_gpio_scl_set_level(int bus, int high)
{
        u32 reg;
 
        switch (bus) {
        case 1:
                reg = readl(GPIO5_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C1_SCL_GPIO5_27_BIT_MASK;
                else
                        reg &= ~I2C1_SCL_GPIO5_27_BIT_MASK;
                writel(reg, GPIO5_BASE_ADDR + GPIO_DR);
                break;
        case 2:
                reg = readl(GPIO4_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C2_SCL_GPIO4_12_BIT_MASK;
                else
                        reg &= ~I2C2_SCL_GPIO4_12_BIT_MASK;
                writel(reg, GPIO4_BASE_ADDR + GPIO_DR);
                break;
        case 3:
                reg = readl(GPIO1_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C3_SCL_GPIO1_3_BIT_MASK;
                else
                        reg &= ~I2C3_SCL_GPIO1_3_BIT_MASK;
                writel(reg, GPIO1_BASE_ADDR + GPIO_DR);
                break;
        }
}
 
/* set 1 to high 0 to low */
static void mx6q_i2c_gpio_sda_set_level(int bus, int high)
{
        u32 reg;
 
        switch (bus) {
        case 1:
                reg = readl(GPIO5_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C1_SDA_GPIO5_26_BIT_MASK;
                else
                        reg &= ~I2C1_SDA_GPIO5_26_BIT_MASK;
                writel(reg, GPIO5_BASE_ADDR + GPIO_DR);
                break;
        case 2:
                reg = readl(GPIO4_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C2_SDA_GPIO4_13_BIT_MASK;
                else
                        reg &= ~I2C2_SDA_GPIO4_13_BIT_MASK;
                writel(reg, GPIO4_BASE_ADDR + GPIO_DR);
                break;
        case 3:
                reg = readl(GPIO1_BASE_ADDR + GPIO_DR);
                if (high)
                        reg |= I2C3_SDA_GPIO1_6_BIT_MASK;
                else
                        reg &= ~I2C3_SDA_GPIO1_6_BIT_MASK;
                writel(reg, GPIO1_BASE_ADDR + GPIO_DR);
                break;
        }
}

static int mx6q_i2c_gpio_check_sda(int bus)
{
        u32 reg;
        int result = 0;
 
        switch (bus) {
        case 1:
                reg = readl(GPIO5_BASE_ADDR + GPIO_PSR);
                result = !!(reg & I2C1_SDA_GPIO5_26_BIT_MASK);
                break;
        case 2:
                reg = readl(GPIO4_BASE_ADDR + GPIO_PSR);
                result = !!(reg & I2C2_SDA_GPIO4_13_BIT_MASK);
                break;
        case 3:
                reg = readl(GPIO1_BASE_ADDR + GPIO_PSR);
                result = !!(reg & I2C3_SDA_GPIO1_6_BIT_MASK);
                break;
        }
 
        return result;
}

/* Random reboot cause i2c SDA low issue:
+  * the i2c bus busy because some device pull down the I2C SDA
+  * line. This happens when Host is reading some byte from slave, and
+  * then host is reset/reboot. Since in this case, device is
+  * controlling i2c SDA line, the only thing host can do this give the
+  * clock on SCL and sending NAK, and STOP to finish this
+  * transaction.
+  *
+  * How to fix this issue:
+  * detect if the SDA was low on bus send 8 dummy clock, and 1
+  * clock + NAK, and STOP to finish i2c transaction the pending
+  * transfer.
+  */
int i2c_bus_recovery(void)
{
       int i, bus, result = 0;

       for (bus = 1; bus <= 3; bus++) {
               mx6q_i2c_gpio_sda_direction(bus, 0);

               if (mx6q_i2c_gpio_check_sda(bus) == 0) {
                       printf("i2c: I2C%d SDA is low, start i2c recovery...\n", bus);
                       mx6q_i2c_gpio_scl_direction(bus, 1);
                       mx6q_i2c_gpio_scl_set_level(bus, 1);
                       __udelay(10000);

                       for (i = 0; i < 9; i++) {
                               mx6q_i2c_gpio_scl_set_level(bus, 1);
                               __udelay(5);
                               mx6q_i2c_gpio_scl_set_level(bus, 0);
                               __udelay(5);
                       }

                       /* 9th clock here, the slave should already
                          release the SDA, we can set SDA as high to
                          a NAK.*/
                       mx6q_i2c_gpio_sda_direction(bus, 1);
                       mx6q_i2c_gpio_sda_set_level(bus, 1);
                       __udelay(1); /* Pull up SDA first */
                       mx6q_i2c_gpio_scl_set_level(bus, 1);
                       __udelay(5); /* plus pervious 1 us */
                       mx6q_i2c_gpio_scl_set_level(bus, 0);
                       __udelay(5);
                       mx6q_i2c_gpio_sda_set_level(bus, 0);
                       __udelay(5);
                       mx6q_i2c_gpio_scl_set_level(bus, 1);
                       __udelay(5);
                       /* Here: SCL is high, and SDA from low to high, it's a
                        * stop condition */
                       mx6q_i2c_gpio_sda_set_level(bus, 1);
                       __udelay(5);

                       mx6q_i2c_gpio_sda_direction(bus, 0);
                       if (mx6q_i2c_gpio_check_sda(bus) == 1)
                               printf("I2C%d Recovery success\n", bus);
                       else {
                               printf("I2C%d Recovery failed, I2C1 SDA still low!!!\n", bus);
                               result |= 1 << bus;
                       }
               }

				/* configure back to i2c */
               switch (bus) {
               case 1:
                       setup_i2c(I2C1_BASE_ADDR);
                       break;
               case 2:
                       setup_i2c(I2C2_BASE_ADDR);
                       break;
               case 3:
                       setup_i2c(I2C3_BASE_ADDR);
                       break;
               }
       }

       return result;
}

#endif
//+++WWJ end

int board_late_init(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	//+++MQ
	setup_iomux_pin();
	//+++MQ

//+++WWJ begin
#ifdef CONFIG_I2C_MXC
       setup_i2c(CONFIG_SYS_I2C_PORT);
       //i2c_bus_recovery();
       i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
#endif
//+++WWJ end

	return 0;
}

int checkboard(void)
{
	puts("Board: MX6-SabreSD\n");
	return 0;
}
