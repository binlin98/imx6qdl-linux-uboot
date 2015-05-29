#ifndef __SHENONMXC_H

#define __SHENONMXC_H

#include "SHEN_TYPE.h"


#if ((CUR_SHEN_TYPE) == (TYPE_VKI)) 
#define CONFIG_HDMI_DEC

//+++wwj begin
//elif ((CUR_SHEN_TYPE) == (TYPE_RII))
#elif ((CUR_SHEN_TYPE) == (TYPE_RRI_DDRV1))
//+++wwj end
#define CONFIG_RII
//#define CONFIG_HDMI_DEC //---wwj
#define CONFIG_EEPROM_RW
//+++wwj
//#define CONFIG_RRI_DDR_FP_201502
#define CONFIG_RRI_DDR_FP_201505

//+++wwj 20150324@add new way of usb-update detect
#define CONFIG_NEW_USB_UPDATE

#elif ((CUR_SHEN_TYPE) == (TYPE_SY_QM215))
//#define CONFIG_QM215 1
//#define CONFIG_NO_VERSION 1
#define CONFIG_SUPER_QUICK
#define CONFIG_F210_DDR_201501

#elif ((CUR_SHEN_TYPE) == (TYPE_SY_HK185))
//#define CONFIG_HK185 1
//#define CONFIG_NO_VERSION 1
#define CONFIG_SUPER_QUICK
#define CONFIG_F181_DDR_201502

#elif ((CUR_SHEN_TYPE) == (TYPE_GW))
//#define CONFIG_HDMI_DEC
#define CONFIG_SUPER_QUICK
#define CONFIG_WITHOUT_SPLASH

#elif ((CUR_SHEN_TYPE) == (TYPE_SY))

#define CONFIG_SUPER_QUICK
//#define CONFIG_HDMI_DEC
//+++wwj 
#define CONFIG_F210_DDR_201501

//+++wwj begin
#elif ((CUR_SHEN_TYPE) == (TYPE_SY_HKC215))
#define CONFIG_HKC215
#define CONFIG_SUPER_QUICK
//+++wwj end
//+++MQ begin [add F01 type]
#elif ((CUR_SHEN_TYPE) == (TYPE_SY_F01))
#define CONFIG_SUPER_QUICK
#define CONFIG_F01_DDR_201505
//+++MQ end

//+++wwj begin 20150306@add fully_func_board compile option
#elif ((CUR_SHEN_TYPE) == (TYPE_FULLY_FUNC_BOARD))
//+++wwj end

//+++wwj begin 20150514@add F180 ddr parameters
#elif ((CUR_SHEN_TYPE) == (TYPE_F180_DDR_20150415))
#define CONFIG_SUPER_QUICK
#define CONFIG_F180_DDR_0415
////+++wwj end

#endif


#endif
