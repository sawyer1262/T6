#ifndef __CLK_SWITCH_DRV__
#define __CLK_SWITCH_DRV__

enum{
	OSC_108M_HZ = 0,
	OSC_120M_HZ ,
	OSC_150M_HZ,
	OSC_160M_HZ,		//����2��Ƶ80Mʹ��
};

extern void CPM_OSC_Switch(int osc_sel);

#endif // end __CLK_SWITCH_DRV__

