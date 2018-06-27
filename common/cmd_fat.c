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
#include <asm/io.h>
#include <part.h>
#include <fat.h>
#include <fs.h>

#if 1
int fat_check_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char* s;
	char* str;
	int i = 0;
	
	mdelay(1000);
	for (i=0; i<3; i++)
	{
		mdelay(500);
		run_command("usb start", 0);
		run_command("usb dev 0", 0);
		
		ret = run_command("fatload usb 0:1 0x10000000 /update.sh", 0);
		s = getenv("filesize");
		if(s != NULL)
		{
			printf("hio board==========update.sh exist\n");
			str = getenv("mmcroot");
			setenv("console", "tty1");
			if (strncmp("/dev/mmcblk3p2", str, 14) == 0)
				setenv("mmcroot", "/dev/mmcblk3p3 rootwait rw");
			
			if (strncmp("/dev/mmcblk2p2", str, 14) == 0)
				setenv("mmcroot", "/dev/mmcblk2p3 rootwait rw");

			break;
		}
		else
		{
			printf("hio board==========update.sh does not exist\n");
			setenv("console", "ttymxc1");
		}	
		
		run_command("usb stop", 0);	
	}	
}
#endif

#if 0
int fat_check_update(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	char* s;
	char* str;
	int i = 0;
    const char *env;
    unsigned int boot_os_flag;
	
	printf("run fat_check_update=?? \n");
	env = getenv("boot_os");
	if (env)
    {
		boot_os_flag = (unsigned)simple_strtol(env, NULL, 0);
		
    }
	else
	{
		run_command("setenv boot_os 1", 0);
		boot_os_flag = 1;
	}	
	
	printf("run1 fat_check_update= %d \n", boot_os_flag);
	
	run_command("usb start", 0);
	run_command("usb dev 0", 0);
		
	ret = run_command("fatload usb 0:1 0x10000000 /update.sh", 0);
	s = getenv("filesize");
	if(s != NULL)
	{
		printf("hio board==========update.sh exist\n");
		str = getenv("mmcroot");
		setenv("console", "tty1");
		if (strncmp("/dev/mmcblk3p2", str, 14) == 0)
			setenv("mmcroot", "/dev/mmcblk3p3 rootwait rw");
		
		if (strncmp("/dev/mmcblk2p2", str, 14) == 0)
			setenv("mmcroot", "/dev/mmcblk2p3 rootwait rw");

		run_command("setenv boot_os 1", 0);
		run_command("saveenv", 0);
	}
	else
	{
		printf("hio board==========update.sh does not exist\n");
		setenv("console", "ttymxc1");
		
		if (boot_os_flag < 5)
		{
			boot_os_flag = boot_os_flag + 1;
			
			if (boot_os_flag == 1)
				run_command("setenv boot_os 1", 0);
			else if (boot_os_flag == 2)
				run_command("setenv boot_os 2", 0);
			else if (boot_os_flag == 3)
				run_command("setenv boot_os 3", 0);
			else if (boot_os_flag == 4)
				run_command("setenv boot_os 4", 0);
			else if (boot_os_flag == 5)
				run_command("setenv boot_os 5", 0);
			
			run_command("saveenv", 0);
			run_command("reset", 0);
		}
		else
		{
			
		}	
	}	
		
	run_command("usb stop", 0);	
}
#endif

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

int do_fat_size(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return do_size(cmdtp, flag, argc, argv, FS_TYPE_FAT);
}

U_BOOT_CMD(
	fatsize,	4,	0,	do_fat_size,
	"determine a file's size",
	"<interface> <dev[:part]> <filename>\n"
	"    - Find file 'filename' from 'dev' on 'interface'\n"
	"      and determine its size."
);

int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return do_load(cmdtp, flag, argc, argv, FS_TYPE_FAT);
}


U_BOOT_CMD(
	fatload,	7,	0,	do_fat_fsload,
	"load binary file from a dos filesystem",
	"<interface> [<dev[:part]> [<addr> [<filename> [bytes [pos]]]]]\n"
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
	loff_t size;
	int ret;
	unsigned long addr;
	unsigned long count;
	block_dev_desc_t *dev_desc = NULL;
	disk_partition_t info;
	int dev = 0;
	int part = 1;
	void *buf;

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

	buf = map_sysmem(addr, count);
	ret = file_fat_write(argv[4], buf, 0, count, &size);
	unmap_sysmem(buf);
	if (ret < 0) {
		printf("\n** Unable to write \"%s\" from %s %d:%d **\n",
			argv[4], argv[1], dev, part);
		return 1;
	}

	printf("%llu bytes written\n", size);

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
