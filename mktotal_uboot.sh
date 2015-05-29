#!/bin/bash
root_path=$(pwd)

if [ "$1" == "VKI" ]
then
	echo "Use $1"
	configed=1
#DDR NT5CB128M16FP-DI
elif [ "$1" == "RRI_DDRV1" ]
then
	echo "Use $1"
	configed=1
elif [ "$1" == "SY_QM215" ]
then
	echo "Use $1"
	configed=1
elif [ "$1" == "SY_HK185" ]
then
	echo "Use $1"
	configed=1
elif [ "$1" == "GW" ]
then
	echo "Use $1"
	configed=1
elif [ "$1" == "SY" ]
then
	echo "Use $1"
	configed=1
elif [ "$1" == "SY_HKC215" ]
then
	echo "Use $1"
	configed=1
#+++MQ begin [add F01 type]
elif [ "$1" == "SY_F01" ]
then
	echo "Use $1"
	configed=1
#+++MQ end

#+++wwj begin 20150306@add fully_func_board compile option
elif [ "$1" == "FULLY_FUNC_BOARD" ]
then
	echo "Use $1"
	configed=1
#+++wwj end

#+++wwj begin 20150514@add F180 ddr parameters
elif [ "$1" == "F180_DDR_20150415" ]
then
	echo "Use $1"
	configed=1
#+++wwj end

else
	echo "----------------------------------------------------------------"
	echo "Please choose below type:"
    	echo "VKI"
	echo "RRI_DDRV1"
	echo "SY_QM215"
	echo "SY_HK185"
	echo "GW"
	echo "SY"
	echo "SY_HKC215"
#+++MQ begin [add F01 type]
	echo "SY_F01"
#+++MQ end
#+++wwj begin 20150306@add fully_func_board compile option
	echo "FULLY_FUNC_BOARD"
#+++wwj end
#+++wwj begin 20150514@add F180 ddr parameters
	echo "F180_DDR_20150415"
#+++wwj end
	echo "Example:"
	echo "        ./mktotal_uboot.sh VKI"
    configed=0
fi

TYPE_FILE=./include/linux/SHEN_TYPE.h
if [ "$configed" == "1" ] 
then
	echo "----------------------------------------------------------------"
	perl -pi -e 's/\#define CUR_SHEN_TYPE .*/\#define CUR_SHEN_TYPE TYPE_'"$1"'/g' $TYPE_FILE
	cat $TYPE_FILE | grep "CUR_SHEN_TYPE"
	echo "----------------------------------------------------------------"
	make ARCH=arm CROSS_COMPILE=arm-eabi- distclean
	
	if [ "$1" == "VKI" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_no_logo_config
	elif [ "$1" == "RRI_DDRV1" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_no_logo_config
	elif [ "$1" == "SY_QM215" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
	elif [ "$1" == "SY_HK185" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
	elif [ "$1" == "GW" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
	elif [ "$1" == "SY" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
	elif [ "$1" == "SY_HKC215" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
#+++MQ begin [add F01 type]
	elif [ "$1" == "SY_F01" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
#+++MQ end
#+++wwj begin 20150514@add F180 ddr parameters
	elif [ "$1" == "F180_DDR_20150415" ]
        then
                make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_config
#+++wwj end

#+++wwj begin 20150306@add fully_func_board compile option
	elif [ "$1" == "FULLY_FUNC_BOARD" ]
	then
		make ARCH=arm CROSS_COMPILE=arm-eabi- mx6dlsabresd_no_logo_config
#+++wwj end

	fi
	
	echo "Start compile $1 ......"
	if [ -f ./u-boot.imx ]
	then
		echo "----------------------------------------------------------------"
		echo "Remove old u-boot.imx"
		rm -rf ./u-boot.imx
		echo [Done]
		echo "----------------------------------------------------------------"
	fi
	make -j8
	echo "----------------------------------------------------------------"

	

	echo "Create new u-boot_$1.imx"
	cp u-boot.imx   u-boot_$1_$(date +"%Y%m%d%H").imx
	echo [Done]
	echo "----------------------------------------------------------------"
	echo "New image files md5sum:"
	md5sum  u-boot_$1_$(date +"%Y%m%d%H").imx > ./u-boot_$1_$(date +"%Y%m%d%H").imx_md5
	echo [Done]
fi
