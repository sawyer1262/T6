#include "password.h"
#include "flash.h"  
#include "lock_config.h"
#include <string.h>
#include "lock_record.h"
#include "rtc.h"
#include "time_cal.h"


PwdStr_t Pwd={0};                            //����
OneTimeCode_t OneTimeCode={0};               //һ��������


/******************************************************************************/
/*
//��ʼ����������
input:   none
output   none
return   none
*/
/******************************************************************************/
void PassWordInit(void)
{
		uint8_t i=0;
		PwdGenStr_t PwdTmp={0};
		PwdExtStr_t PwdTimTmp={0};
		//��һ��������
		if(SysConfig.Bits.OTCode==1)
		{
				IntFlashRead((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));
				if(!CheckOneTimeCodeValid(&OneTimeCode))   //������Ч
				{
						SysConfig.Bits.OTCode=0;
				}
		}
		//�����룬���ж�������Ч��
		Pwd.PwdInfo.Item.PwdSum=0;
		for(i=0;i<PWD_MAX;i++)
		{
				IntFlashRead(&PwdTmp.PwdBuf[0],PWD_DATA_ADDR+i*(PWD_LEN_MAX+3),PWD_LEN_MAX+3);
				if(PwdTmp.Pwd.Type<NORMAL || PwdTmp.Pwd.Type>VISITOR)          //������������Ƿ���Ч
				{
						PwdTmp.Pwd.Type=INVALID;
				}
				else if(CheckPwdVail(PwdTmp.Pwd.Buf,PWD_LEN_MAX)<PWD_LEN_MIN)  //��������Ƿ���Ч
				{
						PwdTmp.Pwd.Type=INVALID;
				}
				else
				{
						Pwd.PwdInfo.Item.PwdSum++;
				}
				Pwd.PwdGenStr[i]=PwdTmp;
		}
		if(Pwd.PwdInfo.Item.PwdSum==0)          //û�����룬����Ĭ�Ϲ���Ա����
		{
				Pwd.PwdGenStr[0].Pwd.Type=ADMIN;
				Pwd.PwdGenStr[0].Pwd.Idx=0;
				for(i=0;i<6;i++)
				{
						Pwd.PwdGenStr[0].Pwd.Buf[i]=i+0x31;      //Ĭ�Ϲ���Ա����'1'-'6'
				}
				for(i=i;i<PWD_LEN_MAX+1;i++)
				{
						Pwd.PwdGenStr[0].Pwd.Buf[i]=0;
				}
				IntFlashWrite(&Pwd.PwdGenStr[0].PwdBuf[0],PWD_DATA_ADDR,PWD_LEN_MAX+3);
		}
		//printf("admin code: %s \n",Pwd.PwdGenStr[0].Pwd.Buf);
		for(i=0;i<PWD_TIM_MAX;i++)
		{
				IntFlashRead(&PwdTimTmp.PwdTimBuf[0],PWD_TIM_DATA_ADDR+i*PWD_TIM_LEN_MAX,PWD_TIM_LEN_MAX);
				
				if(PwdTimTmp.PwdTimBuf[0]!=INACTIVE)     //��־��Ч
				{
						if(!CheckPwdTimVail(&PwdTimTmp))    //�ж����ݣ� 
						{
								memset(PwdTimTmp.PwdTimBuf,0,sizeof(PwdExtStr_t)); //������Ч����0
								IntFlashWrite(&PwdTimTmp.PwdTimBuf[0],PWD_TIM_DATA_ADDR,PWD_TIM_LEN_MAX);
						}
						else 
						{
								Pwd.PwdInfo.Item.PwdTimSum++;
						}
				}
				Pwd.PwdTimStr[i]=PwdTimTmp;
		}
}

/******************************************************************************/
/*
//��ȡ���õ�����SLOT
input:   none
output   none
return   SLOT--���õ�SLOT
*/
/******************************************************************************/
uint8_t GetPassWordFreeSlot(void)
{
		uint8_t i=0;
	
		for(i=0;i<PWD_MAX;i++)
		{
				if(Pwd.PwdGenStr[i].Pwd.Type==INVALID)
				{
						return i;
				}
		}
		return i;
}
/******************************************************************************/
/*
//��ȡ���е�������Чʱ��SLOT
input:   none
output   none
return   ���е���Чʱ��SLOT
*/
/******************************************************************************/
uint8_t GetPassWordTimFreeSlot(void)
{
		uint8_t i=0;
	
		for(i=0;i<PWD_TIM_MAX;i++)
		{
				if(Pwd.PwdTimStr[i].PwdTimBuf[0]==INACTIVE)
				{
						return i;
				}
		}
		return i;
}
/******************************************************************************/
/*
//�������
input:   type -- �û����ͣ�ADMIN-����Ա ����-�����û�
         code -- ��ӵ�����
         ext  -- �������ݣ���Чʱ�����Ϣ
output   slot---��ӳɹ���������
return   
				PWD_ADD_SUCESS            0x00         //��ӳɹ�
				PWD_ADD_TYP_WRONG         0x01         //���ʹ���
				PWD_ADD_MEM_FULL          0x02         //���������
				PWD_ADD_TIME_MEM_FULL     0x03         //��ʱЧ���������
				PWD_ADD_CODE_EXIST        0x04         //�����Ѵ���   
*/
/******************************************************************************/
uint8_t AddPassWord(uint8_t type,uint8_t *code,PwdExtStr_t *ext,uint8_t *slot)
{
		uint8_t i=0,fristfreeslot=0xFF;
		uint8_t fristtimfreeslot=0;
		uint8_t codevalid[PWD_LEN_MAX]={0};
		
		if(slot!=0)*slot=0xFF;
	
		if(type>VISITOR)return PWD_ADD_TYP_WRONG;                                     //���Ͳ���
		
		while(*code!=0)
		{
				codevalid[i++]=*code++;                  
		}
		if(i<PWD_LEN_MIN || i>PWD_LEN_MAX-1)return PWD_ADD_TYP_WRONG;           //<6 >12������6-12λ
		
		if(type==ADMIN)
		{
				Pwd.PwdGenStr[0].Pwd.Type=ADMIN;
				Pwd.PwdGenStr[0].Pwd.Idx=0;
				for(i=0;i<PWD_LEN_MAX;i++)
				{
						Pwd.PwdGenStr[0].Pwd.Buf[i]=codevalid[i];      
				}
				IntFlashWrite(Pwd.PwdGenStr[0].PwdBuf,PWD_DATA_ADDR,PWD_LEN_MAX+3);
				
				LockAddUser(PERIP_CODE_INDEX,0);	
				if(slot!=0)*slot=0;
				return PWD_ADD_SUCESS;
		}
		
		if(Pwd.PwdInfo.Item.PwdSum>=PWD_MAX)return PWD_ADD_MEM_FULL;                               //��������
	
		if(type==VISITOR && Pwd.PwdInfo.Item.PwdTimSum>=PWD_TIM_MAX)return PWD_ADD_TIME_MEM_FULL;  //ʱЧ��������
		
		//���������Ƿ��Ѵ���
		for(i=0;i<PWD_MAX;i++)
		{
				if(Pwd.PwdGenStr[i].Pwd.Type==INVALID)
				{
						if(fristfreeslot==0xFF)fristfreeslot=i;      //�����һ�����е�SLOT�����ڴ������ӵ�����
						continue;
				}
				if(strstr((char*)Pwd.PwdGenStr[i].Pwd.Buf,(char*)codevalid)!=0)   //�ж�����ӵ����룬�Ƿ��Ѿ�����
				{
						if(slot!=0)*slot=i;                 //����Ѵ��ڵ�������
						return PWD_ADD_CODE_EXIST;          //�����Ѵ���
				}
		}
		if(fristfreeslot==0xFF)return PWD_ADD_MEM_FULL;     //�޿���SLOT�������������
		//�������
		Pwd.PwdGenStr[fristfreeslot].Pwd.Type=type;         //��������
		for(i=0;i<PWD_LEN_MAX;i++)
		{
				Pwd.PwdGenStr[fristfreeslot].Pwd.Buf[i]=codevalid[i];
		}
		//�������ʱЧ����д��FLASH
		if(type==VISITOR && ext!=NULL)                          //�ÿͻ���ʱ����      
		{
				fristtimfreeslot=GetPassWordTimFreeSlot();
				if(fristtimfreeslot==PWD_TIM_MAX)return PWD_ADD_TIME_MEM_FULL;

				Pwd.PwdGenStr[fristfreeslot].Pwd.Idx=fristtimfreeslot;    //ָ������ʱЧ
				memcpy(Pwd.PwdTimStr[fristtimfreeslot].PwdTimBuf,ext->PwdTimBuf,sizeof(PwdExtStr_t));
				IntFlashWrite(Pwd.PwdTimStr[fristtimfreeslot].PwdTimBuf,PWD_TIM_DATA_ADDR+fristtimfreeslot*sizeof(PwdExtStr_t),sizeof(PwdExtStr_t));   //����ʱЧ
				Pwd.PwdInfo.Item.PwdTimSum++;
		}
		//д���뵽FLASH
		IntFlashWrite(Pwd.PwdGenStr[fristfreeslot].PwdBuf,PWD_DATA_ADDR+fristfreeslot*sizeof(PwdGenStr_t),sizeof(PwdGenStr_t));                       //��������
		LockAddUser(PERIP_CODE_INDEX,fristfreeslot);	
		Pwd.PwdInfo.Item.PwdSum++;
		if(slot!=0)*slot=fristfreeslot;
		return PWD_ADD_SUCESS;                                 //��ӳɹ�
}

/******************************************************************************/
/*
//ɾ������
input:   slot  -- ����ID
output   none
return   
	       PWD_DEL_SUCCESS           0x00         //ɾ������ɹ�
         PWD_DEL_CODE_NOT_EXIST    0x01         //���벻����
*/
/******************************************************************************/
uint8_t DelPassWord(uint8_t slot)
{
		uint8_t delIdx=slot;
	
		if(delIdx>=100 && delIdx!=0xFF)return PWD_DEL_CODE_NOT_EXIST;    //�����ɾ�����
		
		if(delIdx==0xFF)
		{
				//�����룬�������Ա
				memset(Pwd.PwdGenStr[1].PwdBuf,0,(PWD_MAX-1)*sizeof(PwdGenStr_t));    //�������,�������Ա
				IntFlashWrite(Pwd.PwdGenStr[0].PwdBuf,PWD_DATA_ADDR,PWD_MAX*sizeof(PwdGenStr_t));
				//��ʱЧ
				memset(Pwd.PwdTimStr[0].PwdTimBuf,0,PWD_TIM_MAX*sizeof(PwdExtStr_t));
				IntFlashWrite(Pwd.PwdTimStr[0].PwdTimBuf,PWD_TIM_DATA_ADDR,PWD_TIM_MAX*sizeof(PwdExtStr_t));
				//д��¼
				LockDelUser(PERIP_CODE_INDEX,delIdx);				
				//���³�ʼ������
				PassWordInit();                                  //���¶�����
				return PWD_DEL_SUCCESS;
		}
		if(Pwd.PwdGenStr[delIdx].Pwd.Type==INVALID)return PWD_DEL_CODE_NOT_EXIST;     //���뱾����Ч
		//���ʱЧ��
		if(Pwd.PwdGenStr[delIdx].Pwd.Type==VISITOR && Pwd.PwdTimStr[Pwd.PwdGenStr[delIdx].Pwd.Idx].PwdTimBuf[0]!=INACTIVE)     //�ÿ���ʱ��
		{
				memset(Pwd.PwdTimStr[Pwd.PwdGenStr[delIdx].Pwd.Idx].PwdTimBuf,0,sizeof(PwdExtStr_t));
				IntFlashWrite(Pwd.PwdTimStr[Pwd.PwdGenStr[delIdx].Pwd.Idx].PwdTimBuf,PWD_TIM_DATA_ADDR+Pwd.PwdGenStr[delIdx].Pwd.Idx*sizeof(PwdExtStr_t),sizeof(PwdExtStr_t));
				printf("delet ok\n");
				Pwd.PwdInfo.Item.PwdTimSum--;
		}
		//�������
		memset(Pwd.PwdGenStr[delIdx].PwdBuf,0,sizeof(PwdGenStr_t)); 
		Pwd.PwdGenStr[delIdx].Pwd.Idx=0;
		Pwd.PwdGenStr[delIdx].Pwd.Type=INVALID;
		IntFlashWrite(Pwd.PwdGenStr[delIdx].PwdBuf,PWD_DATA_ADDR+delIdx*sizeof(PwdGenStr_t),sizeof(PwdGenStr_t)); 
		//д������¼
		LockDelUser(PERIP_CODE_INDEX,delIdx);
		//����������Ϣ
		Pwd.PwdInfo.Item.PwdSum--;
		return PWD_DEL_SUCCESS;
}
/******************************************************************************/
/*
//��֤����
input:   code  -- ����֤������
output   out   -- ����ʱЧ����Ϣ
return   
         0-99                                   //�����ڿ��е�ID
	       PWD_IDY_ONETIME_CODE      0xFE         //һ��������
         PWD_IDY_CODE_NOT_EXIST    0xFF         //���벻����
*/
/******************************************************************************/
uint8_t IdyPassWord(uint8_t *code)
{
		uint8_t i=0,len=0;
	
		uint8_t tmpCode[27]={0};                    //���26λ��������+������
		//��ȡ����������Ϣ
		while(*code!=0)
		{
				tmpCode[i++]=*code++;                   //�ݴ�����
		}                                           
		len=i;    //������Ч���볤��
		if(len<PWD_LEN_MIN)return PWD_IDY_CODE_NOT_EXIST;   //����С����С����
		//��֤�Ƿ���һ��������
		if(len==PWD_LEN_MIN && SysConfig.Bits.OTCode==1)    //����һ�������룬һ��������6λ
		{
				for(i=0;i<PWD_LEN_MIN;i++)
				{
						if(tmpCode[i]!=OneTimeCode.OneCode[i])break;
				}
				if(i==PWD_LEN_MIN)    //һ����������֤һ�μ�ɾ��
				{
						memset((uint8_t*)&OneTimeCode,0,sizeof(OneTimeCode_t));
						SysConfig.Bits.OTCode=0;
						IntFlashWrite((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));
						return PWD_IDY_ONETIME_CODE;
				}
		}
		//��֤��ͨ���룬֧����λ����
		for(i=0;i<PWD_MAX;i++)
		{
				if(Pwd.PwdGenStr[i].Pwd.Type==INVALID)continue;         //��Ч�����˳�
				if(strstr((char*)tmpCode,(char*)Pwd.PwdGenStr[i].Pwd.Buf)!=0)break;
		}
		if(i==PWD_MAX)return PWD_IDY_CODE_NOT_EXIST;           //���벻����
		//���������������룬�ж��Ƿ�����ʱ���룬����ʱ��������֤ʱЧ
		if(Pwd.PwdGenStr[i].Pwd.Type==VISITOR)
		{
				uint16_t tmpst=0,tmpsp=0,tmpnow=0;
				t_ctimes gt={0};
				tm rtc_times={0}; 
				RTC_GetTime(&rtc_times);	//��ȡоƬRTCʱ��
				rtctime_to_mytime(&gt,rtc_times);	//��ȡʱ���
				
				if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimBuf[0]==PWD_TIME_CYC)        //������Ч����
				{
						if((1<<gt.weeks)&Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.Week)  //��������
						{
								tmpnow=gt.hours*60+gt.minutes;
								tmpst=(uint16_t)Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.StartHour*60+Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.StartMin;
								tmpsp=(uint16_t)Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.StopHour*60+Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.StopMin;
								if(tmpnow>=tmpst && tmpnow<=tmpsp)
								{
										return i;          //����Ч����
								}
								else
								{
										return PWD_IDY_CODE_NOT_EXIST; 
								}
						}
						else 
						{
								return PWD_IDY_CODE_NOT_EXIST; 
						}
				}
				else if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimBuf[0]==PWD_TIME_FEQ)   //��Чʱ���ڣ��޴�������
				{
						if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimeFreq.Times>0)
						{
								Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimeFreq.Times--;
								IntFlashWrite(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimBuf,PWD_TIM_DATA_ADDR+Pwd.PwdGenStr[i].Pwd.Idx*sizeof(PwdExtStr_t),sizeof(PwdExtStr_t));   //���´�������д��FLASH
								printf("times left:%d\n",Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimeFreq.Times);
								if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimeFreq.Times==0)   //��������
								{         
										DelPassWord(i);              //ɾ������
								}
								return i;
						}
						else
						{
								return PWD_IDY_CODE_NOT_EXIST;
						}
				}
				else
				{
						return PWD_IDY_CODE_NOT_EXIST;    //�����������������
				}
		}
		//for(j=0;j<13;j++)printf("code: 0x%02x ",Pwd.PwdStr[i].Pwd.Buf[j]);
		return i;
}

/******************************************************************************/
/*
//��֤������Ч��
input:   pPwd  -- ����֤������
output   nLen  -- ���볤��
return   j     -- ��Ч�����λ��
*/
/******************************************************************************/
uint8_t CheckPwdVail(uint8_t* pPwd,uint8_t nLen)
{
		uint8_t j;
	
		for(j=0;j<nLen;j++)
		{
				if(pPwd[j]<'0' || pPwd[j]>'9')break;
		}
		return j;
}
/******************************************************************************/
/*
//��֤ʱЧ�����ʱЧ�����Ƿ���Ч
input:   time  -- ��ʱ����ṹ
output   none
return   0 -- ��Ч   1 -- ��Ч
*/
/******************************************************************************/
uint8_t CheckPwdTimVail(PwdExtStr_t *time)
{
		if(time->PwdTimBuf[0]==PWD_TIME_CYC)      //�ÿ�����
		{
				if(time->PwdTimCyc.Week==0 || time->PwdTimCyc.Week>0x7f)return 0;
				else if(time->PwdTimCyc.StartHour>24)return 0;
				else if(time->PwdTimCyc.StartMin>59)return 0;
				else if(time->PwdTimCyc.StopHour>24)return 0;
				else if(time->PwdTimCyc.StopMin>59)return 0;
				else if(time->PwdTimCyc.TimeOutUtc<1589212800)return 0;    //2020��5��12
				return 1;
		}
		else if(time->PwdTimBuf[0]==PWD_TIME_FEQ)     //��ʱ���� 
		{
				if(time->PwdTimeFreq.Times==0)return 0;
				else if(time->PwdTimeFreq.TimeOutUtc<1589212800)return 0;    //2020��5��12
				
				return 1;
		}
		return 0;
}
/******************************************************************************/
/*
//ɾ����������
input:   none
output   none
return   none
*/
/******************************************************************************/
void DelAllPassword(void)
{
		//ɾ����������
		memset(Pwd.PwdGenStr,0,PWD_MAX*sizeof(PwdGenStr_t));
		//ɾ��ʱЧ����
		memset(Pwd.PwdTimStr,0,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		//д��FLASH
		IntFlashWrite((uint8_t*)&Pwd.PwdGenStr,PWD_DATA_ADDR,PWD_MAX*sizeof(PwdGenStr_t));
		IntFlashWrite((uint8_t*)&Pwd.PwdTimStr,PWD_TIM_DATA_ADDR,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		//���¶�����������
		PassWordInit();
}
/******************************************************************************/
/*
//���ʱЧ�����Ƿ���Ч
input:   none
output   none
return   0 -- ��Ч   1 -- ��Ч
*/
/******************************************************************************/
uint8_t CheckOneTimeCodeValid(OneTimeCode_t* onecode)  
{
		uint8_t i=0;
	
		for(i=0;i<6;i++)
		{
				if(onecode->OneCode[i]<'0' || onecode->OneCode[i]>'9')
				{
						return 0;
				}
		}
		if(onecode->StartTime.hour>23 || onecode->StartTime.minute>59 || onecode->StartTime.second>59)
		{
				return 0;
		}
		return 1;
}
/******************************************************************************/
/*
//��ȡ�����������Ч���룺ȥ��*#
input:   in -- ����������ַ�
         out -- ȥ��*#��ʣ����ַ�
output   none
return   none
*/
/******************************************************************************/
uint8_t GetCodeWithoutX(char *in,char *out)
{
		uint8_t i=0,j=0;
		
		for(i=0;i<strlen(in);i++)
		{
				if(in[i]=='*')
				{
						if(j>0)j--;     
				}
				else if(in[i]=='#')
				{
						j=0;
				}
				else 
				{
						out[j++]=in[i];    
				}
		}
		return 0;
}
/******************************************************************************/
/*
//������Ա�����ǲ���Ĭ�ϵ�����123456
input:   none
output   none
return   0 -- ����Ա���벻��1��2��3��4��5��6
         1 -- ����Ա������1��2��3��4��5��6
*/
/******************************************************************************/
uint8_t CheckAdminCode(void)
{
		uint8_t i=0;
		uint8_t defaultCode[PWD_LEN_MAX]={'1','2','3','4','5','6',0,0,0,0,0,0,0};
		for(i=0;i<PWD_LEN_MAX;i++)
		{
				if(Pwd.PwdGenStr[0].Pwd.Buf[i]!=defaultCode[i])break;
		}
		if(i<PWD_LEN_MAX)return 0;
		
		return 1;
}
/******************************************************************************/
/*
//��֤һ���������Ƿ����
input:   code -- �����һ��������
output   none
return   1 -- �����Ѵ���   0 -- �������
*/
/******************************************************************************/
uint8_t CheckOnetimeCodeExist(uint8_t *code)
{
		uint8_t i=0,j=0,len1=0,len2=0;
	
		len2=strlen((char*)code);
	
		for(i=0;i<PWD_MAX;i++)
		{
				if(Pwd.PwdGenStr[i].Pwd.Type==INVALID)continue;
				
				len1=strlen((char*)Pwd.PwdGenStr[i].Pwd.Buf);
				
				if(len1==len2)
				{
						for(j=0;j<len1;j++)
						{
								if(Pwd.PwdGenStr[i].Pwd.Buf[j]!=code[j])break;
						}
						if(j==len1)return 1;
				}
		}
		return 0;
}

/******************************************************************************/
/*
//�ж���ʱ�����Ƿ�ʧЧ

input:   stime--ָ��ʱ��
output   none
return   ʱ��
            -1 ----- ��ǰʱ��С��ָ��ʱ��
            >0 ----- ʱ���
*/
/******************************************************************************/
void CheckTempCodeValid(void)
{
		uint8_t i=0;
		uint32_t timestampnow=0;
		t_ctimes gt={0};
		tm rtc_times={0}; 
		
		if(NetTime.NT.Stat==0)return;   //δ��ȡ��ʱ�䲻���
		
		//ʱ��ת��δʱ���
		RTC_GetTime(&rtc_times);	//��ȡоƬRTCʱ��
		rtctime_to_mytime(&gt,rtc_times);	//��ȡʱ���
		timestampnow=mytime_to_utc(&gt,0);
		//�ж���ʱ�����Ƿ�ʧЧ
	  for(i=0;i<PWD_MAX;i++)
		{
				if(Pwd.PwdGenStr[i].Pwd.Type==VISITOR)        //��ʱ����
				{
						if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimBuf[0]==PWD_TIME_CYC)              //��������
						{
								if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimCyc.TimeOutUtc<timestampnow)   //ʧЧʱ�䵽
								{
										DelPassWord(i);
								}
						}
						else if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimBuf[0]==PWD_TIME_FEQ)
						{
								if(Pwd.PwdTimStr[Pwd.PwdGenStr[i].Pwd.Idx].PwdTimeFreq.TimeOutUtc<timestampnow)   //ʧЧʱ�䵽
								{
										DelPassWord(i);
								}
						}
				}
		}
}


