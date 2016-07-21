/**
  ******************************************************************************
  * @file    Speed_control
  * @author  zhangyan
  * @date    2016.06.21
  * @note    ���������ٶȵĶ�ȡ
  ******************************************************************************
  */
#include "speed_control.h"


/**
  * @brief    speedcontrol_init
  * @author  zhangyan
  * @date    2016.06.21
  * @note    ���������ĳ�ʼ��
  */
void speedcontrol_init(void)
{
    FTM_QD_QuickInit( FTM1_QD_PHA_PA12_PHB_PA13, kFTM_QD_NormalPolarity,kQD_PHABEncoding);
    FTM_QD_QuickInit( FTM2_QD_PHA_PA10_PHB_PA11, kFTM_QD_NormalPolarity,kQD_PHABEncoding);
}


/**
  * @brief    speedcontrol_read
  * @author  zhangyan
  * @date    2016.06.21
  * @note    ��С���ٶȲ��Ҽ�������
  */
void  speedcontrol_read(int *value_left,int *value_right)
{
    static uint8_t dir1;
    static uint8_t dir2;
    FTM_QD_GetData(HW_FTM1,value_left, &dir1);
    FTM_QD_GetData(HW_FTM2,value_right,&dir2);
    
    FTM_QD_ClearCount(HW_FTM1);
    FTM_QD_ClearCount(HW_FTM2);

}

