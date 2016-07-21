/**
  ******************************************************************************
  * @file    ledcontrol.c
  * @author  zhangyan
  * @date    2016.06.23
  * @note    ���ļ�ΪledС���������
  ******************************************************************************
  */
#include "ledcontrol.h"

 /**
 * @brief  ���ٳ�ʼ��һ��GPIO���� ʵ������GPIO_Init���������
 * @code
 *      //��ʼ������PORTB�˿ڵ�10����Ϊ�����������
 *      GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);
 * @endcode
 * @param[in]  instance GPIOģ���
 *              @arg HW_GPIOA оƬ��PORTA�˿�
 *              @arg HW_GPIOB оƬ��PORTB�˿�
 *              @arg HW_GPIOC оƬ��PORTC�˿�
 *              @arg HW_GPIOD оƬ��PORTD�˿�
 *              @arg HW_GPIOE оƬ��PORTE�˿�
 * @param[in]  pinx �˿��ϵ����ź� 0~31
 * @param[in]  mode ���Ź���ģʽ
 *              @arg kGPIO_Mode_IFT ��������
 *              @arg kGPIO_Mode_IPD ��������
 *              @arg kGPIO_Mode_IPU ��������
 *              @arg kGPIO_Mode_OOD ��©���
 *              @arg kGPIO_Mode_OPP �������
 * @retval instance GPIOģ���
 */
void led_init(void)
{
    GPIO_QuickInit(HW_GPIOE,6,kGPIO_Mode_OOD);
    GPIO_QuickInit(HW_GPIOE,7,kGPIO_Mode_OOD);
    GPIO_QuickInit(HW_GPIOE,11,kGPIO_Mode_OOD);
    GPIO_QuickInit(HW_GPIOE,12,kGPIO_Mode_OOD);

}

 /**
 * @brief  ����ָ����������ߵ�ƽ���ߵ͵�ƽ
 * @note   �������������ó��������
 * @code
 *      //����PORTB�˿ڵ�10��������ߵ�ƽ
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param[in]  instance GPIOģ���
 *              @arg HW_GPIOA оƬ��PORTA�˿�
 *              @arg HW_GPIOB оƬ��PORTB�˿�
 *              @arg HW_GPIOC оƬ��PORTC�˿�
 *              @arg HW_GPIOD оƬ��PORTD�˿�
 *              @arg HW_GPIOE оƬ��PORTE�˿�
 * @param[in]  pin  �˿��ϵ����ź� 0~31
 * @param[in]  data ���ŵĵ�ƽ״̬  
 *              @arg 0  �͵�ƽ 
 *              @arg 1  �ߵ�ƽ
 * @retval None
 */
void led_ctl(uint16_t name,uint8_t data)
{
    if(name &   LEDCTL_A)   GPIO_WriteBit(HW_GPIOE,6,data);
    if(name &   LEDCTL_B)   GPIO_WriteBit(HW_GPIOE,7,data);
    if(name &   LEDCTL_C)   GPIO_WriteBit(HW_GPIOE,11,data);
    if(name &   LEDCTL_D)   GPIO_WriteBit(HW_GPIOE,12,data);
    
}

/**
 * @brief  ��תһ�����ŵĵ�ƽ״̬
 * @code
 *      //��תPORTB�˿ڵ�10���ŵĵ�ƽ״̬
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param[in]  instance: GPIOģ���
 *              @arg HW_GPIOA оƬ��PORTA�˿�
 *              @arg HW_GPIOB оƬ��PORTB�˿�
 *              @arg HW_GPIOC оƬ��PORTC�˿�
 *              @arg HW_GPIOD оƬ��PORTD�˿�
 *              @arg HW_GPIOE оƬ��PORTE�˿�
 * @param[in]  pin  �˿��ϵ����ź� 0~31
 * @retval None
 */
void led_toggle(uint16_t name)
{
    
    if(name&LEDCTL_A)   GPIO_ToggleBit(HW_GPIOE,6);
    if(name&LEDCTL_B)   GPIO_ToggleBit(HW_GPIOE,7);
    if(name&LEDCTL_C)   GPIO_ToggleBit(HW_GPIOE,11);
    if(name&LEDCTL_D)   GPIO_ToggleBit(HW_GPIOE,12);
    
}
