#include "AD.h"

/**
 * @brief  AD_init
 * @code
 *    //��ʼ�� ADC0 ͨ��20 ����DM1 ���� ���� 12λ
 *    ADC_QuickInit(ADC0_SE20_DM1, kADC_SingleDiff12or13);
 *    //��ȡADת�����
 *    value = ADC_QuickReadValue(ADC0_SE20_DM1);
 * @retval void
 *note       ADC�ĳ�ʼ��
 */
void  AD_init(void)
{   
    ADC_InitTypeDef adcstructionadc0;
    adcstructionadc0.instance   =   HW_ADC0;
    adcstructionadc0.triggerMode    =   kADC_TriggerSoftware;
    adcstructionadc0.clockDiv   =   kADC_ClockDiv2;
    adcstructionadc0.resolutionMode =   kADC_SingleDiff12or13;
    adcstructionadc0.singleOrDiffMode   = kADC_Single;
    adcstructionadc0.continueMode   =   kADC_ContinueConversionDisable;
    adcstructionadc0.hardwareAveMode    = kADC_HardwareAverageDisable;
    adcstructionadc0.vref   =   kADC_VoltageVREF;
          
    ADC_InitTypeDef adcstructionadc1;
    adcstructionadc1.instance = HW_ADC1;                   ///<ģ���
    adcstructionadc1.triggerMode = kADC_TriggerSoftware;               ///<����ģʽ ������� �� Ӳ������
    adcstructionadc1.clockDiv = kADC_ClockDiv2;                   ///<ADCʱ�ӷ�Ƶ
    adcstructionadc1.resolutionMode = kADC_SingleDiff12or13;             ///<��Ƶ��ѡ�� 8 10 12 16λ���ȵ�
    adcstructionadc1.singleOrDiffMode = kADC_Single;           ///<���� ���� �������
    adcstructionadc1.continueMode = kADC_ContinueConversionDisable;               ///<�Ƿ���������ת��
    adcstructionadc1.hardwareAveMode = kADC_HardwareAverageDisable;            ///<Ӳ��ƽ������ѡ��
    adcstructionadc1.vref = kADC_VoltageVREF;                       ///<ģ���ѹ�ο�Դ
    
    PORT_PinMuxConfig(HW_GPIOE,24,kPinAlt0);
    PORT_PinMuxConfig(HW_GPIOE,25,kPinAlt0);
//    PORT_PinMuxConfig(HW_GPIOA,7,kPinAlt0);
    PORT_PinMuxConfig(HW_GPIOA,8,kPinAlt0);
    
    ADC_Init(&adcstructionadc0);
    ADC_Init(&adcstructionadc1);
    
    ADC_ChlMuxConfig(HW_ADC0,kADC_MuxA);
    ADC_ChlMuxConfig(HW_ADC1,kADC_MuxA);
    
}


/**
 * @brief  AD_Read
 *    value = ADC_QuickReadValue(ADC0_SE20_DM1);
 * @retval void
 *note       ADC�����ݶ�ȡ
*/
void AD_Read(int32_t* leftA_data,int32_t* leftB_data,int32_t* rightA_data,int32_t* rightB_data)
{
   
    ADC_StartConversion(HW_ADC0,17,kADC_MuxA);                  //PTE24
    while(ADC_IsConversionCompleted(HW_ADC0,kADC_MuxA));
    *leftA_data  =   ADC_ReadValue(HW_ADC0,kADC_MuxA);
    
    ADC_StartConversion(HW_ADC0,11,kADC_MuxA);                  //ADC1_SE16
    while(ADC_IsConversionCompleted(HW_ADC0,kADC_MuxA));           
    *leftB_data  =   ADC_ReadValue(HW_ADC0,kADC_MuxA);
    
    ADC_StartConversion(HW_ADC0,18,kADC_MuxA);              //ADC0_SE16
    while(ADC_IsConversionCompleted(HW_ADC0,kADC_MuxA));
    *rightA_data  =   ADC_ReadValue(HW_ADC0,kADC_MuxA);
    
    ADC_StartConversion(HW_ADC0,23,kADC_MuxA);              //ADC0_SE23
    while(ADC_IsConversionCompleted(HW_ADC0,kADC_MuxA));
    *rightB_data  =   ADC_ReadValue(HW_ADC0,kADC_MuxA);
    

}

