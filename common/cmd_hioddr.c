#include <common.h>
#include <config.h>
#include <command.h>
#include <i2c.h>

struct mx6_ddr3_cfg {
    u16 mem_speed;  /* ie 1600 for DDR3-1600 (800,1066,1333,1600) */
    u8 density; /* chip density (Gb) (1,2,4,8) */
    u8 width;   /* bus width (bits) (4,8,16) */
    u8 banks;   /* number of banks */
    u8 rowaddr; /* row address bits (11-16)*/
    u8 coladdr; /* col address bits (9-12) */
    u8 pagesz;  /* page size (K) (1-2) */
    u16 trcd;   /* tRCD=tRP=CL (ns*100) */
    u16 trcmin; /* tRC min (ns*100) */
    u16 trasmin;    /* tRAS min (ns*100) */
    u8 SRT;     /* self-refresh temperature: 0=normal, 1=extended */
};
struct mx6_mmdc_calibration {
    /* write leveling calibration */
    u32 p0_mpwldectrl0;
    u32 p0_mpwldectrl1;
    u32 p1_mpwldectrl0;
    u32 p1_mpwldectrl1;
    /* read DQS gating */
    u32 p0_mpdgctrl0;
    u32 p0_mpdgctrl1;
    u32 p1_mpdgctrl0;
    u32 p1_mpdgctrl1;
    /* read delay */
    u32 p0_mprddlctl;
    u32 p1_mprddlctl;
    /* write delay */
    u32 p0_mpwrdlctl;
    u32 p1_mpwrdlctl;
};

static struct mx6_ddr3_cfg h5tq2g63dfr;
static struct mx6_mmdc_calibration mx6dl_1g_mmdc_calib;
static char g_ddr_version[32];

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

int print_hio_ddr_params(void)
{
	printf("hio_ddr_params: \n");
	printf("ddr ic:%s \n", g_ddr_version);
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
		
		if (i2c_write(0x60, 0x90, 1, &cmd, 1))
		{
			return 1;
		}
		mdelay(100);
		
		for (i=0; i<4; i++)
		{
			mdelay(10);
			if (i2c_read(0x60, 0x30 + i, 1, &buff[i], 1))
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
		
		if (i2c_write(0x60, 0x90, 1, &cmd, 1))
		{
			return 1;
		}
		mdelay(100);
		
		for (i=0; i<4; i++)
		{
			mdelay(10);
			if (i2c_read(0x60, 0x30 + i, 1, &buff[i], 1))
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
	
	for(i=0; i<32; i++)
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

static int do_hio_ddr_params(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	puts ("do_hio_ddr_params init\n");
	int tmp_flag = 0;
	
	i2c_set_bus_num(0x01);
	if (i2c_probe(0x60)) 
	{
		puts("\nError: couldn't find MCU EEPROM device\n");
		return -1;
	}
	
	mdelay(10);
	tmp_flag = tmp_flag || mx6_mmdc_read_struct();
		
	mdelay(10);
	tmp_flag = tmp_flag || mx6_ddr3_read_struct();

	mdelay(10);
	tmp_flag = tmp_flag || mx6_ddr3_version_read_struct();	
	
	print_hio_ddr_params();
	puts ("do_hio_ddr_params done\n");
	return 0;
}

U_BOOT_CMD(
	hioddr, 5, 0,	do_hio_ddr_params,
	"For internal use only",
	"- Do not use"
)