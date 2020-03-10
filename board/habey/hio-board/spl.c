/*
 * Copyright (C) 2014 Wandboard
 * Author: Tungyi Lin <tungyilin1127@gmail.com>
 *         Richard Hu <hakahu@gmail.com>
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/video.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <asm/arch/crm_regs.h>
#include <asm/io.h>
#include <asm/arch/sys_proto.h>
#include <spl.h>

DECLARE_GLOBAL_DATA_PTR;

#if defined(CONFIG_SPL_BUILD)
#include <asm/arch/mx6-ddr.h>
/*
 * Driving strength:
 *   0x30 == 40 Ohm
 *   0x28 == 48 Ohm
 */

#define IMX6DQ_DRIVE_STRENGTH		0x30
#define IMX6SDL_DRIVE_STRENGTH		0x28

//static char g_ddr_version[16];

/* configure MX6Q/DUAL mmdc DDR io registers */
static struct mx6dq_iomux_ddr_regs mx6dq_ddr_ioregs = {
	.dram_sdclk_0 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdclk_1 = IMX6DQ_DRIVE_STRENGTH,
	.dram_cas = IMX6DQ_DRIVE_STRENGTH,
	.dram_ras = IMX6DQ_DRIVE_STRENGTH,
	.dram_reset = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdcke0 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdcke1 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdba2 = 0x00000000,
	.dram_sdodt0 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdodt1 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs0 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs1 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs2 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs3 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs4 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs5 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs6 = IMX6DQ_DRIVE_STRENGTH,
	.dram_sdqs7 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm0 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm1 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm2 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm3 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm4 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm5 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm6 = IMX6DQ_DRIVE_STRENGTH,
	.dram_dqm7 = IMX6DQ_DRIVE_STRENGTH,
};

/* configure MX6Q/DUAL mmdc GRP io registers */
static struct mx6dq_iomux_grp_regs mx6dq_grp_ioregs = {
	.grp_ddr_type = 0x000c0000,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_ddrpke = 0x00000000,
	.grp_addds = IMX6DQ_DRIVE_STRENGTH,
	.grp_ctlds = IMX6DQ_DRIVE_STRENGTH,
	.grp_ddrmode = 0x00020000,
	.grp_b0ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b1ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b2ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b3ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b4ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b5ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b6ds = IMX6DQ_DRIVE_STRENGTH,
	.grp_b7ds = IMX6DQ_DRIVE_STRENGTH,
};

/* configure MX6SOLO/DUALLITE mmdc DDR io registers */
struct mx6sdl_iomux_ddr_regs mx6sdl_ddr_ioregs = {
	.dram_sdclk_0 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdclk_1 = IMX6SDL_DRIVE_STRENGTH,
	.dram_cas = IMX6SDL_DRIVE_STRENGTH,
	.dram_ras = IMX6SDL_DRIVE_STRENGTH,
	.dram_reset = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdcke0 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdcke1 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdba2 = 0x00000000,
	.dram_sdodt0 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdodt1 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs0 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs1 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs2 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs3 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs4 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs5 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs6 = IMX6SDL_DRIVE_STRENGTH,
	.dram_sdqs7 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm0 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm1 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm2 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm3 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm4 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm5 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm6 = IMX6SDL_DRIVE_STRENGTH,
	.dram_dqm7 = IMX6SDL_DRIVE_STRENGTH,
};

/* configure MX6SOLO/DUALLITE mmdc GRP io registers */
struct mx6sdl_iomux_grp_regs mx6sdl_grp_ioregs = {
	.grp_ddr_type = 0x000c0000,
	.grp_ddrmode_ctl = 0x00020000,
	.grp_ddrpke = 0x00000000,
	.grp_addds = IMX6SDL_DRIVE_STRENGTH,
	.grp_ctlds = IMX6SDL_DRIVE_STRENGTH,
	.grp_ddrmode = 0x00020000,
	.grp_b0ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b1ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b2ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b3ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b4ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b5ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b6ds = IMX6SDL_DRIVE_STRENGTH,
	.grp_b7ds = IMX6SDL_DRIVE_STRENGTH,
};

/* H5T04G63AFR-PB */
static struct mx6_ddr3_cfg h5t04g63afr = {
	.mem_speed = 1066,
	.density = 4,
	.width = 16,
	.banks = 8,
	.rowaddr = 15,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1312,
	.trcmin = 5062,
	.trasmin = 3750,
};

/* H5TQ2G63DFR-H9 */
static struct mx6_ddr3_cfg h5tq2g63dfr = {
	.mem_speed = 1333,
	.density = 2,
	.width = 16,
	.banks = 8,
	.rowaddr = 14,
	.coladdr = 10,
	.pagesz = 2,
	.trcd = 1350,
	.trcmin = 4950,
	.trasmin = 3600,
};

static struct mx6_mmdc_calibration mx6q_2g_mmdc_calib = {
	.p0_mpwldectrl0 = 0x0024001F,
	.p0_mpwldectrl1 = 0x002F0027,
	.p1_mpwldectrl0 = 0x00140028,
	.p1_mpwldectrl1 = 0x000F001E,
	.p0_mpdgctrl0 = 0x03440358,
	.p0_mpdgctrl1 = 0x0338032C,
	.p1_mpdgctrl0 = 0x0338034C,
	.p1_mpdgctrl1 = 0x03340268,
	.p0_mprddlctl = 0x46363C44,
	.p1_mprddlctl = 0x443C364C,
	.p0_mpwrdlctl = 0x2E3A3C32,
	.p1_mpwrdlctl = 0x42304238,
};

/* DDR 64bit 2GB */
static struct mx6_ddr_sysinfo mem_q = {
	.dsize		= 2,
	.cs1_mirror	= 0,
	/* config for full 4GB range so that get_mem_size() works */
	.cs_density	= 32,
	.ncs		= 1,
	.bi_on		= 1,
	.rtt_nom	= 1,
	.rtt_wr		= 0,
	.ralat		= 5,
	.walat		= 0,
	.mif3_mode	= 3,
	.rst_to_cke	= 0x23,
	.sde_to_rst	= 0x10,
};

static struct mx6_mmdc_calibration mx6dl_1g_mmdc_calib = {
	.p0_mpwldectrl0 = 0x001f001f,
	.p0_mpwldectrl1 = 0x001f001f,
	.p1_mpwldectrl0 = 0x001f001f,
	.p1_mpwldectrl1 = 0x001f001f,
	.p0_mpdgctrl0 = 0x420e020e,
	.p0_mpdgctrl1 = 0x02000200,
	.p1_mpdgctrl0 = 0x42020202,
	.p1_mpdgctrl1 = 0x01720172,
	.p0_mprddlctl = 0x494c4f4c,
	.p1_mprddlctl = 0x4a4c4c49,
	.p0_mpwrdlctl = 0x3f3f3133,
	.p1_mpwrdlctl = 0x39373f2e,
};

#if 0
static struct mx6_mmdc_calibration mx6s_512m_mmdc_calib = {
	.p0_mpwldectrl0 = 0x0040003c,
	.p0_mpwldectrl1 = 0x0032003e,
	.p0_mpdgctrl0 = 0x42350231,
	.p0_mpdgctrl1 = 0x021a0218,
	.p0_mprddlctl = 0x4b4b4e49,
	.p0_mpwrdlctl = 0x3f3f3035,
};
#endif

/* DDR 64bit 1GB */
static struct mx6_ddr_sysinfo mem_dl = {
	.dsize		= 2,
	.cs1_mirror	= 0,
	/* config for full 4GB range so that get_mem_size() works */
	.cs_density	= 32,
	.ncs		= 1,
	.bi_on		= 1,
	.rtt_nom	= 1,
	.rtt_wr		= 0,
	.ralat		= 5,
	.walat		= 0,
	.mif3_mode	= 3,
	.rst_to_cke	= 0x23,
	.sde_to_rst	= 0x10,
};

#if 0
/* DDR 32bit 512MB */
static struct mx6_ddr_sysinfo mem_s = {
	.dsize		= 1,
	.cs1_mirror	= 0,
	/* config for full 4GB range so that get_mem_size() works */
	.cs_density	= 32,
	.ncs		= 1,
	.bi_on		= 1,
	.rtt_nom	= 1,
	.rtt_wr		= 0,
	.ralat		= 5,
	.walat		= 0,
	.mif3_mode	= 3,
	.rst_to_cke	= 0x23,
	.sde_to_rst	= 0x10,
};
#endif

static void ccgr_init(void)
{
	struct mxc_ccm_reg *ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	writel(0x00C03F3F, &ccm->CCGR0);
	writel(0x0030FC03, &ccm->CCGR1);
	writel(0x0FFFC000, &ccm->CCGR2);
	writel(0x3FF00000, &ccm->CCGR3);
	writel(0x00FFF300, &ccm->CCGR4);
	writel(0x0F0000C3, &ccm->CCGR5);
	writel(0x000003FF, &ccm->CCGR6);
}

static void gpr_init(void)
{
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;

	/* enable AXI cache for VDOA/VPU/IPU */
	writel(0xF00000CF, &iomux->gpr[4]);
	/* set IPU AXI-id0 Qos=0xf(bypass) AXI-id1 Qos=0x7 */
	writel(0x007F007F, &iomux->gpr[6]);
	writel(0x007F007F, &iomux->gpr[7]);
}

//u32->u8
static uint32_t u8_to_u32(uint8_t* u8)
{
	uint32_t tmp = 0;
	uint32_t tmp1 = 0;
	
	tmp1 = u8[0];
	tmp = tmp + (tmp1 << 24);
	
	tmp1 = u8[1];
	tmp = tmp + (tmp1 << 16);	
	
	tmp1 = u8[2];
	tmp = tmp + (tmp1 << 8);
	
	tmp1 = u8[3];
	tmp = tmp + tmp1;

	return tmp;
}

//u16->u8
static uint16_t u8_to_u16(uint8_t* u8)
{
	uint16_t tmp;
	uint16_t tmp1 = 0;
	
	tmp1 = u8[0];
	tmp = tmp + (tmp1 << 8);
	
	tmp1 = u8[1];
	tmp = tmp + tmp1;

	return tmp;
}

int do_hio_ddr_params(void)
{
	printf("hio_ddr_params: \n");
	//printf("ddr ic:%s \n", g_ddr_version);
	printf("mx6_mmdc_calibration.p0_mpwldectrl0:%x \n", mx6dl_1g_mmdc_calib.p0_mpwldectrl0);
	printf("mx6_mmdc_calibration.p0_mpwldectrl1:%x \n", mx6dl_1g_mmdc_calib.p0_mpwldectrl1);
	printf("mx6_mmdc_calibration.p1_mpwldectrl0:%x \n", mx6dl_1g_mmdc_calib.p1_mpwldectrl0);
	printf("mx6_mmdc_calibration.p1_mpwldectrl1:%x \n", mx6dl_1g_mmdc_calib.p1_mpwldectrl1);
	
	printf("mx6_mmdc_calibration.p0_mpdgctrl0:%x \n", mx6dl_1g_mmdc_calib.p0_mpdgctrl0);
	printf("mx6_mmdc_calibration.p0_mpdgctrl1:%x \n", mx6dl_1g_mmdc_calib.p0_mpdgctrl1);
	printf("mx6_mmdc_calibration.p1_mpdgctrl0:%x \n", mx6dl_1g_mmdc_calib.p1_mpdgctrl0);
	printf("mx6_mmdc_calibration.p1_mpdgctrl1:%x \n", mx6dl_1g_mmdc_calib.p1_mpdgctrl1);
	
	printf("mx6_mmdc_calibration.p0_mprddlctl:%x \n", mx6dl_1g_mmdc_calib.p0_mprddlctl);
	printf("mx6_mmdc_calibration.p1_mprddlctl:%x \n", mx6dl_1g_mmdc_calib.p1_mprddlctl);
	
	printf("mx6_mmdc_calibration.p0_mpwrdlctl:%x \n", mx6dl_1g_mmdc_calib.p0_mpwrdlctl);
	printf("mx6_mmdc_calibration.p1_mpwrdlctl:%x \n", mx6dl_1g_mmdc_calib.p1_mpwrdlctl);

	//
	printf("mx6_ddr3_cfg.mem_speed:%x \n", h5tq2g63dfr.mem_speed);
	printf("mx6_ddr3_cfg.density:%x \n", h5tq2g63dfr.density);
	printf("mx6_ddr3_cfg.width:%x \n", h5tq2g63dfr.width);
	printf("mx6_ddr3_cfg.banks:%x \n", h5tq2g63dfr.banks);
	
	printf("mx6_ddr3_cfg.rowaddr:%x \n", h5tq2g63dfr.rowaddr);
	printf("mx6_ddr3_cfg.coladdr:%x \n", h5tq2g63dfr.coladdr);
	printf("mx6_ddr3_cfg.pagesz:%x \n", h5tq2g63dfr.pagesz);
	printf("mx6_ddr3_cfg.trcd:%x \n", h5tq2g63dfr.trcd);
	
	printf("mx6_ddr3_cfg.trcmin:%x \n", h5tq2g63dfr.trcmin);
	printf("mx6_ddr3_cfg.trasmin:%x \n", h5tq2g63dfr.trasmin);
	
	return 0;
}

static uint8_t mx6_mmdc_read_struct(void)
{
	uint8_t cmd;
	uint8_t buff[4];
	uint8_t i;
	uint8_t j;
	struct mx6_mmdc_calibration tmp_mmc;
	
	for (j=1; j<=12; j++)
	{
		
		cmd = j;
		
		if (i2c_write(0x60, 0x90, 1, &cmd, 1) == -1)
		{
			return 1;
		}
		mdelay(100);
		
		for (i=0; i<4; i++)
		{
			mdelay(10);
			if (i2c_read(0x60, 0x30 + i, 1, &buff[i], 1) == -1)
			{
				return 1;
			}
		}
	
		if (j == 1)
			tmp_mmc.p0_mpwldectrl0 = u8_to_u32(buff);
		else if (j == 2)
			tmp_mmc.p0_mpwldectrl1 = u8_to_u32(buff);
		else if (j == 3)
			tmp_mmc.p1_mpwldectrl0 = u8_to_u32(buff);
		else if (j == 4)
			tmp_mmc.p1_mpwldectrl1 = u8_to_u32(buff);
		else if (j == 5)
			tmp_mmc.p0_mpdgctrl0 = u8_to_u32(buff);
		else if (j == 6)
			tmp_mmc.p0_mpdgctrl1 = u8_to_u32(buff);
		else if (j == 7)
			tmp_mmc.p1_mpdgctrl0 = u8_to_u32(buff);
		else if (j == 8)
			tmp_mmc.p1_mpdgctrl1 = u8_to_u32(buff);
		else if (j == 9)	
			tmp_mmc.p0_mprddlctl = u8_to_u32(buff);
		else if (j == 10)
			tmp_mmc.p1_mprddlctl = u8_to_u32(buff);
		else if (j == 11)
			tmp_mmc.p0_mpwrdlctl = u8_to_u32(buff);
		else if (j == 12)	
			tmp_mmc.p1_mpwrdlctl = u8_to_u32(buff);			
	}	

	//printf("mx6_mmdc_read_struct...4.1 \n");
	memcpy(&mx6dl_1g_mmdc_calib, &tmp_mmc, sizeof(struct mx6_mmdc_calibration));
	
	//printf("mx6_mmdc_read_struct...4 \n");
	
	return 0;
}

static uint8_t mx6_ddr3_read_struct(void)
{
	uint8_t cmd;
	uint8_t buff[4];
	uint8_t i;
	uint8_t j;
	struct mx6_ddr3_cfg tmp_mmc;
	
	for (j=13; j<=22; j++)
	{
		
		cmd = j;
		
		if (i2c_write(0x60, 0x90, 1, &cmd, 1) == -1)
		{
			return 1;
		}
		mdelay(100);
		
		for (i=0; i<4; i++)
		{
			mdelay(10);
			if (i2c_read(0x60, 0x30 + i, 1, &buff[i], 1) == -1)
			{
				return 1;
			}	
		}
	
		if (j == 13)
			tmp_mmc.mem_speed = (u16)(u8_to_u32(buff));
		else if (j == 14)
			tmp_mmc.density = (u8)(u8_to_u32(buff));
		else if (j == 15)
			tmp_mmc.width = (u8)(u8_to_u32(buff));
		else if (j == 16)
			tmp_mmc.banks = (u8)(u8_to_u32(buff));
		else if (j == 17)
			tmp_mmc.rowaddr = (u8)(u8_to_u32(buff));
		else if (j == 18)
			tmp_mmc.coladdr = (u8)(u8_to_u32(buff));
		else if (j == 19)
			tmp_mmc.pagesz = (u8)(u8_to_u32(buff));
		else if (j == 20)
			tmp_mmc.trcd = (u16)(u8_to_u32(buff));
		else if (j == 21)	
			tmp_mmc.trcmin = (u16)(u8_to_u32(buff));
		else if (j == 22)
			tmp_mmc.trasmin = (u16)(u8_to_u32(buff));		
	}	

	
	memcpy(&h5tq2g63dfr, &tmp_mmc, sizeof(struct mx6_ddr3_cfg));
	
	return 0;
}

#if 0
static uint8_t mx6_ddr3_version_read_struct()
{
	uint8_t cmd;
	uint8_t buff;
	uint8_t i;

	memset(g_ddr_version, 0x00, 16);
	cmd = 23;
	if (i2c_write(0x60, 0x90, 1, &cmd, 1))
	{
		return 1;
	}
	mdelay(100);
	
	for(i=0; i<15; i++)
	{
		mdelay(10);
		if (i2c_read(0x60, 0x30 + i, 1, &buff, 1))
		{
			return 1;
		}
		g_ddr_version[i] = buff;	
	}

	return 0;	
}
#endif

static void spl_dram_init(void)
{
	int flag = 0;
	int tmp_flag = 0;
	
	udelay(100);
	i2c_set_bus_num(0x01);
	if (i2c_probe(0x60)) 
	{
		flag = 1;
		printf("\nError: couldn't find MCU EEPROM device\n");
	}
	else
	{
		mdelay(50);
		tmp_flag = tmp_flag || mx6_mmdc_read_struct();
		
		mdelay(50);
		tmp_flag = tmp_flag || mx6_ddr3_read_struct();

		mdelay(50);
		//tmp_flag = tmp_flag || mx6_ddr3_version_read_struct();
		
		if (tmp_flag == 1)
		{
			flag = 1;
		}
		else
		{
			flag = 0;
		}		
	}	
	
	printf("spl_dram_init flag : %d, h5tq2g63dfr.density: %d \n", flag, h5tq2g63dfr.density);
	if ((h5tq2g63dfr.density != 2) && (flag == 0))
		flag = 1;
	
	if (flag == 1)
	{
		//strcpy(g_ddr_version, "default ddr ic");
		
		/* H5TQ2G63DFR-H9 */
		h5tq2g63dfr.mem_speed = 1333;
		h5tq2g63dfr.density = 2;
		h5tq2g63dfr.width = 16;
		h5tq2g63dfr.banks = 8;
		h5tq2g63dfr.rowaddr = 14;
		h5tq2g63dfr.coladdr = 10;
		h5tq2g63dfr.pagesz = 2;
		h5tq2g63dfr.trcd = 1350;
		h5tq2g63dfr.trcmin = 4950;
		h5tq2g63dfr.trasmin = 3600;

		//
		mx6dl_1g_mmdc_calib.p0_mpwldectrl0 = 0x001f001f;
		mx6dl_1g_mmdc_calib.p0_mpwldectrl1 = 0x001f001f;
		mx6dl_1g_mmdc_calib.p1_mpwldectrl0 = 0x001f001f;
		mx6dl_1g_mmdc_calib.p1_mpwldectrl1 = 0x001f001f;
		mx6dl_1g_mmdc_calib.p0_mpdgctrl0 = 0x420e020e;
		mx6dl_1g_mmdc_calib.p0_mpdgctrl1 = 0x02000200;
		mx6dl_1g_mmdc_calib.p1_mpdgctrl0 = 0x42020202;
		mx6dl_1g_mmdc_calib.p1_mpdgctrl1 = 0x01720172;
		mx6dl_1g_mmdc_calib.p0_mprddlctl = 0x494c4f4c;
		mx6dl_1g_mmdc_calib.p1_mprddlctl = 0x4a4c4c49;
		mx6dl_1g_mmdc_calib.p0_mpwrdlctl = 0x3f3f3133;
		mx6dl_1g_mmdc_calib.p1_mpwrdlctl = 0x39373f2e;			
	}

	//printf
	do_hio_ddr_params();
	
	//ddr init(Only supports dl)
	if (is_cpu_type(MXC_CPU_MX6DL)) {
		mx6sdl_dram_iocfg(64, &mx6sdl_ddr_ioregs, &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&mem_dl, &mx6dl_1g_mmdc_calib, &h5tq2g63dfr);
	} else if (is_cpu_type(MXC_CPU_MX6Q)) {
		mx6dq_dram_iocfg(64, &mx6dq_ddr_ioregs, &mx6dq_grp_ioregs);
		mx6_dram_cfg(&mem_q, &mx6q_2g_mmdc_calib, &h5t04g63afr);
		//mx6_dram_cfg(&mem_dl, &mx6dl_1g_mmdc_calib, &h5tq2g63dfr);
	}
}

#if 0
static void spl_dram_init(void)
{
	//add ben i2c
	uint8_t buff;
	
	udelay(100);
	i2c_set_bus_num(0x01);
	if (i2c_probe(0x60)) {
		printf("\nError: couldn't find MCU EEPROM device\n");
		//return ;
	}
/*	
	buff = 0x44;
	if (i2c_write(0xc0, 0x60, 1, &buff, 1))
	{
		printf("\nError reading data from EEPROM:%d\n", buff);
		//return ;
	}
*/	
	buff = 0x00;
	udelay(100);
	if (i2c_read(0x60, 0x30, 1, &buff, 1))
	{
		printf("\nError reading data from EEPROM:%d\n", buff);
		//return ;
	}
	else
	{
		printf("\n reading data from EEPROM:%d\n", buff);
	}	
	
	if (is_cpu_type(MXC_CPU_MX6SOLO)) {
		mx6sdl_dram_iocfg(32, &mx6sdl_ddr_ioregs, &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&mem_s, &mx6s_512m_mmdc_calib, &h5tq2g63dfr);
	} else if (is_cpu_type(MXC_CPU_MX6DL)) {
		mx6sdl_dram_iocfg(64, &mx6sdl_ddr_ioregs, &mx6sdl_grp_ioregs);
		mx6_dram_cfg(&mem_dl, &mx6dl_1g_mmdc_calib, &h5tq2g63dfr);
	} else if (is_cpu_type(MXC_CPU_MX6Q)) {
		mx6dq_dram_iocfg(64, &mx6dq_ddr_ioregs, &mx6dq_grp_ioregs);
		mx6_dram_cfg(&mem_q, &mx6q_2g_mmdc_calib, &h5t04g63afr);
	}

	udelay(100);
}
#endif

//int do_hio_ddr_params(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
//{
//	printf("hio_ddr_params: \n");
//	printf("mx6_mmdc_calibration.p0_mpwldectrl0:%x \n", mx6dl_1g_mmdc_calib.p0_mpwldectrl0);
//	printf("mx6_mmdc_calibration.p0_mpwldectrl1:%x \n", mx6dl_1g_mmdc_calib.p0_mpwldectrl1);
//	printf("mx6_mmdc_calibration.p1_mpwldectrl0:%x \n", mx6dl_1g_mmdc_calib.p1_mpwldectrl0);
//	printf("mx6_mmdc_calibration.p1_mpwldectrl1:%x \n", mx6dl_1g_mmdc_calib.p1_mpwldectrl1);
//	
//	printf("mx6_mmdc_calibration.p0_mpdgctrl0:%x \n", mx6dl_1g_mmdc_calib.p0_mpdgctrl0);
//	printf("mx6_mmdc_calibration.p0_mpdgctrl1:%x \n", mx6dl_1g_mmdc_calib.p0_mpdgctrl1);
//	printf("mx6_mmdc_calibration.p1_mpdgctrl0:%x \n", mx6dl_1g_mmdc_calib.p1_mpdgctrl0);
//	printf("mx6_mmdc_calibration.p1_mpdgctrl1:%x \n", mx6dl_1g_mmdc_calib.p1_mpdgctrl1);
//	
//	printf("mx6_mmdc_calibration.p0_mprddlctl:%x \n", mx6dl_1g_mmdc_calib.p0_mprddlctl);
//	printf("mx6_mmdc_calibration.p1_mprddlctl:%x \n", mx6dl_1g_mmdc_calib.p1_mprddlctl);
//	
//	printf("mx6_mmdc_calibration.p0_mpwrdlctl:%x \n", mx6dl_1g_mmdc_calib.p0_mpwrdlctl);
//	printf("mx6_mmdc_calibration.p1_mpwrdlctl:%x \n", mx6dl_1g_mmdc_calib.p1_mpwrdlctl);
//	
//	return 0;
//}

//U_BOOT_CMD(
//	menu, 5, 0,	do_hio_ddr_params,
//	"For internal use only",
//	"- Do not use"
//);

void board_init_f(ulong dummy)
{
	ccgr_init();

	/* setup AIPS and disable watchdog */
	arch_cpu_init();

	gpr_init();

	/* iomux */
	board_early_init_f();

	/* setup GP timer */
	timer_init();

	/* UART clocks enabled and gd valid - init serial console */
	preloader_console_init();

	/* DDR initialization */
	spl_dram_init();
	
	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* load/boot image from boot device */
	board_init_r(NULL, 0);
}

void reset_cpu(ulong addr)
{
}
#endif
