/**
  ******************************************************************************
  * @file    main.c
  * @author  zhangyan
  * @date    2016.06.14
  * @note    ���ܳ�����ģ��
  *          
  ******************************************************************************
  */
  

#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "ftm.h"
#include "pit.h"

#include "AD.h"
#include "bluetooth.h"
#include "speed_control.h"
#include "timecontrol.h"
#include "mpu_transplant.h"
#include "nrfcontrolyan.h"
#include "motorcontrol.h"
#include "flashcontrol.h"
#include "ledcontrol.h"

#include "shell.h"
#include "stdbool.h"
#include "stdlib.h"
#include "QUEUEyan.h"

#include <math.h>

/////////////////////////////////////////////
/*�궨��*/

//ControlBus timeUS ��ʱ���ж�ʱ��
#define ControlBus_timeValue 1000
//msgdeal and flash W/R ��ʱ���ж�ʱ��
#define msgdealISR_timeValue 1000*50
//�ٶȿ���ƽ������
#define SPEED_CONTROL_PERIOD 20
//�������ƽ���ܴ���
#define DIRECTION_CONTROL_PERIOD 2
/////////////////////////////////////////////


/////////////////////////////////////////////
 /*���ݽṹ��*/
 //��������  
bluetoothsend blstruction;

//ʱ����ת�ṹ��
TIMqueue timeturnstruction;

SqQueue detetionbendque;

/////////////////////////////////////////////


/////////////////////////////////////////////
/*ȫ�ֱ�������*/

//С���ٶ�
static float g_fCarSpeed;
//�ٶȿ��Ʋ���
//static float g_fspeedcontrolargp;
//�ٶȿ��ƻ���
static float g_fSpeedControlIntegral;
//�ٶ�����¾�ֵ
static float g_fSpeedControlOutOld = 0 ;
static float g_fSpeedControlOutNew = 0;
//�ٶȿ������
static float g_fSpeedControlOut;
//�ٶȿ��Ƶ�ǰ����
static uint8_t g_nSpeedControlPeriod =0;
//�ǶȽ��ٶ�
static float g_fCarAngle,g_fGyroscopeAngleSpeed;
//�Ƕ����
static float g_fAngleControlOut;
//���ҵ�����
static float g_fLeftMotorOut;
static float g_fRightMotorOut;

//������ٶ�
static float g_fDirControlDValue;

//�ɵķ�����Ƶ�ֵ
static float g_fDirectionControlOutOld;
//�µķ�����Ƶ�ֵ
static float g_fDirectionControlOutNew;

//����ƽ�������
static float g_fDirectionControlOut;

//������Ƶ�ǰ����
static uint8_t g_nDirectionControlPeriod;


/*���ݷ��Ϳ���*/
bool valuecontrol = 0;
/*��ͣ����*/
uint8_t StopMark = 0;
/*����жϱ�Ǳ��*/
uint8_t BendMark;

/////////////////////////////////////////////


/////////////////////////////////////////////
/*��������*/
void ControlBus(void);
void init_start(void);
void msgdealWR(void);
void testbluetooth(void);
void test_speed_control_read(void);
void angle_read(void);
void AngleControl(void);
void MotorOutput(void);
void MotorSpeedRead(void);
void AD_init(void);
void DirectionControl(void);
void leddisplay(void);
void delaytaskstart(void);
float detetionbend(float value);//��������˲�
void stopcontroldelay(void);
////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ������
 * @param[in]  void 
 * @retval None
 * @note 
 */
int main(void)
{

    
    init_start();


    while(1)
    {

        
        shell_main_loop("byGTA^_^>>");
        DelayMs(100);

        
    }
}
                              
////////////////////////////////////////////////////////////////////////////////////////////
                                /*��ʱ���ж�*/
//////////////////////////////////////////////////////////////////////
/**
 * @brief  ControlBus
 * @param[in]  void 
 * @retval None
 * @note ��ʱ���ж� ���ݵĶ�ȡ�ʹ��� ���ڣ�1MS
 */
void ControlBus(void)
{
    
    timeCTL_TaskqueueExecute(&timeturnstruction);
}
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
/**
 * @brief  msgdealWR
 * @param[in]  void 
 * @retval None
 * @note ��ʱ���ж�  ����ͨѶ��flash��д ���ڣ�msgdealISR_timeValue
 */
void msgdealWR(void)
{
  
  if(valuecontrol == true)\
      bluetooth_send();
    
}

//ֹͣ�����ж�
void stop_isr(uint32_t array)
{
    static uint8_t count=0;
    if(array & (1 << 7))
    {
        GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_Low, false);
        count++;
        if(count==1)
        {
          StopMark = 0;
          count = 0;
          led_ctl(LEDCTL_B,0);
        }
        
//        GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_Low, true);
    }
}
  

//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*ģʽѡ��*/
void modeselect(void)
{
    static uint8_t flag;
    GPIO_QuickInit(HW_GPIOE,5,kGPIO_Mode_IPD);
    GPIO_QuickInit(HW_GPIOE,1,kGPIO_Mode_IPD);
    GPIO_QuickInit(HW_GPIOE,4,kGPIO_Mode_IPD);
    GPIO_QuickInit(HW_GPIOE,3,kGPIO_Mode_IPD);
  
    flag=GPIO_ReadBit(HW_GPIOE,3)|GPIO_ReadBit(HW_GPIOE,4)<<1|GPIO_ReadBit(HW_GPIOE,1)<<2|GPIO_ReadBit(HW_GPIOE,5)<<3;
    led_ctl(flag,0);
    
}

/**
 * @brief  ������ܳ���������
 * @param[in]  void 
 * @retval None
 * @note 
 */
void init_start(void)
{
////////////////////
    /*ϵͳ��ʼ��*/
    DelayInit();
  
    /*��ʱ Ӳ����ʼ���ȴ� */
    DelayMs(10);
    
    
//////////////////////////////////////////////////////////////////////////////////
    /*��ʼ����ʱ���ж�*/
  
    /*��ʼ���ж����ȼ�*/
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
    NVIC_SetPriority(PORTA_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_2,0,0));
    NVIC_SetPriority(PIT0_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_2,0,2));
    NVIC_SetPriority(PIT1_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_2,1,2));  
//    NVIC_SetPriority(PIT2_IRQn,NVIC_EncodePriority(NVIC_PriorityGroup_2,2,2));  
    /*���䶨ʱ��Ӳ�ж�*/
    /*smartcarcontror*/
    PIT_QuickInit(HW_PIT_CH0,ControlBus_timeValue);         // ���ٳ�ʼ��PITģ���0ͨ��
    PIT_CallbackInstall(HW_PIT_CH0,ControlBus);             //���ٳ�ʼ��  ��ʱ���ж� ���ݵĶ�ȡ�ʹ���
  
    /*msgdeal and flash W/R*/
    PIT_QuickInit(HW_PIT_CH1,msgdealISR_timeValue);         //���ٳ�ʼ��PITģ���1ͨ��
    PIT_CallbackInstall(HW_PIT_CH1,msgdealWR);              //���ٳ�ʼ��PITģ���1ͨ������ͨѶ
    
       
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU); /* KEY */

    /* ����GPIO�ⲿ�����жϻص����� */
    GPIO_CallbackInstall(HW_GPIOA, stop_isr);
    /* �ж� ���� */
    GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_Low, false);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
    /*��ʼ��ʱ��Ƭ����*/
    timeCTL_TurntableInit(&timeturnstruction);
    timeCTL_TaskqueueInit(&timeturnstruction);
    
    InitQueue(&detetionbendque);
//////////////////////////////////////////////////////////////////////////////////
    /*��ʼ��������������*/   
    
    bluetoooth_init();
    speedcontrol_init();
    mpu6050_init();
    nrfcontrol_init();
    motorcontrol_init();
    led_init();
    led_ctl(LEDCTL_A|LEDCTL_B | LEDCTL_C |LEDCTL_D,1);
    AD_init(); 
    flashcontrol_init(2);
//    modeselect();
//////////////////////////////////////////////////////////////////////////////////    
    /*shell��ʼ��*/
    shell_init();

//////////////////////////////////////////////////////////////////////////////////
    /*����ִ������*/
    timeCTL_TaskqueueInsert(&timeturnstruction,leddisplay,5);
    timeCTL_TaskqueueInsert(&timeturnstruction,angle_read,6);
    timeCTL_TaskqueueInsert(&timeturnstruction,MotorOutput,8);
    timeCTL_TaskqueueInsert(&timeturnstruction,MotorSpeedRead,7);
    timeCTL_TaskqueueInsert(&timeturnstruction,DirectionControl,9);
    timeCTL_TaskqueueInsert(&timeturnstruction,stopcontroldelay,100);
//////////////////////////////////////////////////////////////////////////////////
    /*��ʱ���жϴ�*/
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,ENABLE); //���ж�
    PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF,ENABLE); //���ж�

  
    

}
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ���ܳ�������
 * @note   �����Ƕ��ٶȷ�����Ƽ���������������
 */


/**
 * @brief  8s��ʱ���յ�ֹͣ
 * @param[in]  void 
 * @retval None
 * @note 
 */


void stopcontroldelay(void)
{
    static uint16_t count=0;
    if(StopMark)
      count++;
    else
      count = 0;
    if(count>70)
      GPIO_ITDMAConfig(HW_GPIOA, 7, kGPIO_IT_Low, true);
    
  timeCTL_TaskqueueInsert(&timeturnstruction,stopcontroldelay,100);
}

/**
 * @brief  ʵ��LED��˸
 * @param[in]  void 
 * @retval None
 * @note 80ms��ת
 */

void leddisplay(void)
{
    
    led_toggle(LEDCTL_A);
    timeCTL_TaskqueueInsert(&timeturnstruction,leddisplay,80);
}

/**
 * @brief  �ǶȽ��ٶȶ�ȡ 5ms
 * @param[in]  void 
 * @retval None
 * @note ��MPU6050�ж�ȡ�ǶȽ��ٶȣ�
 *       1 ����roll��X��Ƕ�ֵ���� ����g_fGyroscopeAngleSpeed X����ٶ� �� ���� g_fGyroscopeAngleSpeed ����ֱ������
 *       2 ����z����ٶ�ֵ      �� ����g_fDirControlDValue ���ڷ������
 *       3 ִ��AngleControl()
 */
void angle_read(void)
{
  
    float   pitch;
    float   roll;
    float   yaw;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
  

  //��ȡmpu6050ֵ
    mpu6050_readangle_dmp( &pitch, &roll, &yaw, &gyro_x, &gyro_y, &gyro_z);
    
  //����z����ٶ�
    g_fDirControlDValue = gyro_z;
  //����x��ǶȺͽ��ٶ�
    g_fCarAngle = roll;
    g_fGyroscopeAngleSpeed = gyro_x;



    AngleControl();
    
    //���뵽ʱ���������

    timeCTL_TaskqueueInsert(&timeturnstruction,angle_read,5);
  
}
/*Ŀ��Ƕ�ѡ��*/
float AngleTarget(uint8_t flag)
{
    switch(flag)
    {
      case 1: return flashcontrol_read(CAR_ANGLE_SET);
      case 0: return flashcontrol_read(CAR_ANGLE_SET_BEND);
      case 2: return flashcontrol_read(CAR_ANGLE_SET_START);
    }
      return 1;
    
}

/**
 * @brief  �Ƕȿ��Ƽ��㲢����
 * @param[in]  void 
 * @retval None
 * @note ����PID���㣬��ֵ���� ������ �������ֵ �� ����g_fAngleControlOut
 */
void AngleControl(void)
{
    static uint16_t count;
    float fValue;
    uint8_t flag=0;
    if(StopMark)
    {
      if(count<600)
      {
          count++;
          flag =2;
      }
      else
      {
          flag = 1;
      } 
    }
    else
    {
        count =0;
    
    }
    //PID����
    fValue =(AngleTarget(flag) - g_fCarAngle) *flashcontrol_read(ANGLE_CONTROL_P)\
            -g_fGyroscopeAngleSpeed * flashcontrol_read(ANGLE_CONTROL_D);
    
    //��ֵ����
    if(fValue > flashcontrol_read(ANGLE_CONTROL_OUT_MAX))
        fValue = flashcontrol_read(ANGLE_CONTROL_OUT_MAX);
    else if(fValue < flashcontrol_read( ANGLE_CONTROL_OUT_MIN))
        fValue = flashcontrol_read( ANGLE_CONTROL_OUT_MIN);
    //�������ֵ
    g_fAngleControlOut = fValue;
}

/**
 * @brief  ����������
 * @param[in]  fLeftVoltage �������ֵ
 * @param[in]  fRightVoltage  �ҵ�����ֵ
 * @retval None
 * @note 
         1 �����������ֵ ����4·PWM �Ӷ��ﵽ���Ƶ�����
         2 ���ݱ�־λStopMark ���Ƶ���Ƿ�����
 */
void SetMotorVoltage(float fLeftVoltage, float fRightVoltage)
{
    uint32_t backleft ,frontleft,backright,frontright;
    backleft = frontleft = backright = frontright =0;
    
    if(fLeftVoltage > 0 ) \
        frontleft = fLeftVoltage+flashcontrol_read(MOTOR_CORRECT_VAL_L_FRONT);
    else \
        backleft  = - (fLeftVoltage+flashcontrol_read(MOTOR_CORRECT_VAL_L_BACK));
    
    if(fRightVoltage > 0 ) \
        frontright = fRightVoltage+flashcontrol_read(MOTOR_CORRECT_VAL_R_FRONT);
    else \
        backright  = - (fRightVoltage+flashcontrol_read(MOTOR_CORRECT_VAL_R_BACK));
    /*�������*/
    if(StopMark) \
      motorcontrol_TRL(backleft,frontleft,backright,frontright);
    else \
      motorcontrol_TRL(0,0,0,0);
}

/**
 * @brief  �������˷�������ѹ�����������ֵ
 * @param[in]  void 
 * @retval None
 * @note 
          1 ��ȡ g_fLeftMotorOut,g_fRightMotorOut 
          2 �������������������ѹ
          3 ��������������ֵ
          4 ִ�� SetMotorVoltage(fLeftVal, fRightVal)
 */
void MotorSpeedOut()
{
    
    float fLeftVal, fRightVal;
    
    fLeftVal = g_fLeftMotorOut;
    fRightVal = g_fRightMotorOut;
  
    //����������ѹ
    if(fLeftVal > 0)
        fLeftVal += flashcontrol_read(MOTOR_OUT_DEAD_VAL_L);
    else if(fLeftVal < 0)
        fLeftVal -= flashcontrol_read(MOTOR_OUT_DEAD_VAL_L);
    
    if(fRightVal > 0)
        fRightVal +=flashcontrol_read(MOTOR_OUT_DEAD_VAL_R);
    else if(fRightVal < 0)
        fRightVal -=flashcontrol_read(MOTOR_OUT_DEAD_VAL_R);
    
    //������ֵ
    if(fLeftVal >flashcontrol_read( MOTOR_OUT_MAX )) //��ת���ֵ
        fLeftVal = flashcontrol_read (MOTOR_OUT_MAX);
    if(fLeftVal < flashcontrol_read(MOTOR_OUT_MIN))  //��ת���ֵ
        fLeftVal = flashcontrol_read(MOTOR_OUT_MIN);
    if(fRightVal > flashcontrol_read(MOTOR_OUT_MAX))
        fRightVal = flashcontrol_read(MOTOR_OUT_MAX);
    if(fRightVal < flashcontrol_read(MOTOR_OUT_MIN))
        fRightVal = flashcontrol_read(MOTOR_OUT_MIN);
    
    SetMotorVoltage(fLeftVal, fRightVal);
}

/**
 * @brief  �ۼ��������ֵ׼�������� 5ms
 * @param[in]  void 
 * @retval None
 * @note ��ֱ���������ֵ,�ٶȿ������ֵ,��������ֵ�ں� ִ�� MotorSpeedOut()
 */

void MotorOutput(void) 
{
    float fLeft, fRight;


    fLeft = g_fAngleControlOut - g_fSpeedControlOut -g_fDirectionControlOut;
    fRight = g_fAngleControlOut- g_fSpeedControlOut +g_fDirectionControlOut;
    
    g_fLeftMotorOut = fLeft;
    g_fRightMotorOut = fRight;
    MotorSpeedOut();
//    if(StopMark) 
        timeCTL_TaskqueueInsert(&timeturnstruction,MotorOutput,5);

}

/**
 * @brief  ƽ�����㷢����ʵ�ٶȿ������
 * @param[in]  void 
 * @retval None
 * @note ���ٶ����ֵg_fSpeedControlOutNew ����ƽ������ �ֳ� 20�� �ﵽĿ�����ֵ��g_fSpeedControlOutNew���������ܳ�����
 */
void SpeedControlOutput(void) {
    
    float fValue;
    fValue = g_fSpeedControlOutNew - g_fSpeedControlOutOld;
    
    g_fSpeedControlOut = fValue * (g_nSpeedControlPeriod + 1) /
    SPEED_CONTROL_PERIOD + g_fSpeedControlOutOld;
    
}


/**
 * @brief  �ٶȶ�ȡ��� 5ms
 * @param[in]  void  
 * @retval None
 * @note 
          1 ���д����ۼ�
          2 �ۼ�100ms��20�Σ� �����ٶȶ�ȡ������PID����
          3 ÿ5msִ�� SpeedControlOutput()
 */
void MotorSpeedRead(void)
{
    float fDelta;
    float fP, fI;
    int32_t leftspeed;
    int32_t rightspeed;
    float f_leftspeed;
    float f_rightspeed;
    int16_t i_leftspeed;
    int16_t i_rightspeed;
    if(g_nSpeedControlPeriod==0){
    //��ȡ�ٶ�
    speedcontrol_read(&leftspeed,&rightspeed);
      
    //��ȡ����ֵ�Ƚ��е�λת����int16 �Ӷ��õ���ȷ��������ʾֵ
    i_leftspeed = leftspeed;
    i_rightspeed = rightspeed;      
    i_rightspeed = -i_rightspeed;
    f_leftspeed =(float)i_leftspeed;
    f_rightspeed =(float)i_rightspeed;
    //�õ�С����ǰ�ٶ�
    g_fCarSpeed = (f_leftspeed+f_rightspeed)/2;
//    g_fCarSpeed *= CAR_SPEED_CONSTANT;�ȱ���ת���ٶȵ�λ��

    //������Ŀ���ٶȲ�ֵ
    fDelta =  flashcontrol_read(CAR_SPEED_SET) - g_fCarSpeed;
    //����PID����
      
    fP = fDelta * flashcontrol_read(SPEED_CONTROL_P);//flashcontrol_read(SPEED_CONTROL_P);
    //����
    fI = fDelta * flashcontrol_read(SPEED_CONTROL_I);
    if(StopMark) \
        g_fSpeedControlIntegral += fI;      
    else g_fSpeedControlIntegral =0;

    g_fSpeedControlOutOld = g_fSpeedControlOutNew;   
    g_fSpeedControlOutNew = fP + g_fSpeedControlIntegral;
    

    }
    
    SpeedControlOutput();
    //���д����ۼ�
    g_nSpeedControlPeriod++;
    if(g_nSpeedControlPeriod==20) g_nSpeedControlPeriod = 0;
    //���뵽ʱ���������
    
        timeCTL_TaskqueueInsert(&timeturnstruction,MotorSpeedRead,5);
}

/**
 * @brief  ƽ�����㷢����ʵ����������
 * @param[in]  void 
 * @retval None
 * @note ��g_fDirectionControlOutֵ ����ƽ����� ͬ��
 */
void DirectionControlOutput(void) 
{
    float fValue;
    fValue = g_fDirectionControlOutNew - g_fDirectionControlOutOld;
    g_fDirectionControlOut = fValue * (g_nDirectionControlPeriod + 1) / DIRECTION_CONTROL_PERIOD + g_fDirectionControlOutOld;

}

/**
 * @brief  ���������� 5ms
 * @param[in]  void 
 * @retval None
 * @note 
          1 ���д����ۼ�
          2 �ۼ�10ms��2�Σ� ���з����ȡ������PID����
          3 ÿ5msִ�� DirectionControlOutput()
 */

void DirectionControl(void)
{
    float sampling;
    int32_t leftA_data,leftB_data,rightA_data, rightB_data;
    int16_t i=0,left_add = 0,right_add = 0;
    float nLeft,nRight,fLeftRightAdd,fLeftRightSub,fValue,fDValue;
    float fLeftVoltageSigma ,fRightVoltageSigma;
    if(g_nDirectionControlPeriod==0)
    {
        for(i=0;i<=3;i++)
        {
            AD_Read(&leftA_data,&leftB_data,&rightA_data,&rightB_data);
            left_add  +=rightB_data+rightA_data;
            right_add +=leftB_data+leftA_data;
          
        }
        fLeftVoltageSigma = left_add/4;
        fRightVoltageSigma = right_add/4;
       

           
        nLeft = fLeftVoltageSigma-flashcontrol_read(DIR_ZEROSHIFT_LEFTB);
        nRight = fRightVoltageSigma-flashcontrol_read(DIR_ZEROSHIFT_RIGHTB); 
            
        //�������ݷ���

        fLeftRightAdd = nLeft + nRight;
        fLeftRightSub = nLeft - nRight;
        g_fDirectionControlOutOld = g_fDirectionControlOutNew;
        if(fLeftRightAdd < flashcontrol_read(DIR_LEFT_RIGHT_MINIMUM)) 
            g_fDirectionControlOutNew = 0;
        else 
        {
            fValue = fLeftRightSub * flashcontrol_read(DIR_CONTROL_P)/ fLeftRightAdd;
            fDValue = g_fDirControlDValue;
            fDValue *=flashcontrol_read(DIR_CONTROL_D);          
            fValue += fDValue;
            g_fDirectionControlOutNew = fValue;
            sampling = fValue;
            if(sampling<0) sampling=-sampling;
            if(detetionbend(sampling)<1200) BendMark = 0;
            else BendMark=1;
            blstruction.test1 = bluetoooth_int_16_Exchange((int16_t)BendMark);
           
        } 
        
    }
    DirectionControlOutput();
 
    g_nDirectionControlPeriod++;
    if(g_nDirectionControlPeriod == DIRECTION_CONTROL_PERIOD) g_nDirectionControlPeriod =0;
    
    
        timeCTL_TaskqueueInsert(&timeturnstruction,DirectionControl,5);

}

//���ƶ����ж����
float detetionbend(float value)
{
    uint16_t length = QueueLength(detetionbendque);
    float max;
    if(length<(MAXSIZEQUE-1))
    {
        EnQueue(&detetionbendque,value);
         for(int i=0;i<=length;i++)
         {
          if(detetionbendque.data[i]>max)
              max=detetionbendque.data[i];
         }
        return max ;
    }
    else
    {
        DeQueue(&detetionbendque);
        EnQueue(&detetionbendque,value);
         for(int i=0;i<=length;i++)
         {
          if(detetionbendque.data[i]>max)
              max=detetionbendque.data[i];
         }
        return max ;
    }        

}

////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief  ���ܳ�����ƽ̨
 * @note   ͨ�����ڿ��ƵĽӿں���
 */



/*shell������ƺ���*/

/*hello*/
int CMD_hello(int argc,char *const argv[])
{
    printf("hello my owner\r\n");
  
    return 0;
}

/*�򿪴������ݷ��Ϳ���*/
int CMD_valuesendcontrol(int argc,char *const argv[])
{
      
    if( atoi(argv[argc-1]) > 1|| atoi(argv[argc-1]) <0)
    {
      printf("error:controlvalue is 0(off) or 1(on)\r\n");
      return 1;
    }
    
    valuecontrol = atoi(argv[argc-1]);
    printf("set OK controlvalue is %s \r\n",argv[argc-1]);
    return 0;
}

/*�����ܳ�д��flash����*/
int CMD_flashwrite(int argc,char *const argv[])
{
  if(atoi(argv[argc-2])<0||atoi(argv[argc-2])>255)
  {
    printf("error:flash address is 0 to 255\r\n");
    return 1;
  }
  if(atoi(argv[argc-2])%2)
  {
    printf("error:flash address is even number\r\n");
    return 1;
  }
  
  PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,DISABLE);
  if(flashcontrol_write(atoi(argv[argc-2]),atof(argv[argc-1])))
  {
    printf("flash write error\r\n");
    return 1;
  }
  PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,ENABLE);
  printf("flash write ok address is %s value is %s\r\n",argv[argc-2],argv[argc-1]);
  return 0;
}

/*���ܳ����п���*/
int CMD_startswitch(int argc,char *const argv[])
{
      if( atoi(argv[argc-1]) > 1|| atoi(argv[argc-1]) <0)
    {
      printf("error:switch is 0(off) or 1(on)\r\n");
      return 1;
    }
    StopMark = atoi(argv[argc-1]);
    printf("set OK switch is %s \r\n",argv[argc-1]);
    return 0;
}

/*��ȡflash��д���ֵ*/
int CMD_flashread(int argc,char *const argv[])
{
    int count = atoi(argv[argc-1]);
    int i = 0;
    float valuec;
    if(atoi(argv[argc-1])<0||atoi(argv[argc-1])>255)
    {
      printf("error:flash address is 0 to 255\r\n");
      return 1;
    }
    for(i=0;i<=count;i+=4)
    {
      valuec = flashcontrol_read(i);
      printf("flash address is %d value is %f \r\n",i,valuec);
    }
    
    return 0;
}


/*shell��������*/
SHELL_EXPORT_CMD(CMD_hello, hellorobot ,help of hello);
SHELL_EXPORT_CMD(CMD_valuesendcontrol,valuesend,help of valuesend 0 or 1);
SHELL_EXPORT_CMD(CMD_flashwrite,flashwrite,flashwrite param address and value);
SHELL_EXPORT_CMD(CMD_startswitch,startswitch,help of value is 0 or 1);
SHELL_EXPORT_CMD(CMD_flashread,flashread,max is 255)
////////////////////////////////////////////////////////////////////////////////////////////

