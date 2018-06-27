/*
 * (C) Copyright 2002
 * Richard Jones, rjones@nexus-tech.net
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <s_record.h>
#include <net.h>
#include <ata.h>
#include <part.h>
#include <fat.h>
#include <fs.h>

//#define CONFIG_NEW_USB_UPDATE 1
//+++WWJ begin
#include <linux/shenonmxc.h>
#ifdef CONFIG_EEPROM_RW
#define EEPROM_DDR_START 0x30
#define EEPROM_DDR_SIZE 0x20
#define DDR_FILE_HEAD 0x10
#endif
//+++WWJ end

int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return do_load(cmdtp, flag, argc, argv, FS_TYPE_FAT);
}


U_BOOT_CMD(
	fatload,	7,	0,	do_fat_fsload,
	"load binary file from a dos filesystem",
	"<interface> [<dev[:part]>]  <addr> <filename> [bytes [pos]]\n"
	"    - Load binary file 'filename' from 'dev' on 'interface'\n"
	"      to address 'addr' from dos filesystem.\n"
	"      'pos' gives the file position to start loading from.\n"
	"      If 'pos' is omitted, 0 is used. 'pos' requires 'bytes'.\n"
	"      'bytes' gives the size to load. If 'bytes' is 0 or omitted,\n"
	"      the load stops on end of file.\n"
	"      If either 'pos' or 'bytes' are not aligned to\n"
	"      ARCH_DMA_MINALIGN then a misaligned buffer warning will\n"
	"      be printed and performance will suffer for the load."
);

//+++ wwj
int get_usb_devices(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){
	printf("usb storage sum is %d", usb_devices_sum());
}
U_BOOT_CMD(
	usbdevsum, 1, 0, get_usb_devices,
	"get the sum of usb storage\n",
	"usbdevsum\n"
);
int fat_check_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char* s;
//+++wwj begin  20150324@add new way of usb-update detect
#ifdef CONFIG_NEW_USB_UPDATE
	ret = run_command("ext4load usb 0:2 0x10000000 /update.sh", 0);
#else
	ret = run_command("fatload usb 0:1 0x10000000 /update.sh", 0);
#endif
//+++wwj end 20150324@add new way of usb-update detect
	s = getenv("filesize");
	if(s != NULL){
		printf("WWJ==========update.sh exit\n");
//+++wwj begin  20150324@add new way of usb-update detect
#ifdef CONFIG_NEW_USB_UPDATE
		//set uimage and dtb file device
		setenv("loadfdt",  "fatload usb 0:1 ${fdt_addr} ${fdt_file}");
		//s = getenv("loadfdt");
		//printf("loadfdt = %s\n", s);
		setenv("loaduimage", "fatload usb 0:1 ${loadaddr} ${uimage}");
		//s = getenv("loaduimage");
		//printf("loaduimage = %s\n", s);
		//set  rootfs dev
		setenv("mmcroot", "/dev/sda2 rootwait rw");
#else
		setenv("mmcroot", "/dev/mmcblk0p3 rootwait rw");
#endif
//+++wwj end 20150324@add new way of usb-update detect
	}
	else
		printf("WWJ===========update.sh not exit");
#if 0
	ret = check_update(cmdtp, flag, argc, argv, FS_TYPE_FAT);
	if(ret){
		printf("find file %s ret = %d \n", argv[3], ret);
		setenv("mmcroot", "/dev/mmcblk0p3 rootwait rw");
	} else {
		printf("not find file %s \n", argv[3]);	
	}
#endif
}
U_BOOT_CMD(
	fatcheckupdate, 4, 0, fat_check_update,
	"check update file(u-boot.imx, uImage, rootfs) in usb device",
	"<interface> [<dev[:part]>]  <addr> <filename> [bytes [pos]]\n"
	"    - Load binary file 'filename' from 'dev' on 'interface'\n"
	"      to address 'addr' from dos filesystem.\n"
	"      'pos' gives the file position to start loading from.\n"
	"      If 'pos' is omitted, 0 is used. 'pos' requires 'bytes'.\n"
	"      'bytes' gives the size to load. If 'bytes' is 0 or omitted,\n"
	"      the load stops on end of file.\n"
	"      If either 'pos' or 'bytes' are not aligned to\n"
	"      ARCH_DMA_MINALIGN then a misaligned buffer warning will\n"
	"      be printed and performance will suffer for the load."
);
//+++ wwj

//+++WWJ begin
#ifdef CONFIG_EEPROM_RW
#define EEPROM_LED	IMX_GPIO_NR(4, 10)
int do_update_ddr_parms(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *s;
	int i = 0, retry = 3;
	unsigned int len;
	char cmd[128];
	char* buf = 0x10000000;
	char* read_buf = 0x10000050;
	bool light = true;
	run_command("fatload usb 0:1 0x10000000 /ddr.bin", 0);
	s = getenv("filesize");
	if(NULL != s){
		printf("WWJ===========ddr.bin exit\n");
		len = simple_strtoul(s, NULL, 10);
		if(len < 0x30){
			printf("%s:the file ddr.bin is broken\n", __func__);
			return -1;
		}

		*(buf + 13) = '\0';
		//printf("%s:ddr.bin header is %s\n", __func__, buf);

		if(strncmp("ShenCloudTech", buf, 13)){
			printf("%s:the file is wrong formate\n", __func__);
			return;
		}

		//buf += DDR_FILE_HEAD;
		while(retry--){
			buf = 0x10000000 + DDR_FILE_HEAD;
			read_buf = 0x10000050;
			for(i = EEPROM_DDR_START; i < (EEPROM_DDR_START + EEPROM_DDR_SIZE); i++){
				sprintf(cmd, "i2c mw 54 %x %x", i, *buf);
				buf++;
				//printf("%s:cmd = %s\n", __func__, cmd);
				run_command(cmd, 0);
				*read_buf = 0xff;
				read_buf++;
				udelay(10 * 1000);
			}

			run_command("i2c read 54 30 20 10000050", 0);
			buf = 0x10000000 + DDR_FILE_HEAD;
			read_buf = 0x10000050;
			for(i = 0; i < EEPROM_DDR_SIZE; i++){
				if(*buf != *read_buf)
					break;
				//printf("0x%x:0x%x\n", i, *read_buf);
				buf++;
				read_buf++;
			}
			for(i = 0; i < 6; i++){
				light = !light;
				gpio_direction_output(EEPROM_LED, (light ? 1: 0));
				udelay(500 * 1000);
			}
			gpio_direction_output(EEPROM_LED, 1);
			return 0;
		}	

			gpio_direction_output(EEPROM_LED, 0);
	}else
		printf("WWJ===========ddr.bin not exit\n");
}
U_BOOT_CMD(
	fat_update_ddr_parms, 1, 0, do_update_ddr_parms,
	"read ddr parms from usb and write to EEPROM dev by i2c cmd",
	"<interface> [<dev[:part]>] <filename>\n"
	"    - read binary file 'filename' from 'dev' on 'interface'\n"
	"    - and write to EEPROM dev by i2c cmd\n"
);
#endif
//+++WWJ end

static int do_fat_ls(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return do_ls(cmdtp, flag, argc, argv, FS_TYPE_FAT);
}

U_BOOT_CMD(
	fatls,	4,	1,	do_fat_ls,
	"list files in a directory (default /)",
	"<interface> [<dev[:part]>] [directory]\n"
	"    - list files from 'dev' on 'interface' in a 'directory'"
);

static int do_fat_fsinfo(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	int dev, part;
	block_dev_desc_t *dev_desc;
	disk_partition_t info;

	if (argc < 2) {
		printf("usage: fatinfo <interface> [<dev[:part]>]\n");
		return 0;
	}

	part = get_device_and_partition(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	dev = dev_desc->dev;
	if (fat_set_blk_dev(dev_desc, &info) != 0) {
		printf("\n** Unable to use %s %d:%d for fatinfo **\n",
			argv[1], dev, part);
		return 1;
	}
	return file_fat_detectfs();
}

U_BOOT_CMD(
	fatinfo,	3,	1,	do_fat_fsinfo,
	"print information about filesystem",
	"<interface> [<dev[:part]>]\n"
	"    - print information about filesystem from 'dev' on 'interface'"
);

#ifdef CONFIG_FAT_WRITE
static int do_fat_fswrite(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	long size;
	unsigned long addr;
	unsigned long count;
	block_dev_desc_t *dev_desc = NULL;
	disk_partition_t info;
	int dev = 0;
	int part = 1;

	if (argc < 5)
		return cmd_usage(cmdtp);

	part = get_device_and_partition(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	dev = dev_desc->dev;

	if (fat_set_blk_dev(dev_desc, &info) != 0) {
		printf("\n** Unable to use %s %d:%d for fatwrite **\n",
			argv[1], dev, part);
		return 1;
	}
	addr = simple_strtoul(argv[3], NULL, 16);
	count = simple_strtoul(argv[5], NULL, 16);

	size = file_fat_write(argv[4], (void *)addr, count);
	if (size == -1) {
		printf("\n** Unable to write \"%s\" from %s %d:%d **\n",
			argv[4], argv[1], dev, part);
		return 1;
	}

	printf("%ld bytes written\n", size);

	return 0;
}

U_BOOT_CMD(
	fatwrite,	6,	0,	do_fat_fswrite,
	"write file into a dos filesystem",
	"<interface> <dev[:part]> <addr> <filename> <bytes>\n"
	"    - write file 'filename' from the address 'addr' in RAM\n"
	"      to 'dev' on 'interface'"
);
#endif
