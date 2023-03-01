#ifndef  __AUDIO_H_
#define  __AUDIO_H_



/********************************************************/
/*
����������������Ϊ�Ŵ���������Ҫ��С
*/
/********************************************************/
#include "audio_service.h"


#define     AUDIO_EN_PIN        I2C_SCL


/*************************************/

typedef union{
		struct{
				uint8_t volume;
				uint8_t volmin;
				uint8_t volmax;
				uint8_t volflag;
		}Item;
		uint8_t vol[4];
}VolStr_t;

/*************************************/
#define    IDY_FACE     (1<<0)
#define    IDY_FP       (1<<1)
#define    IDY_CODE     (1<<2)
#define    IDY_CARD     (1<<3)


typedef union{
		struct{
				uint8_t idy0;            //��֤��ʶ
				uint8_t idy1;            //��֤״̬
				uint8_t idyen;
				uint8_t idyflag;         //ʹ�����
		}Item;
		uint8_t idy[4];
}MutiIdy_t;

/*************************************/


//extern VolStr_t SysVolume;
//extern MutiIdy_t MutiIdy;



void Audio_Init(void);
void Audio_DeInit(void);
void Audio_RunningHandle(void);

void Audio_PowerOn(void);
void Audio_PowerOff(void);
void AudioPlayVoice(uint16_t num,PlayMode_t mode);
void AudioPlayNumString(char* str);

void Audio_SetVolume(uint8_t sel);
void AudioTest(void);

#endif


