#include "lock_config.h"           
#include "flash.h"
#include "audio.h"
#include "password.h"
#include <string.h>
#include "card.h"
#include "lock_record.h" 
#include "key.h"
#include "eport_drv.h"
#include "bat.h"

//产品型号;硬件版本;软件版本;发行日期;类型beta或release
//通过版本可以判断产品版本，以及产品类型


#ifdef NO_FACE_MODE   
//const char ver[40]="T6W_H2.0_S1.0_230224_release";                       //硬件版本2.0代表新版本4202E芯片

const char ver[40]="T6MW_H2.0_S1.0_230224_release";        //猫眼不带人脸
//const char ver[40]="T6VW_H2.0_S1.0_230224_release";        //可视不带人脸

#else
const char ver[40]="T6FW_H2.0_S1.0_230224_release";                      //硬件版本2.0代表新版本4202E芯片
//const char ver[40]="T6FV_H2.0_S1.0_230224_release";        //可视带人脸
//const char ver[40]="T6MV_H2.0_S1.0_230224_release";        //猫眼带人脸
#endif

SysConfig_t SysConfig={0},SysConfigBk={0};
PeripLock_t PeripLock={0},PeripLockBk={0};
TamperAlarm_t TamperAlarm={0};
LowBatAlarm_t LowBatAlarm={0};
NetTime_t NetTime={0};
IapInfo_t IapInfo={0};
CoerceFp_t CoerceFp={0};
// DG   22    17        M5       000016    085105
//产地  年  第几周  型号（2-4）  第几台     SN码
char SN[SN_MAX_LEN]={0};  
char SnNum[7]={'7','8','6','9','3','0',0}; 
uint8_t MutiIdyItem=0;
uint8_t SnInvalid=0;

/******************************************************************************/
/*
2023/02/24
1.换新版MCU，由于新版与旧版IO，触摸按键有两个口硬件对调，所以不兼容，硬件版本升为3.0
2.修复管理菜单退出指纹不失败得BUG


*/
/******************************************************************************/
/*
//更新锁体SN号
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UpdateLockSn(void)
{
		uint8_t i=0,snStart=0;
	
		SnInvalid=0;
		//读序列号数据
		IntFlashRead((uint8_t*)SN,SN_ADDR,SN_MAX_LEN);  
		//找出有效序列号位置
		for(i=31;i>0;i--)
		{
				if(SN[i]!=0)
				{
						snStart=i-5;
						break;
				}
		}
		//获取6字节序列号
		for(i=snStart;i<snStart+6;i++)
		{
				SnNum[i-snStart]=SN[i];
				if(SN[i]<'0' || SN[i]>'9')
				{
						SnInvalid=1;
				}
		}
		//验证激活码是否有效
		printf("SN: %s",SnNum);
}

/******************************************************************************/
/*
//读系统设置信息
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockReadSysConfig(void)
{
		uint8_t i=0;  
		//读SN
		UpdateLockSn();
		//读设备控制字
		IntFlashRead(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t));           
		if(SysConfig.Bits.Flag!=0xA)        //1010  
		{
				SysConfig.Bits.Activate=0;
				SysConfig.Bits.Language=0;
				SysConfig.Bits.CardLock=0;
				SysConfig.Bits.ChildLock=0;
				SysConfig.Bits.FaceLock=0;
				SysConfig.Bits.MutiIdy=0;
				SysConfig.Bits.FpLock=0;
				SysConfig.Bits.SupPowSave=0;
				SysConfig.Bits.TampAct=1;
				SysConfig.Bits.KeyPADLock=0;
				SysConfig.Bits.KeepOpen=0;
				SysConfig.Bits.Volume=1;              //0-0,1-5,2-10
				SysConfig.Bits.OTCode=0;
				SysConfig.Bits.PrivMode=0;
				SysConfig.Bits.PirOn=0;               //默认打开PIR
				SysConfig.Bits.DiSel=0;
				SysConfig.Bits.TimeZone=8;
				SysConfig.Bits.Rsv=0;                 //保留值0
				SysConfig.Bits.Flag=0xA;
				IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		}	
		SysConfigBk=SysConfig;
		SysConfig.Bits.Activate=1;
		if(SysConfig.Bits.Activate==0)SysConfig.Bits.PirOn=0;
		printf("timezone:%d\n",SysConfig.Bits.TimeZone);
		//设置音量
		Audio_SetVolume(SysConfig.Bits.Volume);
		//读外设锁相关，此处没有对重新上电情况做处理，放在主循环中每秒检测中更新去了。
		IntFlashRead(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		if(PeripLock.Perip.Flag!=PERIPHLOCK_FLAG)  
		{
				for(i=0;i<4;i++)
				{
						PeripLock.Perip.FailTime[i]=0;
				}
				for(i=0;i<4;i++)
				{
						PeripLock.Perip.FailStart[i].day=0;
						PeripLock.Perip.FailStart[i].hour=0;
						PeripLock.Perip.FailStart[i].minute=0;
						PeripLock.Perip.FailStart[i].second=0;
						PeripLock.Perip.LockStart[i].day=0;
						PeripLock.Perip.LockStart[i].hour=0;
						PeripLock.Perip.LockStart[i].minute=0;
						PeripLock.Perip.LockStart[i].second=0;
				}
				PeripLock.Perip.Flag=PERIPHLOCK_FLAG;
				IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		}
		PeripLockBk=PeripLock;
		//读电池电压
		IntFlashRead(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t)); 
		if(LowBatAlarm.Bat.Flag!=0xAA)
		{
				LowBatAlarm.Bat.Flag=0xAA;
				LowBatAlarm.Bat.Value=0;
				LowBatAlarm.Bat.Day=0;
				LowBatAlarm.Bat.Hour=0;
				LowBatAlarm.Bat.Minute=0;
				LowBatAlarm.Bat.Second=0; 
				IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));   //初始化
		}
		if(LowBatAlarm.Bat.Value>0)     //有保存电压
		{
				mytm_t tm={0};
				tm.day=LowBatAlarm.Bat.Day;
				tm.hour=LowBatAlarm.Bat.Hour;
				tm.minute=LowBatAlarm.Bat.Minute;
				tm.second=LowBatAlarm.Bat.Second;
				if(GetPeripLockTimeElapse(&tm)<=0)    //重新上电全部清0
				{
						LowBatAlarm.Bat.Value=0;
						LowBatAlarm.Bat.Day=0;
						LowBatAlarm.Bat.Hour=0;
						LowBatAlarm.Bat.Minute=0;
						LowBatAlarm.Bat.Second=0; 
				}
				BatPerCent=GetVoltagePercent(LowBatAlarm.Bat.Value);   //唤醒或上电就用以前记录的电池电压计算,不然在传电量数据到APP时会出现0
		}	
		//读网络时间
		IntFlashRead(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		if(NetTime.NT.Flag!=0xAA)          //初始化网络时间结构
		{
				NetTime.NT.Stat=0;
				NetTime.NT.Day=0;
				NetTime.NT.Hour=0;
				NetTime.NT.Minute=0;
				NetTime.NT.Second=0;
				NetTime.NT.Flag=0xAA;
				IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		}
		else if(NetTime.NT.Stat==1)       //已经获取过时间，判断是否有重新上电
		{
				mytm_t tm={0};
				tm.day=NetTime.NT.Day;
				tm.hour=NetTime.NT.Hour;
				tm.minute=NetTime.NT.Minute;
				tm.second=NetTime.NT.Second;
				if(GetPeripLockTimeElapse(&tm)<=0)    //重新上电全部清0
				{
						NetTime.NT.Stat=0;
						NetTime.NT.Day=0;
						NetTime.NT.Hour=0;
						NetTime.NT.Minute=0;
						NetTime.NT.Second=0;
						NetTime.NT.Flag=0xAA;
						IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t));
				}
		}
//		for(i=0;i<8;i++)printf("data:0x%02x ",NetTime.NTBuf[i]);
//		printf("nt: day:%02d,hour:%02d,min:%02d,sec:%02d\n",NetTime.NT.Day,NetTime.NT.Hour,NetTime.NT.Minute,NetTime.NT.Second);
		//读胁迫指纹标识
		IntFlashRead(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t)); 
		if(CoerceFp.CFP.Flag1!=0xAA && CoerceFp.CFP.Flag2!=0x55 && CoerceFp.CFP.Flag3!=0xAA)
		{
				CoerceFp.CFP.Flag1=0xAA;CoerceFp.CFP.Flag2=0x55;CoerceFp.CFP.Flag3=0xAA;
				for(i=0;i<13;i++)
				{
						CoerceFp.CFP.CoerceFp[i]=0;
				}
				IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t)); 
		}
		//读外部FLASH用户操作记录数据包头
		ExFlashRead(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
		if(UserOperaHead.OpHead.Flag!=0xAA55 || UserOperaHead.OpHead.WriteIndex>=USER_RECORD_MAX_NUM)
		{
				printf("user record init\n");
				UserOperaHead.OpHead.Flag=0xAA55;
				UserOperaHead.OpHead.WriteIndex=0;
				UserOperaHead.OpHead.RecordSum=0;
				ExFlashWrite(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
		}
		printf("user record sum: 0x%02x\n",UserOperaHead.OpHead.WriteIndex);
}

/******************************************************************************/
/*
//写系统设置信息
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockWriteSysConfig(void)
{
		if(SysConfigBk.B32!=SysConfig.B32)
		{
				if(SysConfigBk.Bits.Volume!=SysConfig.Bits.Volume)
				{
						Audio_SetVolume(SysConfig.Bits.Volume);
				}
				LockModifySysConfigOpera(SysConfigBk.B32,SysConfig.B32);
				SysConfigBk.B32=SysConfig.B32;
				IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		}
}
/******************************************************************************/
/*
//写外设锁状态信息
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockWritePerpLcokInfo(void)
{
		uint8_t i=0;
		for(i=0;i<sizeof(PeripLock_t);i++)
		{
				if(PeripLockBk.PripBuf[i]!=PeripLock.PripBuf[i])
				{
						PeripLockBk=PeripLock;
						IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
						return;
				}
		}
}
/***************************************************************************/
/*
-----------------简易产生激活码-------------------
. 先调换SN各个字符的位置，1~3互换，2~4互换，5~6互换
. 产生一个随机整数（1~9）
. 将各个字符的值与随机数相加 
. 对10取余
. 去掉第一个数字 ，用随机数取代
. 用新的数字产生激活码

--------------------------举例-------------------------
SN： 786930
.调换位置 -> 697803
.产生随机数-> 2
.相加 -> [6+2,9+2,7+2，8+2，0+2，3+2]
.取余 -> [8，1，9，0，2，5 ]
.去掉第一个数字 ，用随机数取代 ->[2,1,9,0,2,5]
.产生激活码 219025
*/


/****************************************************************************/

/******************************************************************************/
/*
//激活码校验
input:   ActCode1--激活码6字节字符
output   none
return   1-OK 0-NOK 
*/
/******************************************************************************/
uint8_t CheckSnActiveCode(char *ActCode1) 
{
		uint8_t i=0;
		char newSn[7]={0};
		uint8_t arad=ActCode1[0];        //第一个为随机数
		//交换顺序
		newSn[0]=SnNum[2]-'0';
		newSn[1]=SnNum[3]-'0';
		newSn[2]=SnNum[0]-'0';
		newSn[3]=SnNum[1]-'0';
		newSn[4]=SnNum[5]-'0';
		newSn[5]=SnNum[4]-'0';
		
		for(i=0;i<6;i++)
		{
				newSn[i]+=(arad-'0');         //加随机数
				newSn[i]%=10;
				newSn[i]+='0';
		}
		newSn[0]=arad;
		
		printf("actcode: %s",newSn);
		
		
		for(i=0;i<6;i++)
		{
				if(newSn[i]!=ActCode1[i])
				{
						break;
				}
		}
		if(i<6)return 0;
		
		return 1;
}
/******************************************************************************/
/*
//未激活状态提示
input:   none
output   none
return   none 
*/
/******************************************************************************/
void SysActCheck(void)
{
		static uint8_t AlarmAct=0;
	
		if(SysConfig.Bits.Activate==0 && AlarmAct==0 && GetTimerElapse(0)>1000)
		{
				AlarmAct=1;
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"请激活产品"),UNBREAK);
		}
}

/******************************************************************************/
/*
//恢复出厂设置
input:   none
output   none
return   none 
*/
/******************************************************************************/
void FactoryReset(void)
{
		uint16_t i=0;

		//sysconfig设置
//		SysConfig.Bits.Activate=0; 保留系统激活信息，其他还原到默认设置
		SysConfig.Bits.Language=0;
		SysConfig.Bits.CardLock=0;
		SysConfig.Bits.ChildLock=0;
		SysConfig.Bits.FaceLock=0;
		SysConfig.Bits.MutiIdy=0;
		SysConfig.Bits.FpLock=0;
		SysConfig.Bits.SupPowSave=0;
		SysConfig.Bits.TampAct=1;
		SysConfig.Bits.KeyPADLock=0;
		SysConfig.Bits.KeepOpen=0;
		SysConfig.Bits.Volume=1;              //0-0,1-5,2-10
		SysConfig.Bits.OTCode=0;
		SysConfig.Bits.PrivMode=0;
#ifdef NO_FACE_MODE
		SysConfig.Bits.PirOn=0;               //默认关闭PIR
#else
		SysConfig.Bits.PirOn=1;               //默认打开PIR
		if(SysConfig.Bits.Activate==0)SysConfig.Bits.PirOn=0;
#endif
		SysConfig.Bits.DiSel=0;
		SysConfig.Bits.TimeZone=8;            //默认东八区
		SysConfig.Bits.Rsv=0;                 //保留值0
		SysConfig.Bits.Flag=0xA;
		IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		//清外设锁信息
		for(i=0;i<4;i++)
		{
				PeripLock.Perip.FailTime[i]=0;
		}
		for(i=0;i<4;i++)
		{
				PeripLock.Perip.FailStart[i].day=0;
				PeripLock.Perip.FailStart[i].hour=0;
				PeripLock.Perip.FailStart[i].minute=0;
				PeripLock.Perip.FailStart[i].second=0;
				PeripLock.Perip.LockStart[i].day=0;
				PeripLock.Perip.LockStart[i].hour=0;
				PeripLock.Perip.LockStart[i].minute=0;
				PeripLock.Perip.LockStart[i].second=0;
		}
		PeripLock.Perip.Flag=PERIPHLOCK_FLAG;
		IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		//清一次性密码
		memset((uint8_t*)&OneTimeCode,0,sizeof(OneTimeCode_t));   
		IntFlashWrite((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));	
		//清防拆信息
		TamperAlarm.Tamp.Flag=0xAA;
		TamperAlarm.Tamp.Time=10;                                    //默认10S报警 
		TamperAlarm.Tamp.Stat=EPORT_ReadGpioData(KEY_TAMPER_PIN);    //读出状态并保存
		TamperAlarm.Tamp.Day=0;
		TamperAlarm.Tamp.Hour=0;
		TamperAlarm.Tamp.Minute=0;
		TamperAlarm.Tamp.Second=0; 
		IntFlashWrite(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t));
		//清电池电压检测信息
		LowBatAlarm.Bat.Flag=0xAA;
		LowBatAlarm.Bat.Value=0;
		LowBatAlarm.Bat.Day=0;
		LowBatAlarm.Bat.Hour=0;
		LowBatAlarm.Bat.Minute=0;
		LowBatAlarm.Bat.Second=0; 
		IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));   //初始化
		//清网络对时相关
		NetTime.NT.Stat=0;
		NetTime.NT.Day=0;
		NetTime.NT.Hour=0;
		NetTime.NT.Minute=0;
		NetTime.NT.Second=0;
		NetTime.NT.Flag=0xAA;
		IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		//清胁迫
		CoerceFp.CFP.Flag1=0xAA;CoerceFp.CFP.Flag2=0x55;CoerceFp.CFP.Flag3=0xAA;
		for(i=0;i<13;i++)
		{
				CoerceFp.CFP.CoerceFp[i]=0;
		}
		IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t));
		//清用户密码
		memset(Pwd.PwdGenStr[0].PwdBuf,0,PWD_MAX*sizeof(PwdGenStr_t));    //清空密码
		IntFlashWrite(Pwd.PwdGenStr[0].PwdBuf,PWD_DATA_ADDR,PWD_MAX*sizeof(PwdGenStr_t));
		//清时效密码
		memset(Pwd.PwdTimStr[0].PwdTimBuf,0,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		IntFlashWrite(Pwd.PwdTimStr[0].PwdTimBuf,PWD_TIM_DATA_ADDR,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		//清卡
		memset(CardUser,0,sizeof(CardUser));
		IntFlashWrite((uint8_t*)&CardUser,CARD_DATA_ADDR,sizeof(CardUser));
}




















