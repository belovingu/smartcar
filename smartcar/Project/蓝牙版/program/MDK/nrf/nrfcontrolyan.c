#include "nrfcontrolyan.h"


extern uint8_t StopMark;
extern bool valuecontrol;

#define stop_cmd 0x01
#define start_cmd 0x02
#define valuestart_cmd 0x03
#define valuestop_cmd 0x04
//#define PID_cmd 0x03


/* NRF�����ӿ����� */
static uint32_t xfer(uint8_t *buf_in, uint8_t *buf_out, uint32_t len, uint8_t cs_state)
{
    uint8_t dummy;
    
    if(!buf_in)
        buf_in = &dummy;
    if(!buf_out)
        buf_out = &dummy;
    
    while(len--)
    {
        if(len == 0)
        {
            *buf_in = SPI_ReadWriteByte(HW_SPI1, HW_CTAR0, *buf_out, 0, (SPI_PCS_Type)cs_state); 
        }
        else
        {
            *buf_in = SPI_ReadWriteByte(HW_SPI1, HW_CTAR0, *buf_out, 0, kSPI_PCS_KeepAsserted); 
        }
        if(buf_out != &dummy)
            buf_out++;
        if(buf_in != &dummy)
            buf_in++;
    }
    return len;
}

static uint32_t get_reamin(void)
{
    return 0;
}

static void ce_control(uint8_t stat)
{
  
    PEout(0) = stat;
}

const struct nrf24xx_ops_t ops = 
{
    xfer,
    get_reamin,
    ce_control,
    DelayMs,
};





/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/6/25
*     �������ƣ�nrfcontrol_isr
*     ����˵����array : �ⲿ�ж�PTE5 ��Ӧ����
*     ����˵������ȡNRF�źţ�ʵ�����ܳ�����

*/
void nrfcontrol_isr(uint32_t array)
{
    static uint8_t command_nrf[4];
    static uint32_t len_nrf;
  
    GPIO_ITDMAConfig(HW_GPIOE, 5, kGPIO_IT_Low, false);
    if(array & (1 << 5)) /*��Ӧ�İ����ж� ��ת��Ӧ��LED��ƽ */
    {
       while(nrf24l01_read_packet(command_nrf, &len_nrf));
      
       switch(command_nrf[0])
        {
         case stop_cmd: 
                StopMark = 0 ;
         
         break;
        
         case start_cmd:
                StopMark = 1;
         
         break;
         
         case valuestart_cmd:       
                valuecontrol = 1;
       
         break;
         
         case valuestop_cmd: 
                valuecontrol = 0;
         
         break;
         
         default :
           
         break;
        }

      
    }
    GPIO_ITDMAConfig(HW_GPIOE, 5, kGPIO_IT_Low, true);
}


/* ==========================================================
*     ������Ա��yan
*     ��дʱ�䣺2016/6/25
*     �������ƣ�nrfcontrol_init
*     ����˵����void
*     ����˵����NRF��ʼ��������������

*/
void nrfcontrol_init(void)
{
  
    PORT_PinMuxConfig(HW_GPIOE,1,kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOE,2,kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOE,3,kPinAlt2);
    PORT_PinMuxConfig(HW_GPIOE,4,kPinAlt2); 
    GPIO_QuickInit(HW_GPIOE,0,kGPIO_Mode_OPP);

    SPI_InitTypeDef SP;
    SP.instance = HW_SPI1;
    SP.mode = kSPI_Master;
    SP.dataSize = 8;
    SP.bitOrder = kSPI_MSB;
    SP.frameFormat = kSPI_CPOL0_CPHA0;
    SP.baudrate = 2*1000*1000;
    SP.ctar = HW_CTAR0;
    SPI_Init(&SP);

    nrf24l01_init(&ops);

    if(nrf24l01_probe())
    {
    //        printf("no nrf24xx!\r\n");
      while(1);
    }
    else
    {
    //        printf("nrf24xx ok!\r\n");
      nrf24l01_set_rx_mode();
    }

    GPIO_QuickInit(HW_GPIOE, 5, kGPIO_Mode_IPU); /* KEY */

    /* ����GPIO�ⲿ�����жϻص����� */
    GPIO_CallbackInstall(HW_GPIOE, nrfcontrol_isr);
    /* ��PTE26���ŵ��ж� �ߴ��� */
    GPIO_ITDMAConfig(HW_GPIOE, 5, kGPIO_IT_Low, true);

}
