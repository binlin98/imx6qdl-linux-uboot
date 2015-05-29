/*
 * (C) Copyright 2011 - 2012 Samsung Electronics
 * EXT4 filesystem implementation in Uboot by
 * Uma Shankar <uma.shankar@samsung.com>
 * Manjunatha C Achar <a.manjunatha@samsung.com>
 *
 * Ext4fs support
 * made from existing cmd_ext2.c file of Uboot
 *
 * (C) Copyright 2004
 * esd gmbh <www.esd-electronics.com>
 * Reinhard Arlt <reinhard.arlt@esd-electronics.com>
 *
 * made from cmd_reiserfs by
 *
 * (C) Copyright 2003 - 2004
 * Sysgo Real-Time Solutions, AG <www.elinos.com>
 * Pavel Bartusek <pba@sysgo.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Changelog:
 *	0.1 - Newly created file for ext4fs support. Taken from cmd_ext2.c
 *	        file in uboot. Added ext4fs ls load and write support.
 */

#include <common.h>
#include <part.h>
#include <config.h>
#include <command.h>
#include <image.h>
#include <linux/ctype.h>
#include <asm/byteorder.h>
#include <ext4fs.h>
#include <linux/stat.h>
#include <malloc.h>
#include <fs.h>

//+++wwj begin 20130320@fix the bug of  mmcdev change in full_func board 
#include <linux/shenonmxc.h>
//+++wwj end

#if defined(CONFIG_CMD_USB) && defined(CONFIG_USB_STORAGE)
#include <usb.h>
#endif

int do_ext4_load(cmd_tbl_t *cmdtp, int flag, int argc,
						char *const argv[])
{
	return do_load(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

int do_ext4_ls(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	return do_ls(cmdtp, flag, argc, argv, FS_TYPE_EXT);
}

#ifdef CONFIG_LCD_CONF
//WWJ add for ext4 read file
#define msleep(a) udelay(a * 1000)
int ext4_read_txt_file(char* interface, int dev, int part, char* filename, struct tag_lcdconf* fsl_videomode)
{
	int ret, file_len, nxt, i, j, str_len, cmd_len;
	u32 data;
	char *cmd, *cmd_flag, *cmd_value;
	char* s;
	char * addr = 0x1D000000;
	//sprintf("%s %d %d", interface, dev, part);
#ifdef CONFIG_CHANGE_MMCDEV	//+++wwj 20130320@fix the bug of  mmcdev change in full_func board 

//+++wwj begin 20150306@get the boot dev num automatically
	int dev_num = 0;
	s = getenv("mmcdev");
	dev_num = simple_strtoul(s, NULL, 16);
	printf("the current mmcdev = %d\n", dev_num);
//+++wwj end
	printf("mmc dev %d\n", dev_num);
//+++wwj begin 20150306@get the boot dev num automatically
	if(2 == dev_num)
		ret = run_command("mmc dev 2", 0);
	else if(1 == dev_num)
		ret = run_command("mmc dev 1", 0);
	else	
		return 1;
//+++wwj end
//+++wwj begin 20130320@fix the bug of  mmcdev change in full_func board 
#else
	ret = run_command("mmc dev 2", 0);
#endif
//+++wwj end   20130320@fix the bug of  mmcdev change in full_func board
	if(ret)
		printf("start cmd  failed \n");
	msleep(100);
	//sprintf("%d:%d", dev, part);

#ifdef CONFIG_CHANGE_MMCDEV	//+++wwj  20130320@fix the bug of  mmcdev change in full_func board 

//+++wwj begin 20150306@get the boot dev num automatically
	if(2 == dev_num)
		ret = run_command("ext4load mmc 2:3 0x1D000000 /etc/lcd.conf", 0);
	else
		ret = run_command("ext4load mmc 1:3 0x1D000000 /etc/lcd.conf", 0);
//+++wwj end

//+++wwj begin 20130320@fix the bug of  mmcdev change in full_func board 
#else
	ret = run_command("ext4load mmc 2:3 0x1D000000 /etc/lcd.conf", 0);
#endif
//+++wwj end 20130320@fix the bug of  mmcdev change in full_func board
	
	if(ret){
		strncpy(fsl_videomode->name, "UNKNOWN", 19);
		printf("ext4load failed ret\n");
		return 1;
	}
	s = getenv("filesize");
	if (s != NULL)
	{
		file_len = simple_strtoul(s, NULL, 16);
	}
/*
	if (file_len > ENV_SIZE )
	{	
		env_len = ENV_SIZE;
		printf("sandy >>>> error >>>>>>>>>>:The update_env is too large\n");
		return -1;
	}
*/

	//start parsing the file
	for(i =0;  i < file_len; i = nxt+1)
	{
		
		//addr[nxt] cmd end;
		for (nxt = i; addr[nxt] != '\n'; ++nxt)
			;	
		
		for (j = i; addr[j] == ' '; ++j)	//skip ' '
			;
		if(addr[j] == '#')				//if the line start with '#' ,skip
			continue;

		// addr[i] cmd start;
		cmd = (char *)&addr[j];
		cmd_flag = cmd;				//cmd_flag start
		//cmd len
		str_len = nxt - j;
		
		cmd_value = strchr(cmd, ':');			

		if (cmd_value != NULL)
		{
			cmd_len = cmd_value - cmd;
			for (j = cmd_len; cmd_flag[j-1] == ' ' && j > 0; --j)		
					;

			cmd_flag[j] = '\0';		//cmd_flag end
			
			cmd_value++;		//skip '='

			for( ; (*cmd_value == ' ') && (*cmd_value != '\n'); cmd_value++)
				;						//cmd_value start skip ' '

			if (*cmd_value == '\n')
				cmd_value = NULL;
			else
			{
				for(j = str_len; cmd[j] == ' ' && cmd[j] == '\n'; --j)
					;
				cmd[j] = '\0';				//cmd_value end skip ' '
			}
		}
		else
		{
			cmd[str_len] = '\0';
			cmd_value = NULL;
		}

//		printf("%s:%s\n", cmd_flag, cmd_value);
	
		if(!strcmp(cmd_flag, "name")){
			strncpy(fsl_videomode->name, cmd_value, 19);
			fsl_videomode->name[19] = '\0';
		}else if(!strcmp(cmd_flag, "refresh")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->refresh = data;
			//printf("refresh = %d\n", fsl_videomode->refresh);
		}else if(!strcmp(cmd_flag, "xres")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->xres = data;
		}else if(!strcmp(cmd_flag, "yres")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->yres = data;
		}else if(!strcmp(cmd_flag, "pixclock")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->pixclock = data;
		}else if(!strcmp(cmd_flag, "left_margin")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->left_margin = data;
		}else if(!strcmp(cmd_flag, "right_margin")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->right_margin = data;
		}else if(!strcmp(cmd_flag, "upper_margin")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->upper_margin = data;
		}else if(!strcmp(cmd_flag, "lower_margin")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->lower_margin = data;
		}else if(!strcmp(cmd_flag, "hsync_len")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->hsync_len = data;
		}else if(!strcmp(cmd_flag, "vsync_len")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->vsync_len = data;
		}else if(!strcmp(cmd_flag, "sync")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->sync = data;
		}else if(!strcmp(cmd_flag, "vmode")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->vmode = data;
		}else if(!strcmp(cmd_flag, "flag")){
			data = simple_strtoul(cmd_value, NULL, 10);
			fsl_videomode->flag = data;
			//printf("flag = %d\n", fsl_videomode->flag);
		}else
			printf("the cmd_flag %s is illagle\n", cmd_flag);

		/*
		if (!strcmp(cmd_flag, "default_environment"))
		{
				set_default_env();
				saveenv();
				return 0;		
		}

		setenv(cmd_flag, cmd_value);
		*/
			
	}

	//*(addr ) = '\0';
	//ret = ext2load(interface, , addr, envfile);

}
#endif

#if defined(CONFIG_CMD_EXT4_WRITE)
int do_ext4_write(cmd_tbl_t *cmdtp, int flag, int argc,
				char *const argv[])
{
	const char *filename = "/";
	int dev, part;
	unsigned long ram_address;
	unsigned long file_size;
	disk_partition_t info;
	block_dev_desc_t *dev_desc;

	if (argc < 6)
		return cmd_usage(cmdtp);

	part = get_device_and_partition(argv[1], argv[2], &dev_desc, &info, 1);
	if (part < 0)
		return 1;

	dev = dev_desc->dev;

	/* get the filename */
	filename = argv[4];

	/* get the address in hexadecimal format (string to int) */
	ram_address = simple_strtoul(argv[3], NULL, 16);

	/* get the filesize in base 10 format */
	file_size = simple_strtoul(argv[5], NULL, 10);

	/* set the device as block device */
	ext4fs_set_blk_dev(dev_desc, &info);

	/* mount the filesystem */
	if (!ext4fs_mount(info.size)) {
		printf("Bad ext4 partition %s %d:%d\n", argv[1], dev, part);
		goto fail;
	}

	/* start write */
	if (ext4fs_write(filename, (unsigned char *)ram_address, file_size)) {
		printf("** Error ext4fs_write() **\n");
		goto fail;
	}
	ext4fs_close();

	return 0;

fail:
	ext4fs_close();

	return 1;
}

U_BOOT_CMD(ext4write, 6, 1, do_ext4_write,
	"create a file in the root directory",
	"<interface> <dev[:part]> <addr> <absolute filename path> [sizebytes]\n"
	"    - create a file in / directory");

#endif

U_BOOT_CMD(ext4ls, 4, 1, do_ext4_ls,
	   "list files in a directory (default /)",
	   "<interface> <dev[:part]> [directory]\n"
	   "    - list files from 'dev' on 'interface' in a 'directory'");

U_BOOT_CMD(ext4load, 6, 0, do_ext4_load,
	   "load binary file from a Ext4 filesystem",
	   "<interface> <dev[:part]> [addr] [filename] [bytes]\n"
	   "    - load binary file 'filename' from 'dev' on 'interface'\n"
	   "      to address 'addr' from ext4 filesystem");
